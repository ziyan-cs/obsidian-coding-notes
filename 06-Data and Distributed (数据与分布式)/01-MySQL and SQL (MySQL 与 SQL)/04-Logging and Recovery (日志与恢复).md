---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 04-Logging and Recovery (日志与恢复)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 15-Write Ahead Logging (预写日志)

> [!abstract] 核心考点：WAL 预写日志保证持久性、先写日志再写数据、redo log 崩溃恢复能力

## WAL 的核心思想

**WAL（Write-Ahead Logging）：** 在将数据写入磁盘之前，先确保日志已经写入磁盘。

```
常规写入（无 WAL）：
  内存修改 → 刷脏页到磁盘 → 如果崩溃，未刷盘的数据丢失

WAL 写入：
  内存修改 → 写 redo log（顺序 IO，很快）→ 保证已落盘 → 后续刷脏页
  如果崩溃 → 从 redo log 恢复（前滚）
```

**为什么 WAL 快：**

| 操作 | redo log 写入 | 数据页写入 |
|------|-------------|-----------|
| IO 类型 | 顺序写入 | 随机写入 |
| 单次 IO 量 | 小（KB 级） | 大（16KB 页） |
| 写入位置 | 固定文件末尾 | 分散在不同位置 |
| 写入效率 | 极高 | 低（需要寻道） |

## Redo Log 的写入流程

```
UPDATE user SET balance=100 WHERE id=1;

1. Buffer Pool 中找到 id=1 的页（如果不在则从磁盘加载）
2. 修改页中 balance=100（脏页）
3. 生成 redo log 记录（记录的是"页 5 的偏移量 100 处写入 100"）
4. 将 redo log 写入 redo log buffer
5. redo log buffer → 刷入 redo log file（磁盘）
   ↓ 在所选持久化策略、存储设备与系统故障模型的假设下，可用于崩溃恢复
6. 后台线程或其他时机刷脏页到磁盘
```

**关键点：** redo 已按策略持久化后，崩溃恢复可重放必要修改；但数据库持久性还受硬件写缓存、操作系统/电源故障、binlog 配置和复制拓扑影响，不能简化成“任何故障都零丢失”。

## Redo Log Buffer 与刷盘时机

```ini
innodb_log_buffer_size = 16M

innodb_flush_log_at_trx_commit = 1
```

**innodb_flush_log_at_trx_commit 取值：**

| 值 | 行为 | 安全性 | 性能 |
|----|------|-------|------|
| 1（常见默认） | 每次事务提交请求日志落盘 | 在正确的存储持久化假设下最强 | 开销较高 |
| 2 | 每次事务提交写入 OS cache，每秒刷盘 | 次高（OS 崩溃丢 1s 数据） | 快 |
| 0 | 每秒写入 OS cache + 刷盘 | 最低（MySQL 崩溃丢 1s 数据） | 最快 |

**工程权衡：**
```
金融/支付场景 → innodb_flush_log_at_trx_commit = 1
日志/非关键数据 → innodb_flush_log_at_trx_commit = 2
批量导入 → innodb_flush_log_at_trx_commit = 0（然后设为 1）
```

## Undo Log 的作用

Undo Log 是 WAL 的另一面——记录**修改前的数据**，用于事务回滚和 MVCC：

```
UPDATE user SET balance=100 WHERE id=1;

redo log: 记录"页 5 偏移 100 → 写入 100"（重做）
undo log: 记录"id=1 的 balance 原来 = 50"（撤销）

事务回滚时：
  读取 undo log → 将数据恢复到修改前的状态

MVCC 快照读时：
  如果需要读取旧版本 → 通过 undo log 版本链找到
```

## WAL 与 ACID 的对应

```
A（原子性）← undo log：事务回滚
C（一致性）← undo + redo：事务要么全部完成要么全部回滚
I（隔离性）← undo log：MVCC 快照读
D（持久性）← redo log：WAL 保证即使崩溃也不丢数据
```

> [!tip]- **工程要点**：WAL 把“日志先于数据页持久化”作为恢复基础。`innodb_flush_log_at_trx_commit` 的选择是耐久性、延迟和设备语义之间的权衡；任何可承受丢失窗口或性能提升倍数都必须以当前版本、存储栈和压测结果验证。redo/undo 的具体记录格式也属于实现细节。

