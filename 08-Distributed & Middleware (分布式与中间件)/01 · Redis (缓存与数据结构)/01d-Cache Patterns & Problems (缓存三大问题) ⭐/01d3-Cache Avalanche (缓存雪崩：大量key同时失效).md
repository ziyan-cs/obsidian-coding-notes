---
tags:
  - distributed
  - redis
---

> **核心考点**：缓存雪崩的原因、过期时间加随机化、多级缓存、降级与限流

## 缓存雪崩

大量 key 在同一时间过期失效，或 Redis 节点宕机，导致大量请求直达数据库。

```
正常：  Redis（命中）← 请求 → DB 低负载
雪崩：  Redis（大量 key 过期/宕机）→ 请求穿透 → DB 被打垮
                                   → 级联故障
```

### 对比缓存击穿

| | 缓存击穿 | 缓存雪崩 |
|--|---------|---------|
| 范围 | 单个热点 key | 大量 key / 整个 Redis 不可用 |
| 原因 | 热点 key 过期 | 多 key 同时过期 / Redis 宕机 |
| 影响 | 数据库压力上升 | 数据库直接被冲垮 |

---

## 解决方案

### 方案 1：过期时间加随机化

```cpp
// 避免大量 key 在同一秒过期
int ttl = 3600 + rand() % 600;  // 基础 1h + 随机 0-10min
redis.setex(key, ttl, value);

// 或者用固定时间 + 随机偏移
redis.setex(key, 3600, value);
redis.expire(key, ttl);  // 重设随机 TTL
```

### 方案 2：多级缓存

```
用户请求
    ↓
Level 1：本地缓存（Caffeine/LRU） ≈ 响应时间 1ms
    ↓（未命中）
Level 2：Redis 集群              ≈ 响应时间 5ms
    ↓（未命中）
Level 3：数据库 / 降级处理       ≈ 响应时间 50ms+
```

**本地缓存（如 Caffeine）：** 每个应用节点本地缓存热点数据，即使 Redis 不可用，本地缓存仍生效。缺点是各节点缓存不一致。

### 方案 3：限流与降级

```cpp
// 请求限流（令牌桶 / 漏桶）
string getWithDegrade(string key) {
    if (!rateLimiter.tryAcquire()) {
        // 触发限流：返回降级数据
        return getDegradeData(key);
    }
    string value = redis.get(key);
    if (value == null) {
        value = db.query(key);
        redis.set(key, value);
    }
    return value;
}

// 降级数据来源：
// 1. 本地缓存（上次查询结果）
// 2. 静态默认值（如默认推荐列表）
// 3. 空值 + 异步重建
```

### 方案 4：Redis 高可用

```
单机 → 主从（Sentinel）→ 集群（Cluster）
    ↓           ↓               ↓
有宕机风险   自动故障转移   分片 + 冗余
```

---

## 方案对比

| 方案 | 解决的问题 | 成本 |
|------|-----------|------|
| TTL 随机化 | 大量 key 同时过期 | 极低 |
| 多级缓存 | Redis 完全不可用 | 中（本地缓存一致性维护） |
| 限流降级 | 数据库不被打垮 | 低（业务体验略降） |
| Redis 高可用 | Redis 单点故障 | 高（多节点运维） |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 雪崩 vs 击穿 vs 穿透 | 击穿=单点，雪崩=大面积，穿透=不存在 |
| TTL 随机化范围 | 基础 TTL ± 10-20% 随机 |
| 限流的目的 | 不是阻止雪崩，是防止级联故障（DB 被冲垮） |
| 本地缓存缺点 | 各节点不一致、占用 JVM 堆内存 |
| 降级是兜底策略 | 保证系统不完全不可用，而非保证数据最新 |

> **工程要点**：在生产中通常**组合使用以上方案**——TTL 随机化是必做项目，高并发系统必须配置限流。雪崩发生后恢复时，可先"预热"缓存（预加载热点数据），避免重启后的空缓存导致二次雪崩。

---

## 关联笔记

- [01d1-Cache Penetration (缓存穿透：布隆过滤器)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d1-Cache%20Penetration%20(缓存穿透：布隆过滤器).md)
- [01d2-Cache Breakdown (缓存击穿：热点key失效)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d2-Cache%20Breakdown%20(缓存击穿：热点key失效).md)
- [01a1-SDS：Simple Dynamic String (简单动态字符串)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md)
- [01a2-ziplist & listpack (压缩列表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md)
- [01a3-skiplist：Sorted Set Internals (跳表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
