---
tags:
  - distributed/system
status: 🌱
---

> [!important] **核心考点**
> 路由转发 + 过滤器链（鉴权/限流/熔断/日志）+ 高性能代理设计，核心是看你对"网关层职责边界"的理解

## 需求分析

### 功能需求

| 需求 | 说明 |
|------|------|
| 路由转发 | 根据 path / header / method 将请求路由到对应后端服务 |
| 负载均衡 | 支持轮询、最小连接数、一致性哈希 |
| 鉴权认证 | JWT / OAuth2 Token 校验，API Key 认证 |
| 限流 | 按用户 / IP / API 维度限流（令牌桶 / 滑动窗口） |
| 熔断降级 | 后端异常时自动熔断，返回降级响应 |
| 请求/响应转换 | Header 改写、Body 格式转换（JSON ↔ XML） |
| 日志与监控 | 全量请求链路日志 + 指标上报（延迟 / 状态码 / 流量） |
| 灰度发布 | 按 Header / Cookie / IP 比例路由到新版本服务 |

### 非功能需求

| 维度 | 指标 |
|------|------|
| 代理延迟增加 | < 5ms（P99，网关层额外开销） |
| 吞吐量 | 单机 10 万 QPS（纯转发） |
| 路由规则 | 支持 1000+ 条规则，动态更新无重启 |
| 可用性 | 99.999%（网关是流量入口，挂则全站不可用） |
| 水平扩展 | 无状态，加机器线性提升吞吐 |
| 配置热更新 | 路由 / 限流 / 熔断规则修改后秒级生效 |

## 数据量估算

| 项目 | 计算 |
|------|------|
| 入口 QPS | 100 万 / s（总站入口流量） |
| 后端服务 | 200 ~ 500 个微服务 |
| 路由规则 | 500 ~ 1000 条 |
| 每条请求大小 | 平均 2 KB（含 headers） |
| 网关集群 | 20 ~ 50 台机器（单机 2~5 万 QPS） |
| 延迟预算 | 网络 RTT 30ms + 网关 5ms + 业务 100ms ≈ 135ms |

> **关键洞察**：网关是"零号服务"——它的每一毫秒延迟都会被所有下游放大。性能优化的优先级高于功能丰富度。

## 核心模型

```sql
-- 路由规则表（定时同步到网关内存）
CREATE TABLE route_rule (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(64)     NOT NULL COMMENT '路由名称',
    path_pattern    VARCHAR(255)    NOT NULL COMMENT '路径匹配模式，如 /api/v1/users/**',
    methods         VARCHAR(32)     DEFAULT 'GET,POST,PUT,DELETE' COMMENT '允许的 HTTP 方法',
    target_service  VARCHAR(128)    NOT NULL COMMENT '目标服务名（服务发现用）',
    target_path     VARCHAR(255)    DEFAULT NULL COMMENT '路径重写规则，支持模板变量',
    strip_prefix    TINYINT         DEFAULT 1 COMMENT '是否去除前缀',
    match_priority  INT             NOT NULL DEFAULT 0 COMMENT '匹配优先级，越大越优先',
    status          TINYINT         NOT NULL DEFAULT 1,
    filter_chain    VARCHAR(512)    DEFAULT 'rate_limit,auth,log' COMMENT '过滤器链',
    timeout_ms      INT UNSIGNED    NOT NULL DEFAULT 30000,
    retry_count     TINYINT         NOT NULL DEFAULT 0,
    created_at      DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_name (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 限流规则表
CREATE TABLE rate_limit_rule (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    route_id        BIGINT UNSIGNED DEFAULT NULL COMMENT 'NULL 表示全局规则',
    dimension       VARCHAR(16)     NOT NULL COMMENT 'ip / user / api / global',
    limit_count     INT UNSIGNED    NOT NULL COMMENT '限制次数',
    window_seconds  INT UNSIGNED    NOT NULL COMMENT '时间窗口（秒）',
    algorithm       VARCHAR(16)     NOT NULL DEFAULT 'token_bucket' COMMENT 'token_bucket / sliding_window',
    status          TINYINT         NOT NULL DEFAULT 1,
    created_at      DATETIME        NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 熔断规则表
CREATE TABLE circuit_breaker_rule (
    id              BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    route_id        BIGINT UNSIGNED NOT NULL,
    failure_threshold   INT UNSIGNED NOT NULL DEFAULT 10 COMMENT '窗口内失败次数阈值',
    success_threshold   INT UNSIGNED NOT NULL DEFAULT 5  COMMENT '半开后成功次数阈值',
    window_seconds      INT UNSIGNED NOT NULL DEFAULT 30,
    half_open_timeout   INT UNSIGNED NOT NULL DEFAULT 10 COMMENT '半开超时（秒）',
    status          TINYINT NOT NULL DEFAULT 1
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

## 架构设计图

```text
┌────────────────────────────────────────────────────────────────────────────────────────────┐
│  Client Layer                                                                              │
│  ┌──────────────────────────────┐    ┌────────────────────────────────────────────────┐    │
│  │  Client App / Browser        │    │  DNS / CDN                                     │    │
│  └──────────────────────────────┘    └────────────────────────────────────────────────┘    │
└───────────────────────────────────────┬────────────────────────────────────────────────────┘
                                        │
                                        ▼
