---
tags:
  - distributed/system
status: 🌱
---

# 03-Flash Sale (秒杀系统)

> [!abstract] 核心考点：> 流量层层削峰 + Redis 原子扣减库存 + 异步订单处理，核心是"保护数据库不被打穿"

## 30 秒回答

**核心结论**：核心考点：> 流量层层削峰 + Redis 原子扣减库存 + 异步订单处理，核心是"保护数据库不被打穿"


## 需求分析

### 功能需求

| 需求 | 说明 |
|------|------|
| 商品展示 | 秒杀商品详情页（静态化） |
| 库存扣减 | 扣减需防止超卖，原子性操作 |
| 订单创建 | 秒杀成功后生成订单 |
| 支付对接 | 订单支付状态同步 |
| 限购策略 | 同一用户限购 N 件 |
| 运营管理 | 秒杀活动配置、风控规则 |

### 非功能需求

| 维度 | 指标 |
|------|------|
| 峰值 QPS | 1000 万（瞬时涌入） |
| 有效订单 | 10 万件库存，系统只需处理 10 万有效请求 |
| 库存准确率 | 100%（宁可少卖，不可超卖） |
| 响应延迟 | 秒杀按钮 < 200ms 返回结果（排队中 / 成功 / 失败） |
| 恶意请求 | 过滤 99% 的脚本、机器人 |
| 可用性 | 秒杀期间不允许宕机，允许降级 |

## 数据量估算

| 项目 | 计算 |
|------|------|
| 活动前预热 | 页面 PV ≈ 1 亿，CDN 命中率 > 95% |
| 抢购瞬时 QPS | 1000 万/s（第 1 秒），99% 请求在网关层被过滤 |
| 真实落库 QPS | 10 万/s（有效下单请求） |
| 订单总量 | 10 万（商品件数总量） |
| 数据量级 | 商品、活动、订单数据量很小（几千条商品），属于典型的高 QPS 低数据量场景 |

> **核心洞察**：秒杀系统的本质是一个**漏斗**——前端 1000 万 QPS 经过层层过滤，最终只剩 10 万真正的下单请求到达数据库。

## 核心模型

```sql
-- 商品表（提前加载到 Redis）
CREATE TABLE seckill_product (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    activity_id     BIGINT UNSIGNED NOT NULL,
    product_name    VARCHAR(255)    NOT NULL,
    original_price  DECIMAL(10,2)   NOT NULL,
    seckill_price   DECIMAL(10,2)   NOT NULL,
    total_stock     INT UNSIGNED    NOT NULL COMMENT '总库存',
    available_stock INT UNSIGNED    NOT NULL COMMENT '剩余库存',
    start_time      DATETIME        NOT NULL,
    end_time        DATETIME        NOT NULL,
    status          TINYINT         NOT NULL DEFAULT 0 COMMENT '0:待开始 1:进行中 2:已结束',
    version         INT UNSIGNED    NOT NULL DEFAULT 0 COMMENT '乐观锁版本号',
    created_at      DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 订单表（预热后预分片，按 order_id hash）
CREATE TABLE seckill_order_{shard} (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    order_no        VARCHAR(32)     NOT NULL UNIQUE COMMENT '订单号',
    user_id         BIGINT UNSIGNED NOT NULL,
    product_id      BIGINT UNSIGNED NOT NULL,
    quantity        INT UNSIGNED    NOT NULL DEFAULT 1,
    total_amount    DECIMAL(10,2)   NOT NULL,
    status          TINYINT         NOT NULL DEFAULT 0 COMMENT '0:待支付 1:已支付 2:已取消',
    create_time     DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,
    pay_time        DATETIME        DEFAULT NULL,
    INDEX idx_user (user_id, create_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

## 架构设计图

```text
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Client Layer                                                                       │
│  ┌────────────────────────┐              ┌───────────────────────────────────────┐  │
│  │  Browser / App         │              │  CDN (Static Resources)               │  │
│  └───────────┬────────────┘              └───────────────────────────────────────┘  │
└──────────────┼──────────────────────────────────────────────────────────────────────┘
               │ Flash sale button click
               ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Layer 1: Frontend Rate Limiting                                                     │
│                                                                                       │
│  ┌────────────────────────────┐    ┌──────────────────────────────────────────┐      │
│  │ Nginx + Lua (OpenResty)    │───→│ 1. Rate Limiting Module (Nginx)          │      │
│  │                            │    │    Leaky Bucket Algorithm                 │      │
│  │                            │    │    Per IP: 10 req/s                      │      │
│  └────────────────────────────┘    └──────────────────────────────────────────┘      │
│         │                                                                             │
│         └── 2. Static pages → CDN                                                    │
└─────────────────────────────────┬────────────────────────────────────────────────────┘
                                  ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Layer 2: Business Gateway                                                           │
