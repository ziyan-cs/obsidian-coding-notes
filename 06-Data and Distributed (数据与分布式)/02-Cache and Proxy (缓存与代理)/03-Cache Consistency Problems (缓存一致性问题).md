---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# 30 秒回答

缓存是派生数据，不是事实来源。先定义数据库与缓存的写入顺序、失效窗口和失败补偿，再分别处理穿透、击穿与雪崩；不存在一个只靠加 TTL 就能解决的缓存策略。

# Cache-Aside 基线

```text
读：cache miss → database → 填充缓存
写：更新 database 成功 → 删除或更新缓存 → 失败进入补偿/重试观测
```

# 三类问题的边界

| 问题 | 本质 | 常用防护 |
| --- | --- | --- |
| 穿透 | 不存在的 key 反复回源 | 参数校验、布隆过滤器、短 TTL 空值 |
| 击穿 | 热点 key 同时失效 | singleflight、互斥重建、逻辑过期 |
| 雪崩 | 大量 key 或依赖同时不可用 | TTL 打散、限流、降级、容量预案 |

# 自测

1. 数据库写成功但缓存删除失败时，旧值窗口如何产生？
2. 布隆过滤器为什么允许 false positive 却不能 false negative？
3. 热点重建锁为什么也必须有 timeout 与降级方案？

# Cache Penetration (缓存穿透)

> [!note] 本节重点：核心考点：> 缓存穿透原因、布隆过滤器原理、误判率与哈希函数数、解决方案对比

# 缓存穿透

查询一个**根本不存在**的数据，请求绕过缓存直达数据库。

```
用户请求 → Redis（未命中）→ MySQL（没查到）→ 返回 null
          ↓
   大量请求重复相同路径 → MySQL 压力激增
```

## 解决方案

| 方案 | 原理 | 缺点 |
|------|------|------|
| **缓存空值** | 将 null 也缓存，短 TTL（30-60s） | 大量空 key 占内存 |
| **布隆过滤器** | 请求前先判断 key 是否存在 | 有误判率（不存在判为存在） |
| **参数校验** | 基本参数合法性检查 | 无法防止合法不存在的 ID |

---

# 布隆过滤器（Bloom Filter）

概率性数据结构，判断"一定不存在"和"可能存在"：

```
初始化：bitmap = [0, 0, 0, 0, 0, 0, 0, 0]  (m=8)
添加 "key1" → hash1(key1)=0, hash2(key1)=3, hash3(key1)=6
              bitmap[0]=1, bitmap[3]=1, bitmap[6]=1

bitmap: [1, 0, 0, 1, 0, 0, 1, 0]

查询 "key2" → hash1(key2)=0, hash2(key2)=2, hash3(key2)=7
              bitmap[0]=1  ✓, bitmap[2]=0 ✗ → "一定不存在"

查询 "key3" → hash1(key3)=0, hash2(key3)=3, hash3(key3)=6
              bitmap[0]=1 ✓, bitmap[3]=1 ✓, bitmap[6]=1 ✓ → "可能存在"
              ↳ 实际 key3 可能不存在（hash 碰撞导致的误判）
```

**特点：**
- **一定不存在（不允许 false negative）**：返回"不存在"时 100% 正确
- **可能存在（允许 false positive）**：返回"存在"时有误判率（假阳性）
- **无法删除**（标准 BF），可改用 Counting Bloom Filter

## 参数选择

```cpp
// 期望插入 n 个元素，期望误判率 p
// bitmap 大小 m = -n * ln(p) / (ln2)²
// 哈希函数数 k = (m/n) * ln2 ≈ 0.7 * (m/n)

// 示例：n=10⁶, p=1%
// m ≈ -10⁶ * ln(0.01) / (0.48) ≈ 10⁶ * 4.6 / 0.48 ≈ 9.58 × 10⁶ bits ≈ 1.14MB
// k = (9.58×10⁶ / 10⁶) * 0.693 ≈ 6.6 ≈ 7 个哈希函数
```

## Redis 中使用布隆过滤器

```bash
BF.ADD bloomfilter key1      # 添加元素
BF.EXISTS bloomfilter key1   # 检查是否存在 → (integer) 1
BF.EXISTS bloomfilter key99  # → (integer) 0
BF.RESERVE bloomfilter 0.01 10000  # 创建（误判率1%，容量10000）
```

---

# 其他方案

```cpp
// 1. 缓存空值
if (result == null) {
    redis.set(key, "NULL", 30);  // TTL=30s
}

// 2. 参数校验（简单防攻击）
if (id <= 0 || id > MAX_VALID_ID) {
    return "invalid param";
}
```

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| 缓存穿透的本质 | 查不存在的数据，每次都穿到 DB |
| 布隆过滤器不能做什么 | 无法删除、有误判率、需要预知容量 |
| 误判率怎么控制 | 增大 bitmap 或增加哈希函数 |
| 缓存空值的风险 | 空 key 过多可设短 TTL + 定期清理 |

> [!tip]- **工程要点**
> 布隆过滤器适合"大集合 + 允许少量误判"的场景。误判的代价是多一次 DB 查询，在大部分业务中可接受。Redis module 方式省去在应用层自行实现。

---