## 30 秒回答

**redo 和 undo 分别解决什么？** redo 支持崩溃后的重做，保证已提交修改可恢复；undo 保存旧版本，用于回滚与 MVCC。WAL 的核心顺序是先让恢复所需日志按策略持久化，再异步刷数据页。

---



崩溃恢复详解见 → [Redo Log：Crash Recovery (崩溃恢复)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07b-Redo%20Log：Crash%20Recovery%20(崩溃恢复).md) · [Binlog vs Redo Log：Differences (两者区别)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07c-Binlog%20vs%20Redo%20Log：Differences%20(两者区别).md)

---

## 16-Redo Log and Crash Recovery (Redo 日志与崩溃恢复)

> [!abstract] 核心考点：redo log 物理日志记录页修改、崩溃恢复前滚、checkpoint 机制与循环写

## Redo Log 的物理结构

Redo Log 是**物理日志**——记录的是"在某个页的某个偏移量写入了什么数据"，而非 SQL 语句。

```
redo log 记录格式：
  ┌─────────┬─────────┬──────────┬──────────┐
  │  type   │ space_id│ page_no  │  data    │
  ├─────────┼─────────┼──────────┼──────────┤
  │ MLOCK_1 │    5    │   100    │ ...      │
  │ MLOCK_1 │    5    │   100    │ ...      │
  │ MLOCK_1 │    5    │   101    │ ...      │
  └─────────┴─────────┴──────────┴──────────┘

type = MLOCK_1: 写入不大于 512 字节的数据
        MLOCK_2: 写入不大于 1024 字节的数据
        ... 多种类型适配不同大小的修改

记录内容示例：
  "在 space_id=5 的 表空间，page_no=100 的页，偏移量 812 处，写入 8 字节数据"
```

**Redo Log 文件配置：**
```ini
innodb_log_file_size = 512M

innodb_log_files_in_group = 3

```

## Redo Log 的循环写入

Redo log 文件不是无限增长的——它使用**固定大小的循环缓冲区**：

```
redo log 文件组（3 个文件，循环使用）：

  File 0     File 1     File 2
  ┌──────┐   ┌──────┐   ┌──────┐
  │      │   │      │   │      │
  └──────┘   └──────┘   └──────┘
      ↑                     ↑
  write_pos              checkpoint 
  (当前写入位置)           (已刷盘的安全点位)

  当 write_pos 追到 checkpoint 时 → 强制刷脏页 → 推进 checkpoint
```
 
## Checkpoint 机制

Checkpoint 解决了两个问题：
1. **缩减恢复时间**：不需要重放所有 redo log，只需重放 checkpoint 之后的
2. **重用 redo log 空间**：checkpoint 之前的 redo log 可以被覆盖

```
崩溃恢复范围：
  ┌─────────────────────────────────────────────────┐
  │  checkpoint ────→ crash ──────────────────────→ │
  │      │                          │               │
  │  All dirty pages             Redo logs          │
  |  flushed to disk             to replay          |
  │  (No recovery required)      (Forward recovery) │
  └─────────────────────────────────────────────────┘
```

**InnoDB 的 Fuzzy Checkpoint：**
- 不需要每次 checkpoint 刷所有脏页（不像 Oracle 的完全 checkpoint）
- 渐进式推进 checkpoint LSN（Log Sequence Number）
- Page Cleaner 线程后台持续刷脏页
- 当 redo log 空间即将用尽时，加速刷脏页

## 崩溃恢复流程

```text
┌──────────────────────────────┐
│  Database Crash Occurs       │
└──────────┬───────────────────┘
           │
           ▼
┌──────────────────────────────┐
│  Instance Startup            │
│  Read checkpoint LSN         │
└──────────┬───────────────────┘
           │
           ▼
┌──────────────────────────────┐
│  Scan Redo Log               │
│  (checkpoint to end)         │
└──────────┬───────────────────┘
           │
           ▼
┌────────────────────────────────────────────┐
│  REDO Phase (Roll-Forward)                 │
├────────────────────────────────────────────┤
│  1. Find pages where page_lsn < redo_lsn   │
│  2. Re-apply changes from redo log         │
└──────────┬─────────────────────────────────┘
           │
           ▼
┌────────────────────────────────────────────┐
│  UNDO Phase (Rollback)                     │
├────────────────────────────────────────────┤
│  1. Scan undo log for uncommitted          │
│     transactions                           │
│  2. Roll back modifications from           │
│     uncommitted transactions               │
└──────────┬─────────────────────────────────┘
           │
           ▼
┌───────────────────────────────┐
│  Recovery Complete            │
│  Data Consistency Guaranteed  │
└───────────────────────────────┘
```