┌────────────────────────────────────────────────────────────────────────────────────────────┐
│  Entry Point                                                                               │
│  ┌────────────────────────────────────────────┐                                            │
│  │  Global Load Balancer (F5 / DNS Round-     │                                            │
│  │  Robin)                                    │                                            │
│  └──────────────────┬─────────────────────────┘                                            │
│                     ▼                                                                      │
│  ┌────────────────────────────────────────────┐                                            │
│  │  API Gateway Cluster (Stateless,           │                                            │
│  │  Horizontal Scaling)                       │                                            │
│  └──────────────────┬─────────────────────────┘                                            │
└─────────────────────┼──────────────────────────────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│  Gateway Core Architecture (Filter Chain)                                                   │
│                                                                                             │
│  ┌───────────────────────────────────────────────────────────────────────────────────────┐  │
│  │                      Core (High-Performance Proxy Engine)                             │  │
│  │                                                                                       │  │
│  │  ┌─────────────┐    ┌────────────┐    ┌────────────────┐    ┌──────────────────────┐  │  │
│  │  │ TLS         │───→│ Router     │───→│ Filter Chain   │───→│ Proxy (Reverse       │  │  │
│  │  │ Termination │    │ (Radix     │    │ (Chain-of-     │    │ Proxy & Load         │  │  │
│  │  │             │    │  Tree      │    │  Responsibi-   │    │ Balancer)            │  │  │
│  │  └─────────────┘    │  Match)    │    │  lity Pattern) │    └──────────────────────┘  │  │
│  │                     └────────────┘    └────────────────┘                              │  │
│  │                                                                                       │  │
│  │  ┌───────────────────────────────────────────────────────────────────────────────┐    │  │
│  │  │                        Filter Chain (ordered execution)                       │    │  │
│  │  │                                                                               │    │  │
│  │  │  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────────────┐     │   │  │
│  │  │  │ Auth Filter      │  │ Rate Limit       │  │ Circuit Breaker Filter  │    │   │  │
│  │  │  │ JWT / OAuth /    │  │ Filter           │  │ State Machine           │    │   │  │
│  │  │  │ API Key          │  │ Token Bucket /   │  │ (Closed / Open /         │    │   │  │
│  │  │  │                  │  │ Sliding Window / │  │  Half-Open)              │    │   │  │
│  │  │  │                  │  │ Redis            │  │                          │    │   │  │
│  │  │  └──────────────────┘  └──────────────────┘  └──────────────────────────┘    │   │  │
│  │  │                                                                               │   │  │
│  │  │  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────────────┐    │   │  │
│  │  │  │ Log Filter       │  │ Transform Filter │  │ Gray Release Filter      │    │   │  │
│  │  │  │ Full Request/    │  │ Header / Body    │  │ Traffic Coloring /       │    │   │  │
│  │  │  │ Response Logging │  │ Rewriting        │  │ Version Routing          │    │   │  │
│  │  │  └──────────────────┘  └──────────────────┘  └──────────────────────────┘    │   │  │
│  │  └───────────────────────────────────────────────────────────────────────────────┘   │  │
│  └──────────────────────────────────────────────────────────────────────────────────────┘  │
└─────────────────────┬──────────────────────────────────────────────────────────────────────┘
                      │
                      ▼