│                                                                                       │
│  ┌────────────────────────────────────────────────────────────────────────────┐      │
│  │                            API Gateway                                    │      │
│  └────────┬───────────────────────────────┬────────────────────────┬──────────┘      │
│           │                               │                        │                 │
│           ▼                               ▼                        ▼                 │
│  ┌────────────────────┐  ┌──────────────────────────┐  ┌────────────────────────┐   │
│  │ 3. Identity Check  │  │ 4. Risk Control /        │  │ 5. Filter: Purchase   │   │
│  │    & Token Verify  │  │    Bot Detection         │  │    Limit Check +       │   │
│  │                    │  │    Behavior Recognition   │  │    Activity Time       │   │
│  │                    │  │    + Device Fingerprint   │  │    Window              │   │
│  └────────────────────┘  └──────────────────────────┘  └────────────────────────┘   │
└─────────────────────────────────┬────────────────────────────────────────────────────┘
                                  ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Layer 3: Inventory Deduction (Core Bottleneck)                                      │
│                                                                                       │
│  ┌────────────────────────────────────────────────────┐                              │
│  │  Redis Cluster: Lua Script Atomic Deduction         │                              │
│  └──────────────────────┬─────────────────────────────┘                              │
│                         │                                                             │
│              ┌──────────┴──────────┐                                                 │
│              ▼                     ▼                                                 │
│  ┌──────────────────────┐  ┌──────────────────────┐                                  │
│  │ Deduction Success   │  │ Deduction Failed     │                                  │
│  └──────────┬───────────┘  └──────────────────────┘                                  │
│             │ 6. Async order                                                          │
│             │    (write to MQ)     Return "Sold Out"                                  │
└─────────────┼────────────────────────────────────────────────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Layer 4: Async Order Processing                                                     │
│                                                                                       │
│  ┌──────────────────────────┐                                                        │
│  │ Kafka / RocketMQ         │                                                        │
│  │ (Sequential Write, Peak  │                                                        │
│  │ Shaving)                 │                                                        │
│  └──────────┬───────────────┘                                                        │
│             ▼                                                                         │
│  ┌──────────────────────────┐                                                        │
│  │ Order Consumer           │                                                        │
│  │ (Single-thread Consumer) │                                                        │
│  └──────────┬───────────────┘                                                        │
│             ▼                                                                         │
│  ┌──────────────────────────┐                                                        │
│  │ MySQL: Idempotency Check +                                                        │
│  │ Purchase Limit Re-check  │                                                        │
│  └──────────┬───────────────┘                                                        │
│             ▼                                                                         │
│  ┌──────────────────────────┐    ┌──────────────────────────┐    ┌───────────────┐   │
│  │ Generate Order (INSERT) │───→│ Update Redis Inventory   │───→│ Async Notify  │   │
│  └──────────────────────────┘    └──────────────────────────┘    │ User of Result│   │
│                                                                  └───────────────┘   │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│  Monitoring & Fallback                                                               │
│                                                                                       │
│  ┌──────────────────────────┐    ┌──────────────────────────┐                        │
│  │ Prometheus + Grafana     │───→│ Alert                    │                        │
│  │ (Full-link Monitoring)   │    │                          │                        │
│  └──────────────────────────┘    └──────────────────────────┘                        │
│                                                                                       │
│  ┌──────────────────────────┐    ┌──────────────────────────┐                        │
│  │ Sentinel / Hystrix       │───→│ Degradation Strategy:    │                        │
│  │ (Circuit Breaker /       │    │ Return "Queuing"         │                        │
│  │ Degradation)             │    │                          │                        │
│  └──────────────────────────┘    └──────────────────────────┘                        │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### 流量漏斗示意

```
客户端请求 1000 万 QPS
    ↓ Nginx 限流（拒绝 99% 重复 IP）
用户判断 10 万 QPS
    ↓ 风控过滤（踢掉脚本）
合法请求 5 万 QPS
    ↓ Redis 扣库存
库存成功 10 万（总量）
    ↓ 异步写入
订单落库 10 万
```

## 关键难点与解决方案

### 1. 超卖问题（库存准确性）

**问题**：高并发下两个线程同时读到库存为 1，都扣减成功 → 超卖。

**方案**：Redis Lua 脚本保证原子性

```lua
-- Lua 脚本原子扣减
local key = KEYS[1]          -- 库存 key: stock:{product_id}
local user_key = KEYS[2]     -- 用户限购 key: bought:{product_id}:{user_id}
local quantity = tonumber(ARGV[1])
local limit = tonumber(ARGV[2])

-- 限购检查
local bought = redis.call('GET', user_key) or 0
if tonumber(bought) + quantity > limit then
    return -2  -- 超过限购数量
end

-- 库存检查
local stock = redis.call('GET', key) or 0
if stock < quantity then
    return -1  -- 库存不足
end

-- 原子扣减
redis.call('DECRBY', key, quantity)
redis.call('INCRBY', user_key, quantity)
redis.call('EXPIRE', user_key, 86400)  -- 24h 有效期
return 0  -- 成功
```

