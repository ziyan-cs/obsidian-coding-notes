---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: stable
---

> [!important] **核心考点**
> AOF 写回策略（always/everysec/no）、AOF 重写机制、AOF 文件格式、混合持久化

## AOF 日志

AOF（Append Only File）记录每个写命令，重启时重放恢复数据。

### 写回策略

```ini
# redis.conf
appendfsync always     # 每条命令都 fsync 到磁盘（最安全，最慢）
appendfsync everysec   # 每秒 fsync 一次（默认，推荐）
appendfsync no         # 交给 OS 决定刷盘（最快，丢最多）
```

| 策略 | 数据丢失 | 性能（TPS） |
|------|---------|------------|
| always | 最多丢 1 条命令 | ≈ 数百（每次写都 fsync） |
| everysec | 最多丢 1 秒数据 | ≈ 数万 |
| no | 最多丢若干秒数据 | ≈ 十数万 |

### AOF 文件格式

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

## AOF 重写（Rewrite）

AOF 文件随时间增长无限膨胀，需要定期压缩——重写不是读取旧 AOF，而是直接从内存数据生成。

### 触发时机

```ini
auto-aof-rewrite-percentage 100    # 文件比上次重写增大 100%
auto-aof-rewrite-min-size 64mb     # 文件至少 64MB
# 手动：BGREWRITEAOF
```

### 重写过程

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

## 混合持久化（Redis 4.0+）

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

## RDB vs AOF 对比

| 特性 | RDB | AOF | 混合 |
|------|-----|-----|------|
| 文件大小 | 小 | 大 | 中等 |
| 恢复速度 | 快 | 慢（重放所有命令） | 快 |
| 数据安全性 | 丢多 | 丢少（everysec 丢 1 秒） | 丢少 |
| 对性能影响 | fork 阻塞 + COW | 写回策略影响写延迟 | 折中 |
| 可读性 | 二进制 | 文本协议（可读） | 二进制开头 |

---

## 经典题型速查

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

RDB 快照与 AOF 持久化对比详解见 → [01b1-RDB：Snapshot & BGSAVE (快照原理)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01b-Persistence：RDB%20&%20AOF%20(持久化机制)%20⭐/01b1-RDB：Snapshot%20&%20BGSAVE%20(快照原理).md)
