---
tags:
  - database/innodb
status: seed
review_due: 2026-10-03
confidence: 1
verified: stable
---

# Binlog vs Redo Log：Differences — 两者区别

> [!important] **核心考点**：binlog 逻辑日志与 redo log 物理日志区别、binlog 三种格式（STATEMENT/ROW/MIXED）、两阶段提交

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

**优点：** 精确的记录每一行变化，没有任何不一致风险
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

> [!tip]- **工程要点**：生产环境推荐 `binlog_format=ROW`（保证主从一致性）+ `sync_binlog=1`（每次事务提交刷盘）。两阶段提交是 MySQL 数据一致性的基石——理解了它，才能真正理解为什么主从复制不会丢数据。**面试核心问题：** "MySQL 如何保证 binlog 和 redo log 的一致性？" → 答案就是两阶段提交。

---



WAL机制详解见 → [WAL：Write-Ahead Logging (WAL机制)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07a-WAL：Write-Ahead%20Logging%20(WAL机制).md) · [Redo Log：Crash Recovery (崩溃恢复)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07b-Redo%20Log：Crash%20Recovery%20(崩溃恢复).md)
