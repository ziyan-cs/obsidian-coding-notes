---
tags:
  - distributed/system
status: 🌱
---

> [!important] **核心考点**
> 发号器 + 哈希映射 + HTTP 重定向，重点在唯一 ID 生成策略和缓存加速

> 本文为面试/系统设计参考；动手实现与证据见 [[12-Backend Projects (后端项目)/02-URL Shortener (短链接服务)|URL Shortener 项目]]。

## 需求分析

### 功能需求

| 需求 | 说明 |
|------|------|
| 长链接转短链接 | 输入长 URL，生成唯一短 key |
| 短链接重定向 | 访问短链接时 301/302 跳转回原始 URL |
| 过期策略 | 支持 TTL，过期自动清理 |
| 访问统计 | 记录点击次数、来源 IP、UA、时间分布 |
| 自定义短链 | 允许用户自定义 alias（需检查唯一性） |

### 非功能需求

| 维度 | 指标 |
|------|------|
| QPS 写入 | 平均 30/s，峰值 200/s（用户主动创建） |
| QPS 读取 | 平均 3000/s，峰值 20000/s（每次点击都是读） |
| 可用性 | 99.99%（写不可用可接受，读必须高可用） |
| 延迟 | 写入 < 200ms，重定向 < 50ms（含网络） |
| 持久性 | 数据不丢失，但少量数据抖动可接受 |

## 数据量估算

| 项目 | 计算 |
|------|------|
| 年新增 URL | 1 亿条 |
| 短 key 长度 | 6~8 位 base62（62^8 ≈ 218 万亿，足够） |
| 存储总量 | 1 年 ≈ 1 亿 × 1KB = 100 GB，5 年 ≈ 500 GB |
| 读放大 | 读写比 ≈ 100:1，缓存需扛 20K QPS |
| 带宽 | 重定向响应 ≈ 500 bytes，20K QPS × 500B ≈ 10 MB/s |

## 核心模型

```sql
-- 核心表：URL 映射
CREATE TABLE url_mapping (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY COMMENT '自增主键/发号器源',
    short_key   VARCHAR(8)  NOT NULL UNIQUE KEY    COMMENT '短链 key（base62 编码）',
    original_url TEXT       NOT NULL                COMMENT '原始长链接',
    expire_at   DATETIME    DEFAULT NULL            COMMENT '过期时间，NULL 永不过期',
    created_at  DATETIME    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at  DATETIME    NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_short_key (short_key),
    INDEX idx_expire (expire_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 访问日志表（分表，按短链 hash 或日期）
CREATE TABLE access_log (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    short_key   VARCHAR(8)  NOT NULL,
    ip          VARCHAR(45) NOT NULL,
    user_agent  TEXT,
    referer     TEXT,
    country     VARCHAR(64),
    click_time  DATETIME    NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_short_time (short_key, click_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

## 架构设计图

```text
┌──────────────────────────────────────────────────────────────────────────────────┐
│                         User Browser                                             │
└────────────────────────────────┬─────────────────────────────────────────────────┘
                                 │
                                 ▼
┌──────────────────────────────────────────────────────────────────────────────────┐
│                         DNS / CDN                                                │
└────────────────────────────────┬─────────────────────────────────────────────────┘
                                 │
                                 ▼
┌──────────────────────────────────────────────────────────────────────────────────┐
│                         Nginx Reverse Proxy (LVS + Keepalived)                   │
└────────────────────────────────┬─────────────────────────────────────────────────┘
                                 │
                                 ▼
┌──────────────────────────────────────────────────────────────────────────────────┐
│                         Web Service (Stateless, Horizontal Scaling)              │
└──────┬──────────────────────────────────────────────────────────────────┬─────────┘
       │                                                                  │
       │  GET /{short_key}                                          Write Path
       │  (Read Path)                                               (Low Freq)
       │                                                                 │
       ▼                                                                 ▼
┌──────────────────┐                                            ┌──────────────────┐
│                  │                                            │  ID Generator    │
│   Redis Cache    │                                            │  Snowflake /     │
│                  │                                            │  Redis INCR      │
└──────┬───────────┘                                            └────────┬─────────┘
       │                                                                 │
  ┌────┴────┐                                                     ┌──────┴──────┐
  │         │                                                     │             │
  ▼         ▼                                                     ▼             │
┌────┐ ┌────────┐                                          ┌──────────────┐    │
│Hit │ │  Miss  │                                          │  Base62 Encode│    │
└──┬─┘ └───┬────┘                                          │  → short_key │    │
   │       │                                               └──────┬───────┘    │
   ▼       ▼                                                      │            │
┌──────┐ ┌───────────┐                                           ▼            │
│301/302│ │  MySQL    │                                    ┌────────────────┐ │
│Redirect│ │  Fallback│                                    │  MySQL         │ │
└───────┘ │  Query   │                                    │  (url_mapping) │ │
          └─────┬─────┘                                    └───────┬────────┘ │
                │                                                  │          │
                ├──→ Redis Cache (populate)                       │          │
                │                                                  ▼          │
                ├──→ Redirect                              ┌────────────────┐ │
                                                           │  Redis Cache   │ │
                                                           │  (Pre-warm)    │ │
                                                           └────────────────┘ │
                                                                              │
