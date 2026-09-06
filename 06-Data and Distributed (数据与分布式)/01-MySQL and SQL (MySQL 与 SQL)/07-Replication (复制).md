---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# Replication (主从复制)

> [!note] 本节重点：核心考点：主从复制 binlog 异步复制流程、半同步复制保证、主从延迟原因与解决方案

# 主从复制概述

MySQL 主从复制用于**读写分离**、**数据备份**和**高可用切换**。

```text
Master                         Binlog         Slave                    Relay Log    SQL Thread
  │                              │              │                         │             │
  ├── Transaction commit ───────►│              │                         │             │
  │   (write to Binlog)          │              │                         │             │
  │                              │              │                         │             │
  │◄──── IO Thread: request ────────────────────│                         │             │
  │       Binlog                 |              │                         │             │
  │                              │              │                         │             │
  ├── Send Binlog events ──────────────────────►│                         │             │
  │                              │              │                         │             │
  │                              │              ├── Write to Relay Log ──►│             │
  │                              │              │                         │             │
  │                              │              │                         ├── Read ────►│
  │                              │              │                         │  Relay Log  │
  │                              │              │◄──── Replay SQL ────────│             │
  │                              │              │  (data consistency)     │             │
  │                              │              │                         │             │
  └──────────────────────────────┴──────────────┴─────────────────────────┴─────────────┘

Notes:
  Async Replication (common default)
  Semi-sync: wait for at least one slave ACK before returning to client
  Slave may experience replication lag (seconds to minutes)
```

# 复制的三种线程

复制过程涉及三个线程：

```
source
  - binlog dump thread sends binlog events to each replica

replica
  - I/O thread receives events and writes relay log
  - SQL / applier thread reads relay log and applies events
  - parallel apply support and settings vary by MySQL version
```

# 复制流程详解

```
Master                             Slave
  │                                  │
  │ 1. Transaction commit            │
  │    (Write changes to binlog)     │
  │                                  │
  │ 2. Binlog Dump Thread            │
  │    Send binlog events ──────────→│ 3. I/O Thread
  │                                  │    Receive & write to relay log
  │                                  │
  │                                  │ 4. SQL Thread
  │                                  │    Read & execute relay log events
  │                                  │
  │                                  │ 5. Apply data to slave tables
```

# 复制模式

## 异步复制（默认）

```
主库提交事务 → 不等待从库确认 → 直接返回客户端

优点：主库性能不受影响
缺点：主库崩溃时，已提交的事务可能未同步到从库 → 数据丢失
```

## 半同步复制

```
主库提交事务 → 等待至少一个副本确认收到所需复制日志 → 返回客户端

vim /usr/my.cnf:
  plugin-load = semisync_master.so;semisync_slave.so
  rpl_semi_sync_master_enabled = 1
  rpl_semi_sync_slave_enabled = 1
  rpl_semi_sync_master_timeout = 10000  # 10 秒超时

优点：缩小已提交事务未到任何副本的窗口；是否可在故障切换后保住数据还取决于确认点、持久化、拓扑和切换流程
缺点：写入延迟增加（至少 1 次网络往返）
```

## 组复制（Group Replication, MySQL 5.7+）

使用组通信与一致性机制协调成员，用于 InnoDB Cluster；语义、故障处理和版本能力应以当前官方文档验证。

# 主从延迟的原因

```
SHOW SLAVE STATUS\G
`Seconds_Behind_Source` / `Seconds_Behind_Master` is only a reference.
It can be NULL and does not prove end-to-end read visibility.

common lag causes
  - apply throughput is below source write throughput: consider parallel apply
  - replica hardware is insufficient: size replicas for the workload
  - huge transaction: split the write into bounded transactions
  - read workload or lock contention on the replica: isolate workloads
```

# 并行复制（MySQL 5.7+）

```ini
slave_parallel_workers = 4          # 并行 SQL 线程数
slave_parallel_type = LOGICAL_CLOCK # 基于逻辑时钟并行
```

# 主从延迟监控与处理

```sql
-- 查看复制状态
SHOW SLAVE STATUS\G
-- Slave_IO_Running: Yes
-- Slave_SQL_Running: Yes
-- Seconds_Behind_Master: 5

-- 查看 binlog 位置
SHOW MASTER STATUS;
-- File: mysql-bin.000042, Position: 123456

-- 从库上查看 relay log 处理情况
SHOW RELAYLOG EVENTS IN 'relay-log.000001';
```

**延迟告警设置：**
```sql
-- 延迟超过 30 秒告警
SELECT
  CASE
    WHEN Seconds_Behind_Master > 30 THEN 'REPLICATION_LAG'
    ELSE 'OK'
  END AS status
FROM information_schema.global_status
WHERE variable_name = 'Seconds_Behind_Master';
```

**应用层读写分离的延迟处理：**
```cpp
// 写入主库后立即读取 → 强制从主库读（暂时绕过从库）
User user = masterDb.query("SELECT * FROM user WHERE id = ?", id);

// 写后读需要明确一致性策略：读主、等待副本追至目标位点，或回退主库。
// 固定 sleep 不能证明副本已经追上。

// 或：主库写后记录时间戳，从库读时比较延迟
auto writeTs = std::chrono::steady_clock::now();
// ... 从库读取检查 repl lag，未追上则回退主库
```

> [!tip]- **工程要点**：异步复制、半同步与组复制是在延迟、可用性和数据丢失窗口间取舍，不存在单一“零丢失”按钮。写后读不要固定等待若干毫秒；要么读主，要么依据位点/GTID 等确认副本进度。管理命令和 `Master/Slave` 命名在新版本中已逐步演变为 `Source/Replica`，以当前版本为准。

# 30 秒回答

**复制延迟如何处理？** 先区分日志接收延迟与应用延迟，再看大事务、并行应用能力、硬件与读负载。需要写后读一致时，读主或等待指定复制进度；固定 sleep 只是在赌延迟。

---

分库分表概念见 → [Sharding & Partitioning Overview (分库分表概念)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/11-Sharding%20&%20Partitioning%20Overview%20(分库分表概念).md) · [MySQL vs Redis：Caching Strategy (缓存策略对比)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/12-MySQL%20vs%20Redis：Caching%20Strategy%20(缓存策略对比).md)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **07-Replication (复制)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
