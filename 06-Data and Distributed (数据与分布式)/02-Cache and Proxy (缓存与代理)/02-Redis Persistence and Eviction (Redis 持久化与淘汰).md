---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# RDB Persistence (RDB 持久化)

> [!note] 本节重点：核心考点：> RDB 触发方式、BGSAVE 写时复制（COW）、RDB 文件结构、优缺点

# RDB 快照

RDB 是 Redis 的全量快照持久化方式，将内存数据全部写入磁盘文件（`dump.rdb`）。

## 触发方式

```ini
save 900 1           # 900 秒内至少 1 个 key 变化 → BGSAVE
save 300 10          # 300 秒内至少 10 个 key 变化 → BGSAVE
save 60  10000       # 60 秒内至少 10000 个 key 变化 → BGSAVE

```

## BGSAVE 写时复制（COW）

```text
Client              Redis Main Process        Forked Child           Disk
  │                        │                      │                  │
  ├── BGSAVE command ─────→│                      │                  │
  │                        ├── fork()             │                  │
  │                        ├─────────────────────→│                  │
  │                        │ Child process created│                  │
  │                        │                      │                  │
  │◄──── Parent continues  │                      │                  │
  │     handling requests  │                      │                  │
  │                        │                      ├── Write in-memory│
  │                        │                      │   data to temp   │
  │                        │                      │   RDB file ─────→│
  │                        │                      │                  │
  │                        │    Uses Copy-on-Write:                  │
  │                        │    fork shares memory pages             │
  │                        │    parent process copies on write       │
  │                        │                      │                  │
  │                        │                      ├── Write complete │
  │                        │                      │   rename to      │
  │                        │                      │   dump.rdb ─────→│
  │                        │◄──── Notify parent ──┤                  │
  │◄──── BGSAVE OK ────────┤                      │                  │
```

**COW 代价：** fork 后如果有大量写入，每个写操作的页（默认 4KB）都会触发复制，增加内存和延迟。`info persistence` 可监控 `rdb_changes_since_last_save`。

## RDB 文件结构

```
┌──────────┬─────────────┬──────────────┬──────────────┬────────────┐
│ "REDIS"  │ RDB Version │ Aux Fields   │ Database Data│ EOF Marker │
│ 0006     │ 5 bytes     │    ...       │  key-value   │ 8 bytes    │
└──────────┴─────────────┴──────────────┴──────────────┴────────────┘
                      ↓
           ┌──────────────────────────────┐
           │ SELECTDB 0                   │
           │  key-value pairs             │
           │  Type Encoding + Key + Value │
           │  EXPIRETIME, etc.            │
           └──────────────────────────────┘
```

---

# RDB 优缺点

| 优点 | 缺点 |
|------|------|
| 文件紧凑，适合备份和灾难恢复 | 丢数据风险大（两次快照间写入全丢） |
| 恢复速度远快于 AOF（直接加载） | BGSAVE fork 可能阻塞主进程 |
| 子进程写，主进程性能影响小（除 fork） | 大数据量时 fork 耗时可能达秒级 |
| 单个文件，数据迁移方便 | 频繁执行影响磁盘 I/O |

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| BGSAVE 为什么不阻塞 | fork 子进程读共享内存写入文件 |
| COW 对 Redis 的影响 | 写入量越大，子进程持有更多物理页，内存消耗可能翻倍 |
| SAVE 和 BGSAVE 的区别 | SAVE 主进程阻塞写；BGSAVE 子进程写 |
| RDB 适合什么场景 | 定时备份、灾备恢复、数据迁移 |
| RDB 丢多少数据 | 最多丢最后一次 BGSAVE 到宕机间的所有写操作 |

> [!tip]- **工程要点**
> RDB + AOF 混合使用是最佳实践（Redis 4.0+ 支持混合持久化 = AOF rewrite 时生成 RDB 段 + AOF 增量段）。`latency-monitor-threshold` 可用于监控 fork 阻塞。

# 30 秒回答 / 自测

- **30 秒回答**：RDB 是全量快照；BGSAVE 用 fork 子进程 + COW 异步写，主进程不阻塞（但 fork 本身会短暂阻塞，写量大时内存可能翻倍）；最多丢最后一次快照后的数据，适合备份/灾备，不适合高持久化要求，需搭配 AOF。
- **常见误区**：以为 BGSAVE 完全零开销（fork 大内存实例可达秒级）；只开 RDB 却要求"不丢数据"（应配 AOF/混合持久化）。
- **自测**：1) COW 机制下，BGSAVE 期间大量写操作会发生什么？ 2) 为什么 fork 会被形容为"可能阻塞主进程"？

---

