---
tags:
  - redis/core
status: 🌱
---

> [!important] **核心考点**
> 缓存击穿原因、互斥锁方案、逻辑过期方案、热点 key 处理

## 缓存击穿

一个**热点 key** 在缓存中恰好过期，大量并发请求同时穿透到数据库。

```
           Redis（key 刚过期）
               ↑ 大量请求
         ┌─────┴───────┐
         │  互斥锁？    │
         └─────────────┘
               ↓
            MySQL（单 key 的大量并发，压力激增）
```

---

## 解决方案

### 方案 1：互斥锁（Mutex Key）

```cpp
// 伪代码：只让一个线程去 DB 回源
string get(string key) {
    string value = redis.get(key);
    if (value != null) return value;

    // 尝试获取互斥锁
    if (redis.setnx("lock:" + key, "1", 60)) {
        // 只有第一个线程能拿到锁
        value = db.query(key);
        redis.set(key, value, 3600);
        redis.del("lock:" + key);
        return value;
    } else {
        // 其他线程等待重试
        sleep(50);
        return get(key);  // 递归重试
    }
}
```

**缺点：** 阻塞其他请求，引入锁竞争，可能影响吞吐量。

### 方案 2：逻辑过期（提前主动刷新）

```cpp
// 缓存中存 value + 逻辑过期时间，而不是依赖 Redis TTL
struct CacheEntry {
    string data;
    long logicExpireTime;  // 逻辑过期时间戳
};

string get(string key) {
    CacheEntry entry = redis.get(key);

    // 逻辑未过期 → 直接返回
    if (entry.logicExpireTime > now()) {
        return entry.data;
    }

    // 逻辑已过期 → 尝试获取刷新锁
    if (redis.setnx("refresh:" + key, "1", 60)) {
        string newValue = db.query(key);
        redis.set(key, CacheEntry(newValue, now() + 3600));
        redis.del("refresh:" + key);
        return newValue;
    } else {
        // 没拿到锁的线程直接返回旧数据
        return entry.data;  // 返回过期但可用的数据
    }
}
```

**优点：** 无阻塞，高并发下仍能返回数据（虽可能略旧）。

### 方案 3：热点 key 永不过期

对于真正的热点 key（如热搜、首页推荐），设置为**不主动过期**，后台定时刷新：

```cpp
// 后台线程定期刷新
while (true) {
    for (auto& hotKey : hotKeys) {
        string value = db.query(hotKey);
        redis.set(hotKey, value);  // 不设 TTL
        sleep(60);  // 每分钟刷新一次
    }
}
```

---

## 方案对比

| 方案 | 复杂度 | 数据一致性 | 并发能力 | 适用场景 |
|------|--------|-----------|---------|---------|
| 互斥锁 | 低 | 强 | 低（串行回源） | 并发不高的场景 |
| 逻辑过期 | 中 | 弱（可能读旧） | 高（无阻塞） | 高并发、可接受短暂不一致 |
| 永不过期 + 后台刷新 | 低 | 最终一致 | 极高 | 真正的热点 key |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 穿透 vs 击穿 vs 雪崩 | 穿透=查不存在，击穿=热点过期，雪崩=大量同时过期 |
| 互斥锁的注意点 | 防止死锁（设置锁超时）、防止锁误删（用唯一 ID） |
| 逻辑过期的前提 | 业务能接受短暂数据不一致 |
| 什么是热点 key | 访问量极高的 key（如明星热搜、秒杀商品） |

> [!tip]- **工程要点**
> 热点 key 的识别可以写入时标记（如 `hincrby hotkey_count` 计数），或在代理层（如 Codis/Twemproxy）统计访问频率。实际生产中逻辑过期 + 后台刷新是最常用的方案。

---

缓存三大问题系列详解见 → [01d1-Cache Penetration (缓存穿透：布隆过滤器)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d1-Cache%20Penetration%20(缓存穿透：布隆过滤器).md) · [01d3-Cache Avalanche (缓存雪崩：大量key同时失效)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d3-Cache%20Avalanche%20(缓存雪崩：大量key同时失效).md)