## LSN（Log Sequence Number）

LSN 是 redo log 的唯一标识，单调递增：

```
LSN 的作用：
  - 标识 redo log 中的位置
  - 每个数据页记录最近修改的 LSN（page_lsn）
  - 恢复时：page_lsn < redo_log_lsn → 需要重放
  - Checkpoint 位置由 LSN 记录

查看 LSN：
  SHOW ENGINE INNODB STATUS\G
  ---
  LOG
  Log sequence number          123456789    ← 当前写入位置
  Log flushed up to            123456000    ← 已刷盘位置
  Last checkpoint at           123400000    ← checkpoint 位置
```

> [!tip]- **工程要点**：redo log 太小可能增加 checkpoint 压力并造成写入抖动；过大又会拉长恢复扫描窗口。容量、checkpoint 进度与恢复时间受版本、写入模式、设备和恢复流程影响，不能套用固定“小时数、百分比或分钟数”公式。应采集日志生成速率、脏页与恢复演练数据后再调优。

## 30 秒回答

**Checkpoint 为什么重要？** 它记录了恢复时不必再从更早日志重放的位置。redo log 过小会更频繁推动脏页刷盘，过大则可能扩大恢复工作量；大小必须以写入负载、存储能力和可接受恢复时间共同决定。

---



WAL机制详解见 → [WAL：Write-Ahead Logging (WAL机制)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07a-WAL：Write-Ahead%20Logging%20(WAL机制).md) · [Binlog vs Redo Log：Differences (两者区别)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07c-Binlog%20vs%20Redo%20Log：Differences%20(两者区别).md)

---

## 17-Binlog and Redo Log (Binlog 与 Redo Log)

> [!abstract] 核心考点：binlog 逻辑日志与 redo log 物理日志区别、binlog 三种格式（STATEMENT/ROW/MIXED）、两阶段提交

## Binlog vs Redo Log 概览

| 特性 | Redo Log | Binlog |
|------|----------|--------|
| 所属层 | InnoDB 存储引擎层 | MySQL Server 层 |
| 日志性质 | **物理日志**（页修改） | **逻辑日志**（SQL 或行变更） |
| 写入方式 | 循环写入（固定大小覆盖） | 追加写入（不断增长，可轮转） |
| 用途 | 崩溃恢复（Crash Recovery） | 主从复制、时间点恢复（PITR） |
| 记录内容 | "在页 X 的偏移 Y 写入了 Z" | "执行了 SQL" 或 "某行从 A 变成 B" |
| 同步方式 | 事务提交时刷盘（由参数控制） | 事务提交时刷盘（由 sync_binlog 控制） |

## 物理日志 vs 逻辑日志

```
Redo Log（物理日志）：
  "space_id=5, page_no=100, offset=812, write 8 bytes: 0x0000000000000064"
  → 记录的是最底层的页修改，与 SQL 语句无关

Binlog（逻辑日志）：
  "UPDATE user SET balance=100 WHERE id=1"
  → 记录的是业务含义的 SQL 语句或行变更
```

**物理日志的优点：** 
- 记录的是"怎么改"，与具体 SQL 无关，更纯粹
- 崩溃恢复时只需按位置重放，不需要理解 SQL 语义

**逻辑日志的优点：**
- 跨版本兼容（不同 MySQL 版本的页结构不同，但 SQL 兼容）
- 主从复制时主库和从库可以不同版本
- 支持时间点恢复（可以恢复到任意一秒）

## Binlog 三种格式

### STATEMENT 格式

```sql
-- 配置
SET SESSION binlog_format = STATEMENT;

-- 记录的是原始 SQL
UPDATE user SET balance=balance-100 WHERE id=1;
-- binlog 中记录：UPDATE user SET balance=balance-100 WHERE id=1;
```

