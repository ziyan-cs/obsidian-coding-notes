---
tags:
  - database/ha
status: 🌱
---

# Master-Slave Replication — 主从复制原理

> [!important] **核心考点**：主从复制 binlog 异步复制流程、半同步复制保证、主从延迟原因与解决方案

## 主从复制概述

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

## 复制的三种线程

复制过程涉及三个线程：

```
Master:
  Binlog Dump Thread
    └─ 在事务提交时读取 binlog，发送给从库
    └─ 每个从库对应一个 dump 线程

Slave:
  I/O Thread
    └─ 连接到主库，请求 binlog
    └─ 将接收到的 binlog 写入 relay log（中继日志）

  SQL / applier thread
    └─ 从 relay log 读取事件
    └─ 在从库上重放 SQL
    └─ 可配置并行应用；具体能力和参数随 MySQL 版本变化
```

## 复制流程详解

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

## 复制模式

### 异步复制（默认）

```
主库提交事务 → 不等待从库确认 → 直接返回客户端

优点：主库性能不受影响
缺点：主库崩溃时，已提交的事务可能未同步到从库 → 数据丢失
```

### 半同步复制

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

### 组复制（Group Replication, MySQL 5.7+）

使用组通信与一致性机制协调成员，用于 InnoDB Cluster；语义、故障处理和版本能力应以当前官方文档验证。

## 主从延迟的原因

```
SHOW SLAVE STATUS\G
关键字段：
  Seconds_Behind_Source / Seconds_Behind_Master: 0   ← 仅是一个参考指标；可能为 NULL 或不代表端到端可见性

延迟原因：
  1. 从库 SQL Thread 单线程重放
     └─ 主库写入并发高 → 从库重放跟不上
     └─ 解决办法：开启并行复制

  2. 从库硬件性能低于主库
     └─ 解决办法：从库配置不低于主库

  3. 大事务
     └─ 一个 UPDATE 影响 1000 万行 → 从库执行 10 秒
     └─ 解决办法：拆分大事务

  4. 从库上的读负载
     └─ 从库同时在服务读请求
     └─ 解决办法：从库只做备份，不对外服务

  5. 锁竞争
     └─ 从库重放时与其他查询冲突
```

## 并行复制（MySQL 5.7+）

```ini
# 从库配置
slave_parallel_workers = 4          # 并行 SQL 线程数
slave_parallel_type = LOGICAL_CLOCK # 基于逻辑时钟并行
# slave_parallel_type = DATABASE    # 基于数据库并行（多库场景）
```

## 主从延迟监控与处理

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

## 30 秒回答

**复制延迟如何处理？** 先区分日志接收延迟与应用延迟，再看大事务、并行应用能力、硬件与读负载。需要写后读一致时，读主或等待指定复制进度；固定 sleep 只是在赌延迟。

---



分库分表概念见 → [Sharding & Partitioning Overview (分库分表概念)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/11-Sharding%20&%20Partitioning%20Overview%20(分库分表概念).md) · [MySQL vs Redis：Caching Strategy (缓存策略对比)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/12-MySQL%20vs%20Redis：Caching%20Strategy%20(缓存策略对比).md)