┌──────────────────────────────────────────────────────────────────────────────┤
│  Async Pipeline                                                             │
│                                                                             │
│  ┌────────────────┐    ┌──────────────┐    ┌──────────────────────────┐     │
│  │ Click Event    │───→│ Kafka /      │───→│ Access Log Consumer     │     │
│  │ Logging        │    │ Message Queue│    └──────────┬───────────────┘     │
│  └────────────────┘    └──────────────┘               │                     │
│                                                  ┌────┴────┐                │
│                                                  ▼         ▼                │
│                                           ┌──────────┐ ┌──────────┐         │
│                                           │ClickHouse│ │ Redis    │         │
│                                           │ / ES     │ │ Counter  │         │
│                                           │(Analysis)│ │(Real-time│         │
│                                           └──────────┘ │ Click    │         │
│                                                        │ Count)   │         │
│                                                        └──────────┘         │
├──────────────────────────────────────────────────────────────────────────────┤
│  Scheduled Tasks                                                             │
│                                                                             │
│  ┌─────────────────┐    ┌──────────────────────────┐    ┌───────────────┐   │
│  │ Cron Job /       │───→│ Scan expire_at < NOW()  │───→│ Delete Redis  │   │
│  │ TTL Scanner      │    └──────────────────────────┘    │ Cache         │   │
│  └─────────────────┘                                     └──────┬────────┘   │
│                                                                 │            │
│                                                                 ▼            │
│                                                          ┌───────────────┐   │
│                                                          │ Archive to    │   │
│                                                          │ Cold Storage / │   │
│                                                          │ Physical Delete │   │
│                                                          └───────────────┘   │
└──────────────────────────────────────────────────────────────────────────────┘
```

### 关键设计说明

**读链路**是性能的核心瓶颈（读写比 100:1），所以：
1. **Redis 缓存**扛 99% 的重定向请求，缓存 key = `short_url:{short_key}`，value = 原始 URL
2. **回源限速**：对缓存未命中使用 singleflight 防止缓存击穿
3. **HTTP 301 vs 302**：默认 301（浏览器永久缓存，减少回源），需要计数的场景用 302

## 关键难点与解决方案

### 1. 短 Key 生成策略

| 方案 | 优点 | 缺点 | 结论 |
|------|------|------|------|
| **发号器 + Base62** | 无冲突、定长、可排序 | 需要独立发号器服务 | ✅ **推荐** |
| MD5 取前 N 位 | 去中心化 | 有碰撞概率（需额外处理） | 可作备选 |
| UUID 压缩 | 去中心化 | 太长（32 位 hex） | 不适用 |
| 随机生成 + 查重 | 简单 | 碰撞重试开销随数据量增大 | 不可用 |

**最终方案**：Snowflake 生成 64-bit ID → Base62 编码为 7~8 位短 key。

Snowflake 分片：`1-bit reserved + 41-bit timestamp + 10-bit worker + 12-bit sequence`

```
Snowflake ID: 0 | 41-bit timestamp | 10-bit worker | 12-bit sequence
```
每秒单机 4096 个 ID，完全够用。

### 2. 缓存击穿与雪崩

- **击穿**：缓存过期瞬间大量并发请求回源 → 用 singleflight / mutex 只让一个请求回源
- **雪崩**：大量 key 同时过期 → 过期时间加随机偏移（base TTL + random(0, 3600)）
- **热点**：某个短链突然爆火 → 本地缓存 + Redis 热点探测

### 3. 短链遍历安全

- 发号器连续可能导致遍历抓取
- **方案**：发号器步长随机步进，或每发一定数量后跳变 segment
- 业务层做频率限制（同 IP 查询频率限制、User-Agent 校验）

### 4. 过期清理机制

- **策略**：惰性删除（访问时检查）+ 定时扫描 + 预归档
- 每天扫描一次过期记录，软删除后批量归档到冷存储（S3 / HDFS）
- Redis 层面：使用 TTL 特性让 key 自动过期

## 面试追问

| 追问方向 | 参考回答 |
|---------|---------|
| **为什么选 Base62 而不是 Base64？** | Base64 包含 `+` 和 `/` 符号，在 URL 中需要转义，Base62（a-z A-Z 0-9）天然 URL-safe |
| **怎么处理长 URL 相同时是否生成同一个短链？** | 两种方案：① 每次生成不同（统计意义上独立）；② 查重后复用（需额外查重表，写入慢）。建议按业务场景区分：同一用户可生成多个，防止恶意擦除 |
| **如何支持自定义短链？** | 用户指定 alias 时写入 Reserve 表，先检查唯一性。实现上可以做两层：优先读自定义表，再读取自动生成表 |
| **Redis 存储短链需要多少内存？** | 1 亿条短链，key ≈ 20 字节（`short_url:abc12345`），value ≈ 100 字节（原始 URL），1 亿 × 120B ≈ 12 GB，单机 Redis 完全可放 |
| **301 和 302 的区别？业务上怎么选？** | 301 永久重定向，浏览器缓存，减少回源但无法计数；302 临时重定向，每次请求都到服务端。需要点击统计的用 302，纯跳转用 301 |
| **怎么估算短链系统的写 QPS？** | 假设 Twitter/X 日活 3 亿，每天新增短链 5 千万，日间 12 小时均匀分布 ≈ 50M / 43200s ≈ 1157/s。多数业务系统低于此数量级，10~100 QPS 写入即可 |
| **一致性哈希在短链系统中的作用？** | 通常不需要，短链系统无状态，前端 Nginx 直接轮询即可。但如果要做用户级分片存储，可按 user_id 一致性哈希分库 |

短 URL 系统分布式与缓存基础详解见 → [CAP Theorem & BASE Theory](08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) · [Redis Cache Patterns](08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐.md)