AOF 日志与 RDB 快照对比详解见 → 01b2-AOF：Write-Ahead Log & Rewrite (日志重写)

---

# AOF Persistence (AOF 持久化)

> [!note] 本节重点：核心考点：> AOF 写回策略（always/everysec/no）、AOF 重写机制、AOF 文件格式、混合持久化

# AOF 日志

AOF（Append Only File）记录每个写命令，重启时重放恢复数据。

## 写回策略

```ini
appendfsync always     # 每条命令都 fsync 到磁盘（最安全，最慢）
appendfsync everysec   # 每秒 fsync 一次（默认，推荐）
appendfsync no         # 交给 OS 决定刷盘（最快，丢最多）
```

| 策略 | 数据丢失 | 性能（TPS） |
|------|---------|------------|
| always | 最多丢 1 条命令 | ≈ 数百（每次写都 fsync） |
| everysec | 最多丢 1 秒数据 | ≈ 数万 |
| no | 最多丢若干秒数据 | ≈ 十数万 |

## AOF 文件格式

```
*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n
*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n

解释：
*3          → 3 个参数
$3          → 参数长度 3
SET         → 命令
$3 → key    → 键
$5 → value  → 值
```

---

# AOF 重写（Rewrite）

AOF 文件随时间增长无限膨胀，需要定期压缩——重写不是读取旧 AOF，而是直接从内存数据生成。

## 触发时机

```ini
auto-aof-rewrite-percentage 100    # 文件比上次重写增大 100%
auto-aof-rewrite-min-size 64mb     # 文件至少 64MB
```

## 重写过程

```text
┌──────────────────────────────────────────────────────────────────────┐
│  Normal Write Path                                                   │
│                                                                      │
│  ┌──────────┐     ┌──────────────────┐     ┌──────────────────────┐  │
│  │SET foo   │────→│ Append to AOF    │────→│ fsync to disk        │  │
│  │bar       │     │ buffer           │     │ (every sec / every   │  │
│  └──────────┘     └──────────────────┘     │  op / OS-dependent)  │  │
│                                            └──────────────────────┘  │
├──────────────────────────────────────────────────────────────────────┤
│  AOF Rewrite Path                                                    │
│                                                                      │
│  ┌────────────────┐     ┌────────────────────────────┐               │
│  │ Current in-    │────→│ Scan memory and generate   │               │
│  │ memory state   │    │ minimal command set         │               │
│  └────────────────┘     └────────────────────────────┘               │
│                                                                      │
│  ┌────────────────────┐     ┌──────────────────────────────┐         │
│  │ Old AOF file may   │────→│ Fork child process to        │         │
│  │ be very large      │     │ perform rewrite              │         │
│  └────────────────────┘     └──────────────┬───────────────┘         │
│                                            │                         │
│                                            ▼                         │
│                          ┌──────────────────────────────┐            │
│                          │ Parent increment buffer →    │            │
│                          │ merge with rewritten content │            │
│                          └──────────────┬───────────────┘            │
│                                         │                            │
│                                         ▼                            │
│                          ┌──────────────────────────────┐            │
│                          │ New AOF file replaces old    │            │
│                          └──────────────────────────────┘            │
│                                                                      │
│  Example: A key incremented 1000 times                               │
│  → rewrite as SET key 1000 (only the final state preserved)          │
└──────────────────────────────────────────────────────────────────────┘
```

**重写优化：** 将多次命令合并为最少命令。例如 `RPUSH list A` `RPUSH list B` 合并为 `RPUSH list A B`。

---

# 混合持久化（Redis 4.0+）

```ini
aof-use-rdb-preamble yes
```

AOF 重写时，将当前内存数据以 RDB 格式写在 AOF 文件开头，后接增量 AOF 命令：

```
┌──────────────────┬──────────────────────────┐
│  RDB Snapshot    │  AOF Incremental Cmds    │
│  (Full Data)     │  (Post-rewrite Writes)   │
└──────────────────┴──────────────────────────┘
```

**优势：** 加载时先加载 RDB（快）再重放 AOF（增量），既快又只丢少量数据。

---

# RDB vs AOF 对比

| 特性 | RDB | AOF | 混合 |
|------|-----|-----|------|
| 文件大小 | 小 | 大 | 中等 |
| 恢复速度 | 快 | 慢（重放所有命令） | 快 |
| 数据安全性 | 丢多 | 丢少（everysec 丢 1 秒） | 丢少 |
| 对性能影响 | fork 阻塞 + COW | 写回策略影响写延迟 | 折中 |
| 可读性 | 二进制 | 文本协议（可读） | 二进制开头 |

---

