---
tags:
  - database/ha
status: 🌱
---

> [!important] **核心考点**：MySQL 与 Redis 缓存策略对比、缓存穿透/击穿/雪崩、读写缓存一致性方案

## MySQL vs Redis 定位

| 特性 | MySQL | Redis |
|------|-------|-------|
| 数据类型 | 结构化表（行列） | Key-Value + 多种数据结构 |
| 数据存储 | 磁盘（持久化） | 内存（可持久化） |
| 读写速度 | 千级 QPS | 十万级 QPS |
| 查询能力 | 复杂 SQL 查询 | 简单 KV + 特定操作 |
| 数据容量 | TB 级 | 受限于内存大小 |
| 一致性 | ACID 强一致 | 最终一致性 |

**缓存定位：** Redis 是 MySQL 的前置加速层，不是替代品。

## 缓存架构模式

### Cache-Aside（旁路缓存）

应用程序同时维护缓存和数据库——最常用的模式。

```
读流程：
  请求 → 查缓存 → 命中 → 返回
               ↓ 未命中
           查数据库 → 写入缓存 → 返回

写流程：
  请求 → 写数据库 → 删除缓存（或更新缓存）
```

```cpp
// Cache-Aside 读（C++ + hiredis）
string getUser(int userId) {
    redisReply* reply = (redisReply*)redisCommand(c,
        "GET user:%d", userId);
    if (reply && reply->type == REDIS_REPLY_STRING) {
        string val = reply->str;
        freeReplyObject(reply);
        return val;
    }
    freeReplyObject(reply);

    // 缓存未命中 → 查数据库 → 写缓存
    User user = db.query("SELECT * FROM user WHERE id = ?", userId);
    redisCommand(c, "SETEX user:%d 3600 %s", userId, user.toJson().c_str());
    return user.toJson();
}

// Cache-Aside 写
void updateUser(int userId, const string& data) {
    db.execute("UPDATE user SET ... WHERE id = ?", userId, data);
    redisCommand(c, "DEL user:%d", userId);  // 删除缓存，而非更新
}
```

**为什么写时删除缓存而不是更新缓存？**
```
更新缓存的问题：
  写 1 → 更新缓存为 A
  写 2 → 更新缓存为 B
  (写 2 先完成，写 1 后完成 → 缓存为 A，数据库为 B → 不一致！)

删除缓存：
  读取时如果缓存不存在 → 重建缓存 → 保证最终一致性
```

### 其他模式

**Read-Through（穿透读）：**
```
缓存层位于数据库前面，应用只与缓存交互。
缓存未命中时，缓存组件自动加载数据库数据。
适合：对应用透明的缓存方案（如 Redis Enterprise 的 Auto Tiering）
```

**Write-Through（穿透写）：**
```
先写缓存，缓存同步写数据库（同步）。
优点：数据强一致
缺点：写入延迟增加（两次写入）
```

**Write-Behind（异步写）：**
```
先写缓存，异步批量写数据库。
优点：写入性能极高
缺点：可能丢数据（缓存崩溃）
适合：日志/计数场景
```

## 缓存三大问题

### 缓存穿透

```
问题：查询一个**不存在**的数据
  → 缓存未命中 → 查询数据库（也不存在）→ 不写缓存
  → 下次同样的请求再次穿透到数据库
  → 恶意攻击：大量请求不存在的 key → 数据库被打垮

解决方案：

1. 缓存空值（最简单有效）
   user = redis.get(key)
   if not user:
       user = db.query(key)
       if not user:
           redis.setex(key, 60, EMPTY_PLACEHOLDER)  # 短时间缓存空值
       else:
           redis.setex(key, 3600, user)

2. 布隆过滤器（Bloom Filter）
   - 将所有存在的 key 放入布隆过滤器
   - 请求先过布隆过滤器：不存在 → 直接拒绝
   - 优点：节省内存，O(1) 判断
   - 缺点：有误判率（可能认为存在但实际不存在）

3. 参数校验
   - 明显不合法的参数直接拒绝（如 user_id = -1）
```

### 缓存击穿

```
问题：一个**热点 key** 在缓存过期的一瞬间
  → 大量并发请求同时发现缓存过期
  → 全部去查询数据库 → 数据库瞬间高负载

解决方案：

1. 互斥锁（Mutex Key）
   - 缓存过期时，只有一个线程去查数据库
   - 其他线程等待
   
   public String get(key) {
       String value = redis.get(key);
       if (value == null) {
           if (redis.setnx("lock:" + key, 1, 10)) {
               // 只有拿到锁的线程查数据库
               value = db.query(key);
               redis.set(key, value, 3600);
               redis.delete("lock:" + key);
           } else {
               Thread.sleep(50);  // 等待
               return get(key);   // 重试
           }
       }
       return value;
   }

2. 热点数据永不过期
   - 不设置过期时间
   - 后台线程异步更新
   - 适合不要求实时一致性的数据
```

### 缓存雪崩

```
问题：大量缓存同时过期 或 Redis 崩溃
  → 所有请求直达数据库 → 数据库被打垮

解决方案：

1. 过期时间加随机值（避免同时过期）
   redis.setex(key, base_ttl + random(0, 300), value)
   # base_ttl=3600, random=0-300s → 过期时间分布在 3600-3900s

2. 多级缓存
   - L1: 本地缓存（Caffeine/Guava Cache）
   - L2: Redis 集群
   - L3: MySQL
   本地缓存分担大部分请求，减少 Redis 和 MySQL 压力

3. Redis 高可用
   - 主从 + 哨兵
   - Redis Cluster
   - 避免单点故障

4. 限流 + 降级
   - 请求量过大时直接返回"服务繁忙"
   - 保护数据库不被打垮
```

## 读写缓存一致性

最终一致性方案（工程中最常用）：

```
更新数据库 → 删除缓存 → 保证最终一致

C 端对一致性要求高的场景：
  如果必须"写后立即读"一致：
    → 缓存设为 0 过期（但内存占用大）
    → 或使用"分布式锁"保证读写串行化

实际工程经验：
  大多数业务接受"最终一致性"
  "缓存不一致"通常只持续几毫秒到几秒
  加锁保证强一致的代价大于不一致带来的问题
```

> [!tip]- **工程要点**：缓存设计的三条经验——1) **一定要有过期时间**；没有过期时间的缓存是"内存泄漏"；2) **穿透、击穿、雪崩是三个完全不同的问题**，解决思路不同，面试时不要混淆；3) 99% 的场景用 **Cache-Aside + 删除缓存** 就够了，不需要更复杂的方案。写入时先写数据库再删缓存（而不是先删缓存再写数据库），降低并发读写不一致的概率。

---



主从复制原理见 → [Master-Slave Replication (主从复制原理)](/06-Database%20(MySQL)/04%20·%20高可用与架构/10-Master-Slave%20Replication%20(主从复制原理).md) · [Sharding & Partitioning Overview (分库分表概念)](/06-Database%20(MySQL)/04%20·%20高可用与架构/11-Sharding%20&%20Partitioning%20Overview%20(分库分表概念).md)