缓存三大问题系列详解见 → [01d2-Cache Breakdown (缓存击穿：热点key失效)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d2-Cache%20Breakdown%20(缓存击穿：热点key失效).md) · [01d3-Cache Avalanche (缓存雪崩：大量key同时失效)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d3-Cache%20Avalanche%20(缓存雪崩：大量key同时失效).md)

---

# Cache Breakdown (缓存击穿)

> [!note] 本节重点：核心考点：> 缓存击穿原因、互斥锁方案、逻辑过期方案、热点 key 处理

# 缓存击穿

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

# 解决方案

## 方案 1：互斥锁（Mutex Key）

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

## 方案 2：逻辑过期（提前主动刷新）

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

## 方案 3：热点 key 永不过期

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

# 方案对比

| 方案 | 复杂度 | 数据一致性 | 并发能力 | 适用场景 |
|------|--------|-----------|---------|---------|
| 互斥锁 | 低 | 强 | 低（串行回源） | 并发不高的场景 |
| 逻辑过期 | 中 | 弱（可能读旧） | 高（无阻塞） | 高并发、可接受短暂不一致 |
| 永不过期 + 后台刷新 | 低 | 最终一致 | 极高 | 真正的热点 key |

---

# 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| 穿透 vs 击穿 vs 雪崩 | 穿透=查不存在，击穿=热点过期，雪崩=大量同时过期 |
| 互斥锁的注意点 | 防止死锁（设置锁超时）、防止锁误删（用唯一 ID） |
| 逻辑过期的前提 | 业务能接受短暂数据不一致 |
| 什么是热点 key | 访问量极高的 key（如明星热搜、秒杀商品） |

> [!tip]- **工程要点**
> 热点 key 的识别可以写入时标记（如 `hincrby hotkey_count` 计数），或在代理层（如 Codis/Twemproxy）统计访问频率。实际生产中逻辑过期 + 后台刷新是最常用的方案。

---

缓存三大问题系列详解见 → [01d1-Cache Penetration (缓存穿透：布隆过滤器)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d1-Cache%20Penetration%20(缓存穿透：布隆过滤器).md) · [01d3-Cache Avalanche (缓存雪崩：大量key同时失效)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d3-Cache%20Avalanche%20(缓存雪崩：大量key同时失效).md)

---

# Cache Avalanche (缓存雪崩)

> [!note] 本节重点：核心考点：> 缓存雪崩的原因、过期时间加随机化、多级缓存、降级与限流

# 缓存雪崩

大量 key 在同一时间过期失效，或 Redis 节点宕机，导致大量请求直达数据库。

```
正常：  Redis（命中）← 请求 → DB 低负载
雪崩：  Redis（大量 key 过期/宕机）→ 请求穿透 → DB 被打垮
                                   → 级联故障
```

## 对比缓存击穿

| | 缓存击穿 | 缓存雪崩 |
|--|---------|---------|
| 范围 | 单个热点 key | 大量 key / 整个 Redis 不可用 |
| 原因 | 热点 key 过期 | 多 key 同时过期 / Redis 宕机 |
| 影响 | 数据库压力上升 | 数据库直接被冲垮 |

---

# 解决方案 · 延伸要点 2
## 方案 1：过期时间加随机化

```cpp
// 避免大量 key 在同一秒过期
int ttl = 3600 + rand() % 600;  // 基础 1h + 随机 0-10min
redis.setex(key, ttl, value);

// 或者用固定时间 + 随机偏移
redis.setex(key, 3600, value);
redis.expire(key, ttl);  // 重设随机 TTL
```

## 方案 2：多级缓存

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

## 方案 3：限流与降级

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

## 方案 4：Redis 高可用

```
单机 → 主从（Sentinel）→ 集群（Cluster）
    ↓           ↓               ↓
有宕机风险   自动故障转移   分片 + 冗余
```

---

# 方案对比 · 延伸要点 2
| 方案 | 解决的问题 | 成本 |
|------|-----------|------|
| TTL 随机化 | 大量 key 同时过期 | 极低 |
| 多级缓存 | Redis 完全不可用 | 中（本地缓存一致性维护） |
| 限流降级 | 数据库不被打垮 | 低（业务体验略降） |
| Redis 高可用 | Redis 单点故障 | 高（多节点运维） |

---

# 经典题型速查 · 延伸要点 3
| 题型 | 要点 |
|------|------|
| 雪崩 vs 击穿 vs 穿透 | 击穿=单点，雪崩=大面积，穿透=不存在 |
| TTL 随机化范围 | 基础 TTL ± 10-20% 随机 |
| 限流的目的 | 不是阻止雪崩，是防止级联故障（DB 被冲垮） |
| 本地缓存缺点 | 各节点不一致、占用 JVM 堆内存 |
| 降级是兜底策略 | 保证系统不完全不可用，而非保证数据最新 |

> [!tip]- **工程要点**
> 在生产中通常**组合使用以上方案**——TTL 随机化是必做项目，高并发系统必须配置限流。雪崩发生后恢复时，可先"预热"缓存（预加载热点数据），避免重启后的空缓存导致二次雪崩。

---

缓存三大问题系列详解见 → [01d1-Cache Penetration (缓存穿透：布隆过滤器)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d1-Cache%20Penetration%20(缓存穿透：布隆过滤器).md) · [01d2-Cache Breakdown (缓存击穿：热点key失效)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d2-Cache%20Breakdown%20(缓存击穿：热点key失效).md)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Cache Consistency Problems (缓存一致性问题)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