┌────────────────────────────────────────────────────────────────────────────────────────────┐
│  Backend Services                                                                          │
│                                                                                            │
│  ┌──────────────────────────┐  ┌──────────────────────────┐  ┌──────────────────────────┐  │
│  │ Service A (User Service) │  │ Service B (Order Service)│  │ Service C (Product       │  │
│  └──────────────────────────┘  └──────────────────────────┘  │  Service)                │  │
│                                                              └──────────────────────────┘  │
└────────────────────────────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────────────────────────────────┐
│  Control Plane                          │  Observability                                   │
│                                         │                                                  │
│  ┌────────────┐  ┌──────────────────┐   │  ┌──────────────────────────────────────────┐    │
│  │ Admin API  │→ │ Config Service   │   │  │ Prometheus (QPS / Latency / Status)      │    │
│  └────────────┘  └───────┬──────────┘   │  ├──────────────────────────────────────────┤    │
│                          │              │  │ Jaeger / Zipkin (Distributed Tracing)    │    │
│                          ▼              │  ├──────────────────────────────────────────┤    │
│  ┌─────────────────────────────────┐    │  │ ELK / Loki (Log Center)                  │    │
│  │ Dynamic Rule Distribution       │    │  └──────────────────────────────────────────┘    │
│  │ (etcd / Apollo)                 │    │                                                  │
│  └────────────────┬────────────────┘    │                                                  │
│                   │ (async update)      │                                                  │
│                   └──→ Gateway Cluster  │                                                  │
└────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 过滤器链执行流程

```text
Client              API Gateway          Rate Limiter        Auth Service        Circuit Breaker      Backend Service
  │                     │                     │                   │                    │                    │
  ├── 1. HTTP Request ─→│                     │                   │                    │                    │
  │                     ├── 2. Route Match   │                   │                    │                    │
  │                     │   (/api/v1/orders  │                   │                    │                    │
  │                     │    → order-service)│                   │                    │                    │
  │                     ├── 3. Rate Limit    │                   │                    │                    │
  │                     │   Check (IP+User) ─→│                   │                    │                    │
  │                     │◄── 4. Pass ────────┤                   │                    │                    │
  │                     │    / 429 Reject     │                   │                    │                    │
  │                     ├── 5. JWT           │                   │                    │                    │
  │                     │   Validation ────────────────────────→│                    │                    │
  │                     │◄── 6. user_id, ───────────────────────┤                    │                    │
  │                     │   roles            │                   │                    │                    │
  │                     ├── 7. Circuit       │                   │                    │                    │
  │                     │   Breaker State? ──────────────────────────────────────────→│                    │
  │                     │◄── 8. Closed ──────────────────────────────────────────────┤                    │
  │                     │    / Open → 503    │                   │                    │                    │
  │                     ├── 9. Proxy to      │                   │                    │                    │
  │                     │   order-service ──────────────────────────────────────────────────────────────→│
  │                     │◄── 10. Response ──────────────────────────────────────────────────────────────┤
  │                     ├── 11. Post-filters │                   │                    │                    │
  │                     │   (logging,        │                   │                    │                    │
  │                     │    metrics)        │                   │                    │                    │
  │◄── 12. Response ────┤                    │                   │                    │                    │
  │      to Client       │                     │                   │                    │                    │
```

## 过滤器类型对比

| 过滤器 | 作用 | 实现方式 | 性能影响 |
|--------|------|---------|---------|
| 鉴权 | JWT 校验、API Key 校验 | 解析 Token → Redis 查黑名单 → 放行 | 1~2ms（含 Redis） |
| 限流 | 防止单个用户打爆服务 | 令牌桶（Lua + Redis） | 0.5~1ms（Redis 请求） |
| 熔断 | 下游故障时快速失败 | 滑动窗口计数 + 状态机 | 1~2ms（本地内存） |
| 日志 | 全量审计日志 | 异步写 Kafka | < 0.1ms（异步） |
| 转换 | Header / Body 改写 | 内存操作 | 0.1~1ms |
| 灰度 | 按规则分流到新版本 | 解析 Header/Cookie → 查路由表 | 0.5ms |