**为什么不用 MySQL 行锁**：MySQL 行锁在高并发下吞吐量极低（~1000 tps），而 Redis 单机可达 10 万+ tps。

### 2. 流量尖峰削平

**问题**：1000 万 QPS 瞬间打到服务端 → 服务雪崩。

**分层削峰策略**：

| 层级 | 手段 | 削峰比例 |
|------|------|---------|
| 浏览器 | 按钮置灰 + 随机延迟 + 验证码 | ~50% |
| Nginx | 漏桶限流 + IP 黑名单 | ~90% |
| 网关 | Token 校验 + 风控过滤 | ~50% |
| 消息队列 | FIFO 排队 + 限速消费 | 100% |

**核心思路**：所有请求先排队（Kafka/RocketMQ），消费者以可控速度处理。用户端返回"排队中"，通过轮询或 WebSocket 异步通知结果。

### 3. 风控与反作弊

| 攻击手段 | 检测方案 | 处理措施 |
|---------|---------|---------|
| 脚本高频刷 | IP + UA + 行为特征 | Nginx 限流 + 拉黑 |
| 多账号刷单 | 设备指纹 + 手机号关联 | 风控系统打分，低分拒绝 |
| 提前抢跑 | 动态 Token + 时间戳签名 | 请求中附带秒杀令牌 |
| 绕过前端直接调 API | Token + 签名校验 | 接口防重放（Nonce + Timestamp） |

**秒杀令牌方案**：
```
客户端预加载页面时获取 Token（限量发放）
秒杀请求携带 Token → 服务端校验 Token 有效性（Redis 存储）
Token 用完即废，防止囤积
```

### 4. 降级与兜底策略

| 场景 | 降级动作 |
|------|---------|
| Redis 宕机 | 降级到 MySQL 乐观锁（吞吐下降但可用） |
| 消息队列堆积 | 立即熔断新请求，用户端展示"拥挤" |
| 订单系统过载 | 只完成库存扣减 + 消息落盘，延后处理订单 |
| 支付超时 | 自动取消订单，释放库存回 Redis |

## 面试追问

| 追问方向 | 参考回答 |
|---------|---------|
| **为什么用 Lua 脚本而不是 Redis 事务（MULTI/EXEC）？** | Lua 脚本在 Redis 中原子执行，且中间可以嵌入业务逻辑（如限购判断）。Redis 事务仅保证隔离性不保证回滚，且不支持条件判断。Lua 是秒杀扣减的业界标准方案 |
| **消息队列消费时，消费者挂了怎么办？** | 关键在于消费后提交 offset 的时机。先处理完业务逻辑再提交 offset，如果消费者在业务处理中崩溃，重启后会重新消费（至少一次语义），通过订单幂等表去重 |
| **怎么实现库存"回滚"？** | 用户超时未支付 → 定时任务扫描 + 支付回调通知 → 异步回滚库存（Redis INCR）。注意回滚和超卖：在回滚时加 Check，确保库存不超过总库存 |
| **静态资源 CDN 预热怎么做？** | 活动开始前将商品详情页全量推送到 CDN 节点。商品详情中动态部分（如倒计时）通过 AJAX 请求获取，避免页面级别的回源 |
| **如果 Redis 是单点瓶颈，怎么扩展？** | 商品库存可以按商品 ID 分片到不同 Redis 节点（一致性哈希）。秒杀是单个热点商品的场景，单个商品的库存只能在单节点操作（Lua 脚本需要单个 key），此时只能通过异步排队降低压力 |
| **怎么测试秒杀系统的正确性？** | ① 单元测试：Lua 脚本的各种边界（库存=0、超量扣减）；② 压力测试：jmeter 模拟 10 倍预估并发；③ 正确性验证：压测后检查 sum(已售) <= total_stock；④ Chaos Engineering：随机 Kill 服务验证库存不丢 |
| **下单 Token 怎么保证不提前泄漏？** | Token 含时间戳和签名（HMAC-SHA256），服务端验证时间窗口（±5s）。Token 在秒杀开始时才生效，提前预发的 Token 在活动时间前被拒绝 |

秒杀系统缓存与消息队列详解见 → [Cache Penetration / Breakdown / Avalanche](07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐.md) · [Kafka Message Delivery Guarantees](07-Distributed%20&%20Middleware%20(分布式与中间件)/03-Message%20Queues%20(消息队列)/03b-Kafka%20Architecture%20Overview%20(Kafka架构概览)/03b2-Message%20Delivery%20Guarantees%20(消息可靠性).md)



## 零基础阅读路径

先将本页结论转换成自己的 30 秒表达；再补一个具体约束和项目证据；最后练习反例与取舍，避免把速记当成理解。

## 常见误区

- 只背标准答案，不把结论放进真实约束、取舍和项目证据中，表达会显得空泛。
- 不计时复述、不追问反例，也不通过项目日志或代码核对，就会形成虚假熟练。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Flash Sale (秒杀系统)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
