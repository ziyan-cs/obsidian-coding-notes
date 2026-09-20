---
study_stage: backlog
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。


> [!note] 本节重点：主从复制 binlog 异步复制流程、半同步复制保证、主从延迟原因与解决方案

# 复制解决什么问题

MySQL 复制可用于**读扩展**、**故障切换候选**和部分维护任务；副本会复制误操作，因此不能替代独立备份。

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

# 复制线程与职责

复制过程涉及三个线程：

```
source
  - binlog dump thread sends binlog events to each replica

replica
  - I/O thread receives events and writes relay log
  - SQL / applier thread reads relay log and applies events
  - parallel apply support and settings vary by MySQL version
```

# 复制数据流

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

## 组复制（Group Replication）

使用组通信与一致性机制协调成员，用于 InnoDB Cluster；语义、故障处理和版本能力应以当前官方文档验证。

# 状态检查与版本差异

MySQL 新旧版本的复制命令、字段和术语不同。旧资料常见 `SHOW SLAVE STATUS`、`SHOW MASTER STATUS` 与 `Seconds_Behind_Master`；新版本逐步使用 Source/Replica 命名。实际操作必须查目标版本手册，并从接收线程、应用线程、GTID/位点和最近错误共同判断，不能用单一秒数推导端到端可见性。
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