## 关键难点与解决方案

### 1. 高性能代理的实现

**问题**：网关作为流量入口，每增加 1ms 延迟都会放大到所有下游。

**优化手段**：

| 维度 | 方案 | 效果 |
|------|------|------|
| IO 模型 | epoll + Reactor / io_uring | 单机 10 万 + 连接 |
| 路由匹配 | Radix Tree（前缀树） vs 正则遍历 | 1000 条规则 O(k) 匹配 |
| 零拷贝 | splice / sendfile | 减少内核态用户态切换 |
| 内存复用 | 对象池复用 Request/Response Buffer | 减少 GC 压力 |
| 连接池 | 与后端服务的连接复用 | 减少 TCP 建连开销 |
| TLS 卸载 | 硬件加速卡 / kernel TLS | TLS 握手从 5ms → 0.5ms |

**路由匹配选择**：
```
Nginx 风格：前缀匹配 + 正则捕获 → Radix Tree 实现 O(n) → O(log n)
Kong (OpenResty)：lua-resty-router
Spring Cloud Gateway：AntPathMatcher (正则)
推荐：Radix Tree / Trie 实现，支持通配符和路径参数
```

### 2. 限流算法的选择

| 算法 | 原理 | 优点 | 缺点 |
|------|------|------|------|
| **令牌桶** | 固定速率生成令牌，取令牌通行 | 允许突发流量，平滑 | 需维护定时器 |
| **漏桶** | 固定速率出水，超量丢弃 | 严格限制速率 | 不能处理突发 |
| **滑动窗口** | 时间窗口内计数 | 实现简单，精度可控 | 边界精度与存储权衡 |
| **计数器** | 固定窗口计数 | 最简单 | 窗口边界突刺 |

**推荐方案**：令牌桶（Redis Lua 实现，分布式限流）

```lua
-- Redis 令牌桶 Lua
local key = KEYS[1]             -- rate_limit:{route_id}:{user_id}
local rate = tonumber(ARGV[1])  -- 每秒令牌数，如 100
local burst = tonumber(ARGV[2]) -- 桶容量，如 200
local now = tonumber(ARGV[3])
local cost = tonumber(ARGV[4])  -- 本次消耗令牌数，通常为 1

local bucket = redis.call('HGETALL', key)
local last_tokens = burst
local last_refill = now

if #bucket > 0 then
    last_tokens = tonumber(bucket[2])
    last_refill = tonumber(bucket[4])
end

-- 计算时间窗口内恢复的令牌
local elapsed = math.max(0, now - last_refill)
local tokens_to_add = math.floor(elapsed * rate / 1000)
local current_tokens = math.min(burst, last_tokens + tokens_to_add)

if current_tokens >= cost then
    redis.call('HMSET', key, 'tokens', current_tokens - cost, 'last_refill', now)
    redis.call('EXPIRE', key, 10)
    return 1  -- 通过
else
    return 0  -- 拒绝
end
```

### 3. 熔断器状态机

**三种状态 + 计数滑动窗口**：

```
        ┌──────────────────────────────┐
        │                              │
        ▼   失败 ≥ threshold           │
  ┌──────────┐      ┌──────────┐       │
  │  Closed   │─────►│   Open   │       │
  │ (正常)    │      │ (断开)   │       │
  └──────────┘      └──────────┘       │
        ▲               │              │
        │   超时        │              │
        │   ┌──────────┐│              │
        │   │ Half-Open│◄──────────────┘
        │   │ (半开)   │
        │   └──────────┘
        │       │
        └───────┘
         成功 ≥ threshold
```

- **Closed**：正常代理，统计滑动窗口内的失败率
- **Open**：请求直接返回 503（不转发到后端），超时后进入 Half-Open
- **Half-Open**：放行少量试探请求，成功则回到 Closed，失败则回到 Open

### 4. 动态路由热更新

**问题**：路由规则变更不能重启网关。

