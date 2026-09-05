---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: stable
---

> [!important] **核心考点**
> 过期策略（定期删除 + 惰性删除）、内存淘汰的 8 种策略、LRU 近似实现、LFU

## 过期策略

Redis 中 key 过期后的删除机制，混合使用两种策略：

### 惰性删除（Lazy Deletion）

```
访问 key → 检查是否过期 → 过期则删除并返回 nil
                          未过期则正常返回
```

- **优点**：CPU 友好，只在访问时检查
- **缺点**：过期 key 可能长期占用内存（不被访问就不删除）

### 定期删除（Active Expiration）

```c
// serverCron 定时调用的 activeExpireCycle（简化）
void activeExpireCycle(void) {
    for (int i = 0; i < dbs_per_call; i++) {
        redisDb *db = server.db[current_db];
        // 从过期字典中随机抽 20 个 key
        for (int j = 0; j < 20; j++) {
            sds key = getRandomExpiredKey(db->expires);
            if (key == NULL) break;
            if (isExpired(key)) {
                deleteKey(db, key);  // 删除
                expired++;
            }
        }
        // 如果过期 key 比例 > 25%，继续循环
        if (expired > 20/4) break;  // 需继续
    }
    // 最多执行 25ms，防止阻塞
}
```

**定期删除的特点：**
- 每秒执行 10 次（hz=10），每次最多 25ms
- 每次随机抽样 20 个过期 key
- 若过期比例 > 25% 则再抽一轮（防止过期 key 堆积）

---

## 内存淘汰（Eviction）

当 `maxmemory` 达到上限时，按策略淘汰 key 释放内存。

```ini
# redis.conf
maxmemory 4gb
maxmemory-policy allkeys-lru       # 8 种策略之一
```

### 8 种淘汰策略

| 策略 | 范围 | 淘汰依据 | 说明 |
|------|------|---------|------|
| `noeviction` | — | — | 直接返回 OOM 错误（写操作失败） |
| `allkeys-lru` | 所有 key | LRU | 淘汰最近最少使用的 key（最常用） |
| `allkeys-lfu` | 所有 key | LFU | 淘汰访问频率最低的 key |
| `allkeys-random` | 所有 key | 随机 | 随机淘汰 |
| `volatile-lru` | 设置了 TTL 的 key | LRU | 淘汰最近最少使用的过期 key |
| `volatile-lfu` | 设置了 TTL 的 key | LFU | 淘汰访问频率最低的过期 key |
| `volatile-random` | 设置了 TTL 的 key | 随机 | 随机淘汰 |
| `volatile-ttl` | 设置了 TTL 的 key | TTL 最短 | 淘汰最快过期的 key |

---

## LRU 近似实现

Redis 没有用精确 LRU（代价高），而是采样近似 LRU：

```c
// 每次淘汰时随机采样 N 个 key（默认 N=5）
// 从中淘汰空闲时间最长的那个
// maxmemory-samples 5  可调整采样数

// 每个 redisObject 都有一个 lru 字段
typedef struct redisObject {
    unsigned lru:24;      // LRU 时间戳（分钟级精度）
    // ...                // ≈ 24 位可存 194 天
} robj;

// 计算空闲时间
unsigned long long estimateObjectIdleTime(robj *o) {
    return (server.lruclock - o->lru);  // 近似
}
```

**采样数的影响：**
- samples=5：近似 LRU，性能好但淘汰不够精确
- samples=10：更接近精确 LRU，但淘汰时开销略增

---

## LFU 实现

Redis 4.0+ 支持 LFU 淘汰，用双向计数器：

```c
// lru 字段在 LFU 模式下复用为：
// ┌──────────────┬────────────────┐
// │  16-bit LDT  │  8-bit counter │
// │ 最后递减时间   │ 对数计数器      │
// └──────────────┴────────────────┘

// 对数计数器：访问次数越多，计数器增长越慢
// counter = 1 + (counter * LFU_LOG_FACTOR) / ...
// 最大值 255，所以频繁访问不会溢出

// 衰减：每隔一定时间 counter 减半
// lfu-decay-time 1   # 每 1 分钟衰减一次
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 惰性删除 + 定期删除的效果 | 99% 的过期 key 能被及时删除，且不影响性能 |
| 定期删除每次多久 | 最多 25ms |
| LRU 和 LFU 选哪个 | 请求频率均匀用 LRU；有热点用 LFU（某些 key 访问极高） |
| allkeys-lru 最常用 | 因为它对所有 key 公平，不会因未设 TTL 就留在内存 |
| 淘汰时是否立即释放内存 | 同步淘汰，一次可能淘汰多个 key 直到足够 |

> [!tip]- **工程要点**
> 生产环境通常 `maxmemory-policy allkeys-lru` + 合理设置 `maxmemory`（通常为机器内存的 50-70%，留余量给 COW 和 OS）。监控 `evicted_keys` 指标，如果持续增长说明内存不足需要扩容。

---

Redis 单线程模型与项目集成详解见 → [Redis Single Thread Model (单线程模型为何高性能)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01e-Redis%20Single%20Thread%20Model%20(单线程模型为何高性能)%20⭐.md) · [Redis Integration：C++ Client hiredis (项目集成)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01f-Redis%20Integration：C++%20Client%20hiredis%20(项目集成)%20⭐.md)