**问题：** 非确定性函数可能导致主从不一致。
```sql
UPDATE user SET update_time = NOW() WHERE id=1;
-- 主库执行时 NOW() = 2024-01-01 12:00:00
-- 从库重放时 NOW() = 2024-01-01 12:05:30  ← 不一致！
```

### ROW 格式（MySQL 5.7+ 默认）

```sql
-- 记录的是每一行修改前后的值
UPDATE user SET balance=balance-100 WHERE id=1;
-- binlog 中记录：
--   BEFORE: {id=1, name='Bob', balance=500}
--   AFTER:  {id=1, name='Bob', balance=400}
```

**优点：** 相比 statement 格式更能准确表达行变更，减少许多语义差异；仍要考虑版本、复制配置、DDL 与故障恢复边界
**缺点：** 批量操作会产生大量 binlog 数据

### MIXED 格式

MySQL 自动判断：如果 SQL 是确定性的，用 STATEMENT；否则用 ROW。

```sql
-- 确定性 SQL → STATEMENT
UPDATE user SET balance=0 WHERE id=1;
-- binlog: UPDATE user SET balance=0 WHERE id=1

-- 非确定性 SQL → ROW
UPDATE user SET update_time=NOW() WHERE id=1;
-- binlog: 记录行修改前后的完整值
```

## 两阶段提交（Two-Phase Commit）

Binlog 和 Redo Log 需要在事务提交时保持一致——两阶段提交解决这个问题。

```
事务提交的两个阶段：

                    Prepare Phase
                         ↓
    ① Redo Log 写入 Prepare 状态（此时事务处于 prepare 阶段）
    ② 写入 Binlog（binlog 是协调者）
                         ↓
                    Commit Phase
                         ↓
    ③ Redo Log 写入 Commit 状态（事务正式提交）

异常场景分析：
  崩溃发生在 ① 之后、② 之前：
    → 重启后 redo log 是 prepare 状态，binlog 没有该事务
    → 回滚事务（undo log 回滚）

  崩溃发生在 ② 之后、③ 之前：
    → 重启后 redo log 是 prepare 状态，但 binlog 中已有该事务
    → 提交事务（redo log 完成 commit）

  崩溃发生在 ③ 之后：
    → redo log 和 binlog 都已记录 → 正常恢复
```

**为什么需要两阶段提交：**
```
假设没有两阶段提交：

方案 A：先写 binlog，再写 redo log
  binlog 写完 → 崩溃 → redo log 没有该事务
  从库同步了 binlog → 主库未提交 → 主从不一致

方案 B：先写 redo log，再写 binlog
  redo log commit → 崩溃 → binlog 没有该事务
  主库已包含数据 → 从库未同步 → 主从不一致

两阶段提交保证了 redo log 和 binlog 的最终一致性。
```

```sql
-- 查看 binlog 相关信息
SHOW BINARY LOGS;                              -- 所有 binlog 文件列表
SHOW MASTER STATUS;                             -- 当前正在写的 binlog
SHOW BINLOG EVENTS IN 'mysql-bin.000001';       -- binlog 事件内容
```

> [!tip]- **工程要点**：`binlog_format`、`sync_binlog`、redo flush 策略与复制拓扑需要按恢复目标和性能预算配置；ROW 常用于降低复制语义差异，但不等于“复制永不丢失”。两阶段提交协调 binlog 与 redo 的提交恢复判定，仍不能替代副本确认、备份和故障演练。

## 30 秒回答

redo log 服务于 InnoDB 崩溃恢复，binlog 服务于复制与时间点恢复；二者作用层与写入方式不同。提交时的两阶段协调让重启后能根据 binlog 判断 prepare 事务的去向，避免主库页恢复与复制日志明显分叉。真正的数据保护还依赖刷盘策略、副本、备份与恢复演练。

---



WAL机制详解见 → [WAL：Write-Ahead Logging (WAL机制)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07a-WAL：Write-Ahead%20Logging%20(WAL机制).md) · [Redo Log：Crash Recovery (崩溃恢复)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07b-Redo%20Log：Crash%20Recovery%20(崩溃恢复).md)