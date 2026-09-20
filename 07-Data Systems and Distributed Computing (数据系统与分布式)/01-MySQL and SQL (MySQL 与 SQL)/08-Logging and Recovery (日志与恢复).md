---
study_stage: backlog
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# Write Ahead Logging (预写日志)

> [!note] 本节重点：WAL 预写日志保证持久性、先写日志再写数据、redo log 崩溃恢复能力

## WAL 的核心思想

**WAL（Write-Ahead Logging）：** 在脏数据页持久化之前，先保证恢复它所需的 redo 已按所选策略持久化。事务提交是否足够耐久，还取决于 redo/binlog 刷盘策略及存储设备是否兑现落盘语义。

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
| 写入组织 | 顺序追加、可批量/组提交 | 后续按脏页刷新 |
| 作用 | 缩短提交关键路径，提供恢复信息 | 将最终页状态写入表空间 |
| 注意 | 仍有刷盘及容量成本 | SSD 也有随机写成本，但不能照搬机械盘寻道模型 |

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
| 2 | 提交时写入 OS cache，后台周期性刷盘 | 主机/OS 崩溃可能丢已确认事务；周期不是严格 1 秒上界 | 延迟通常较低 |
| 0 | 后台周期性写入并刷盘 | mysqld/主机崩溃均可能丢已确认事务；窗口不保证恰为 1 秒 | 延迟通常较低 |

不要按“金融/日志/批量导入”机械套参数。先写清恢复点目标（RPO）、允许丢失的已确认事务、是否启用 binlog 与故障模型，再核对 `sync_binlog`、设备写缓存和备份策略。改全局刷盘参数会影响同实例其他业务。

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
C（一致性）← 业务约束、SQL 约束、事务语义共同维护，不是某个日志单独保证
I（隔离性）← MVCC + 锁与所选隔离级别
D（持久性）← redo 与刷盘/存储假设；binlog、复制与备份承担不同恢复目标
```

> [!tip]- **工程要点**：WAL 把“日志先于数据页持久化”作为恢复基础。`innodb_flush_log_at_trx_commit` 的选择是耐久性、延迟和设备语义之间的权衡；任何可承受丢失窗口或性能提升倍数都必须以当前版本、存储栈和压测结果验证。redo/undo 的具体记录格式也属于实现细节。

> [!summary] 核心摘要
>
> **redo 和 undo 分别解决什么？** redo 支持崩溃后的重做，保证已提交修改可恢复；undo 保存旧版本，用于回滚与 MVCC。WAL 的核心顺序是先让恢复所需日志按策略持久化，再异步刷数据页。
>
> ---
>
>
> ---

# Redo Log and Crash Recovery (Redo 日志与崩溃恢复)

> [!note] 本节重点：redo log 物理日志记录页修改、崩溃恢复前滚、checkpoint 机制与循环写

## Redo Log 的物理结构

Redo 记录 InnoDB 页级变更所需的恢复信息，而非原始 SQL；具体记录类型、格式与版本相关，不应把某一个字节覆盖示意当作真实统一格式。

```
概念示意：事务修改 Buffer Pool 页 → 产生 redo 记录 → 按提交策略刷盘
恢复时依据检查点与日志序号重放需要的页级变更，再处理未提交事务。
```

MySQL 8.4 应优先了解 `innodb_redo_log_capacity`；旧笔记常见的 `innodb_log_file_size` 与 `innodb_log_files_in_group` 已被其取代，不要照抄旧配置。实际容量应按写入速率、checkpoint 压力和恢复演练调整。[MySQL 8.4 官方说明](https://dev.mysql.com/doc/refman/8.4/en/innodb-parameters.html)

## Redo Log 的循环写入

Redo log 文件不是无限增长的——它使用**固定大小的循环缓冲区**：

```
逻辑上可将 redo 空间理解为有容量约束、可复用的日志区域：
  追加位置 ──────────→ 新 redo
  检查点 ───────────→ 此位置之前对应的脏页已满足复用条件

  写入逼近可复用边界时 → 增加刷脏页/checkpoint 压力，写入可能受限
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

# LSN（Log Sequence Number）

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

# Binlog and Redo Log (Binlog 与 Redo Log)

> [!note] 本节重点：binlog 逻辑日志与 redo log 物理日志区别、binlog 三种格式（STATEMENT/ROW/MIXED）、两阶段提交

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

**Binlog 的用途：** 复制与基于备份的时间点恢复（PITR）。恢复到目标时刻需要合适的全量备份、连续可用的 binlog、时间/位点选择与演练；它不是任意时刻都能“恢复到任意一秒”的按钮。跨版本复制也必须遵守官方支持矩阵，不能因其是逻辑日志就推断任意版本兼容。

## Binlog 三种格式

### STATEMENT 格式

STATEMENT 记录语句及执行所需上下文。部分依赖数据分布、执行顺序或系统状态的语句会被判为不安全；但 `NOW()` **不是**“副本重新取本机当前时间而必然不一致”的正确例子，MySQL 文档明确将其视为可安全复制的函数之一。不要在新环境中为了实验直接切换生产会话的 binlog 格式。

### ROW 格式（MySQL 8.4 默认）

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

MIXED 根据服务器对语句安全性的判断选择格式，并非“含非确定性函数一律 ROW”。MySQL 8.4 已将 `binlog_format` 标记为 deprecated，新复制部署优先使用 ROW；具体变更必须依照部署版本文档。[官方格式与安全性说明](https://dev.mysql.com/doc/refman/8.4/en/replication-rbr-safe-unsafe.html)

## 两阶段提交（Two-Phase Commit）

Binlog 和 Redo Log 需要在事务提交时保持一致——两阶段提交解决这个问题。

```
事务提交的两个阶段：

                    Prepare Phase
                         ↓
    ① Redo Log 写入 Prepare 状态（此时事务处于 prepare 阶段）
    ② 写入 Binlog，并按 sync_binlog 等策略持久化
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

该内部提交协议用于协调同一 MySQL 实例内 redo 与 binlog 的恢复判定；它**不是**跨数据库、跨服务的 XA/2PC，也不自动保证副本或外部消费者已持久处理。刷盘策略和设备故障仍可能影响可恢复性。
```

```sql
-- 查看 binlog 相关信息
SHOW BINARY LOGS;                              -- 所有 binlog 文件列表
SHOW BINARY LOG STATUS;                         -- MySQL 8.4 当前 binlog 文件与位置
SHOW BINLOG EVENTS IN 'mysql-bin.000001';       -- binlog 事件内容
```

> [!tip]- **工程要点**：`binlog_format`、`sync_binlog`、redo flush 策略与复制拓扑需要按恢复目标和性能预算配置；ROW 常用于降低复制语义差异，但不等于“复制永不丢失”。两阶段提交协调 binlog 与 redo 的提交恢复判定，仍不能替代副本确认、备份和故障演练。

> [!info]- 延伸阅读
> - 下一步：[09-Query Analysis and Optimization (查询分析与优化)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/01-MySQL%20and%20SQL%20(MySQL%20与%20SQL)/09-Query%20Analysis%20and%20Optimization%20(查询分析与优化).md)