**方案**：
1. 规则存储在 etcd / 配置中心
2. 网关启动时全量加载规则到内存 Radix Tree
3. etcd Watch 订阅变更，收到变更事件后原子的 Swap 路由树指针
4. 新请求走新路由，旧请求继续走旧路由（无中断切换）

```go
// Go 无锁热更新路由表
var currentRouter atomic.Value

func watchRouteChanges() {
    for change := range watcher.EventChan() {
        newRouter := buildRouter(loadAllRules())
        currentRouter.Store(newRouter)
    }
}

func handleRequest(ctx *Context) {
    router := currentRouter.Load().(*Router)
    route := router.Match(ctx.Path)
    // ...
}
```

### 5. 网关高可用设计

| 场景 | 策略 |
|------|------|
| 网关进程挂了 | 上层 LVS / Nginx 自动剔除，客户端重试 |
| 后端服务超时 | 熔断器 Open，返回合理降级响应（JSON + 503） |
| 限流 Redis 挂了 | 降级到本地单机限流（不完美但可用） |
| 鉴权服务挂了 | 允许本地缓存的 Token 继续有效（忽略下线校验） |
| 配置中心挂了 | 网关内存中的路由规则不变，正常运行 |

## 面试追问

| 追问方向 | 参考回答 |
|---------|---------|
| **网关 vs 普通反向代理（Nginx）的区别？** | Nginx 是 L7 反向代理，侧重性能；API 网关附加了鉴权、限流、熔断、转换、灰度等业务功能。实践中常在 Nginx 之后部署网关（Nginx 负责 TLS 卸载 + 静态资源，网关负责业务路由和策略） |
| **网关怎么处理大文件上传（1GB+）？** | 流式转发，不在网关内存中缓存完整 Body。使用 chunked transfer encoding，边接收边转发。避免内存 OOM |
| **网关的鉴权怎么做才能不成为性能瓶颈？** | ① JWT 无状态鉴权（本地解析 + 缓存公钥）；② Token 黑名单用 Redis Bloom Filter；③ 敏感操作才回源鉴权服务校验；④ 分层的 token：短期 access token（1h 本地校验），长期 refresh token（需要回源） |
| **怎么保证网关不会因为后端慢而耗尽连接？** | ① 每个路由设置独立的连接池上限；② 后端服务级别的超时控制（connect / read / write 分别设超时）；③ 熔断器防止故障传导；④ 管理后台的过载保护（adaptive concurrency limit） |
| **网关重启时如何处理正在处理中的请求？** | 优雅关闭（graceful shutdown）：① 关闭监听端口（不再接受新请求）；② 等待所有 inflight 请求完成（最多等待 max_wait_seconds）；③ 超过等待时间的请求返回 502；④ 进程退出 |
| **Kong / Spring Cloud Gateway / Nginx + Lua 各有什么优劣？** | Kong：功能丰富、生态好，但性能受 Lua 限制；Spring Cloud Gateway：Java 生态集成好，适合 Spring 技术栈，但 Java 内存占用高；Nginx + Lua（OpenResty）：性能最强，适合流量入口，但 Lua 开发效率低。选型取决于团队技术栈和性能要求 |
| **网关的限流信息和鉴权 Token 怎么在微服务间传递？** | 通过请求 Header 透传：`X-User-Id`、`X-User-Roles`、`X-Request-Id`、`X-Gray-Tag`。微服务信任网关（网关在 Header 中加入签名，防止客户端伪造） |
| **全链路追踪在网关层如何实现？** | 网关生成 Trace ID（`X-Request-Id` / `X-Trace-Id`），透传给所有下游。使用 OpenTelemetry SDK 集成 Jaeger / Zipkin。网关层记录 span：网关收到请求 → 过滤器执行 → 后端请求 → 响应返回 |

API Gateway 路由与网络安全详解见 → [Reverse Proxy & Load Balancing](08-Distributed%20&%20Middleware%20(分布式与中间件)/02%20·%20Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md) · [HTTPS & TLS Overview](05-Network%20Programming%20(网络编程)/03%20·%20HTTP与应用层/07-HTTPS%20&%20TLS%20Overview%20(HTTPS原理概览).md)