# 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| AOF everysec 最推荐 | 丢 1 秒数据 vs always 的性能代价权衡 |
| AOF 重写为什么用子进程 | 避免阻塞主进程 + fork 后的 COW 保证数据一致性 |
| AOF 文件损坏怎么办 | `redis-check-aof --fix` 修复 |
| 混合持久化加载流程 | 读文件头判断是否为 RDB → 加载 RDB → 重放剩余 AOF |
| AOF rewrite 中的增量处理 | 重写缓冲区记录重写期间的命令，完成后追加 |

> [!tip]- **工程要点**
> 生产环境推荐 `appendfsync everysec` + `aof-use-rdb-preamble yes`。RDB 仍建议开启作为备份补充（灾难恢复场景）。`info persistence` 监控 `aof_pending_bio_fsync` 判断 AOF 是否堆积。

---

RDB 快照与 AOF 持久化对比详解见 → 01b1-RDB：Snapshot & BGSAVE (快照原理)

---

# Expiration and Eviction (过期与淘汰)

> [!note] 本节重点：核心考点：> 过期策略（定期删除 + 惰性删除）、内存淘汰的 8 种策略、LRU 近似实现、LFU

> [!warning] Redis 配置与实现细节随版本变化
> `hz`、采样数、时间预算、可用淘汰策略以及对象内部位布局都应以当前 Redis 官方文档和实际配置为准。这里保留机制心智模型，不把示意常量当作稳定面试答案。

# 过期策略

Redis 中 key 过期后的删除机制，混合使用两种策略：

## 惰性删除（Lazy Deletion）

```
访问 key → 检查是否过期 → 过期则删除并返回 nil
                          未过期则正常返回
```

- **优点**：CPU 友好，只在访问时检查
- **缺点**：过期 key 可能长期占用内存（不被访问就不删除）

## 定期删除（Active Expiration）

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
- 周期性抽样并删除过期 key，避免惰性删除造成长期内存占用
- 执行频率、抽样与时间预算由 Redis 版本和 `hz` / active-expire 相关配置共同影响
- 过期比例较高时会继续努力清理，但不能把固定阈值背成跨版本规则

---

# 内存淘汰（Eviction）

当 `maxmemory` 达到上限时，按策略淘汰 key 释放内存。

```ini
maxmemory 4gb
maxmemory-policy allkeys-lru       # 8 种策略之一
```

## 8 种淘汰策略

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

# LRU 近似实现

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

# LFU 实现

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

# 经典题型速查 · 延伸要点 3
| 题型 | 要点 |
|------|------|
| 惰性删除 + 定期删除的效果 | 在内存及时回收与 CPU 开销间折中，效果受负载和配置影响 |
| 定期删除每次多久 | 由版本与运行时配置决定，需查当前文档/实测 |
| LRU 和 LFU 选哪个 | 请求频率均匀用 LRU；有热点用 LFU（某些 key 访问极高） |
| allkeys-lru 最常用 | 因为它对所有 key 公平，不会因未设 TTL 就留在内存 |
| 淘汰时是否立即释放内存 | 同步淘汰，一次可能淘汰多个 key 直到足够 |

> [!tip]- **工程要点**
> 生产环境通常 `maxmemory-policy allkeys-lru` + 合理设置 `maxmemory`（通常为机器内存的 50-70%，留余量给 COW 和 OS）。监控 `evicted_keys` 指标，如果持续增长说明内存不足需要扩容。

# 30 秒回答

过期是 key 的生命周期语义，淘汰是在内存达到 `maxmemory` 后的资源策略；二者不能混为一谈。Redis 通过访问时检查与后台主动清理处理过期 key，再按 `maxmemory-policy` 在候选 key 中淘汰。生产选型要看数据是否允许被驱逐、TTL 覆盖率、写入峰值和持久化/COW 内存余量。

# 自测

1. 一个 key 有 TTL 却迟迟未访问，为什么不应假设它会立刻释放内存？
2. `volatile-*` 与 `allkeys-*` 的候选集合有何不同？
3. `evicted_keys` 持续增长时，为什么不能只盲目调大采样数？

---

Redis 单线程模型与项目集成详解见 → Redis Single Thread Model (单线程模型为何高性能) · Redis Integration：C++ Client hiredis (项目集成)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Redis Persistence and Eviction (Redis 持久化与淘汰)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Cache and Proxy Map (缓存与代理导航)](/06-Data%20and%20Distributed%20(数据与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/00-Cache%20and%20Proxy%20Map%20(缓存与代理导航).md)
- 下一步：[03-Cache Consistency Problems (缓存一致性问题)](/06-Data%20and%20Distributed%20(数据与分布式)/02-Cache%20and%20Proxy%20(缓存与代理)/03-Cache%20Consistency%20Problems%20(缓存一致性问题).md)
