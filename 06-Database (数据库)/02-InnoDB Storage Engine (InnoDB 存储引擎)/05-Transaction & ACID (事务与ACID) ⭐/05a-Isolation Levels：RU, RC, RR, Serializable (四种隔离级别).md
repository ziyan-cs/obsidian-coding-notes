---
tags:
  - database/innodb
status: seed
review_due: 2026-10-03
confidence: 1
verified: stable
---

# Isolation Levels：RU, RC, RR, Serializable — 四种隔离级别

> [!important] **核心考点**：四种隔离级别（RU/RC/RR/Serializable）的并发问题防护能力、MySQL InnoDB 默认 RR 级别

## SQL 标准隔离级别

SQL 标准定义了四种隔离级别，从低到高依次递增防护能力：

```text
┌─────────────────────────────────────────────────────────────────────┐
│  SQL Standard Four Isolation Levels                                 │
├─────────────────────────────────────────────────────────────────────┤
│  READ UNCOMMITTED                                                   │
│    Dirty Read: Yes      Non-repeatable Read: Yes      Phantom: Yes  │
├─────────────────────────────────────────────────────────────────────┤
│       │  (isolation increases, concurrency decreases)               │
│       ▼                                                             │
│  READ COMMITTED                                                     │
│    Dirty Read: No       Non-repeatable Read: Yes      Phantom: Yes  │
├─────────────────────────────────────────────────────────────────────┤
│       │  (isolation increases, concurrency decreases)               │
│       ▼                                                             │
│  REPEATABLE READ (MySQL Default)                                    │
│    Dirty Read: No       Non-repeatable Read: No       Phantom: Yes  │
├─────────────────────────────────────────────────────────────────────┤
│       │  (isolation increases, concurrency decreases)               │
│       ▼                                                             │
│  SERIALIZABLE                                                       │
│    Dirty Read: No       Non-repeatable Read: No       Phantom: No   │
└─────────────────────────────────────────────────────────────────────┘

Note: InnoDB 的一致性快照读在 RR 下可保持事务内视图一致；锁定读/写入的范围保护与 Next-Key Lock 有关。具体“幻读”必须区分快照读与 locking read。
```

## RU（Read Uncommitted，读未提交）

```sql
SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
```

**行为：** 事务可以读到其他事务未提交的数据修改。

```
事务 A: BEGIN; UPDATE account SET balance=0 WHERE id=1;
事务 B: SELECT balance FROM account WHERE id=1;  -- 读到 0（未提交！）
事务 A: ROLLBACK;  -- 回滚
事务 B: 之前读到的 0 是脏数据
```

**问题：** 脏读。实际生产中几乎不用。

## RC（Read Committed，读已提交）

```sql
SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;
```

**行为：** 事务只能读到其他事务已提交的数据修改。

```
事务 A: BEGIN; UPDATE account SET balance=0 WHERE id=1;
事务 B: SELECT balance FROM account WHERE id=1;  -- 读到 100（原始值）
事务 A: COMMIT;
事务 B: SELECT balance FROM account WHERE id=1;  -- 读到 0（两次结果不同）
```

**问题：** 不可重复读——同一事务内两次相同的 SELECT 返回不同结果。

**实现：** 每条语句开始时生成一个 Read View（MVCC）。

## RR（Repeatable Read，可重复读）

```sql
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;
```

**行为：** 事务内多次读取同一行数据结果一致（通过 MVCC 快照）。

```
事务 A: BEGIN;
事务 A: SELECT * FROM t WHERE id=1;  -- 读到 {id=1, val=10}
事务 B: UPDATE t SET val=20 WHERE id=1; COMMIT;
事务 A: SELECT * FROM t WHERE id=1;  -- 仍读到 {id=1, val=10}（快照读）
```

**InnoDB 默认隔离级别**。RR 下普通一致性读依靠事务级 Read View；`FOR UPDATE` 等锁定读的范围保护由 next-key/gap locking 等机制决定，需结合索引与语句分析。

## Serializable（可串行化）

```sql
SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;
```

**行为：** InnoDB 会让普通读具有锁定读语义，从而显著减少可并发的读写组合；它不是简单地把所有事务排成一个全局队列。

```
事务 A: SELECT * FROM t WHERE id=1;  -- 加共享锁
事务 B: UPDATE t SET val=20 WHERE id=1;  -- 等待事务 A 释放锁！
```

**问题：** 并发性能极低。只有数据一致性要求极严格的场景使用。

## InnoDB 隔离级别对照

| 隔离级别 | MVCC 快照 | 使用的锁 | 常见场景 |
|---------|----------|---------|---------|
| RU | 无 | 无（读不加锁） | 极少使用 |
| RC | 一致性读通常为每语句 Read View | 锁范围依语句、索引、约束而定 | 需要较弱读隔离时的候选 |
| RR | 一致性读通常复用事务内 Read View | 锁定读可能使用 record/next-key/gap lock | InnoDB 默认，需理解范围锁 |
| Serializable | 读具锁定语义 | 并发能力显著下降 | 极少数需强隔离且可接受代价的操作 |

## 如何选择隔离级别

**RC vs RR 生产选择：**
```
RC（如 PostgreSQL 默认、Oracle、SQL Server）：
  - 不会有 Gap Lock 竞争，死锁概率低
  - 但不可重复读在业务层面可以接受（通常以最终结果为准）
  - 场景：高并发 OLTP 系统

RR（InnoDB 默认）：
  - 事务内读一致性好
  - Gap Lock 可能导致更多的锁等待和死锁
  - 场景：需要一致性读的报表生成、备份
```

```sql
-- 修改隔离级别
-- 全局
SET GLOBAL TRANSACTION ISOLATION LEVEL READ COMMITTED;

-- 会话
SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;

-- 下一个事务
SET TRANSACTION ISOLATION LEVEL READ COMMITTED;
```

> [!tip]- **工程要点**：选择 RC 还是 RR 应从业务读语义、锁定读、死锁模式、复制配置和压测结果出发，不要把任一隔离级别当成通用最优解。RR 下的范围锁可能增加锁等待；RC 也不是“完全没有 gap lock”，外键/重复键检查等场景仍要以当前版本文档验证。

## 30 秒回答

隔离级别决定并发事务能观察到什么，以及为此付出的锁与并发代价。InnoDB 的 RC 通常每条一致性读建立视图，RR 通常让同一事务的快照读保持一致；但锁定读、索引范围和约束会改变实际锁行为。排查问题时先区分“快照读还是锁定读”，再看执行计划和锁信息。

---



三大并发问题详解见 → [Dirty Read, Non-repeatable Read, Phantom Read (三大并发问题)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05b-Dirty%20Read,%20Non-repeatable%20Read,%20Phantom%20Read%20(三大并发问题).md) · [MVCC Internals：undo log & read view (MVCC底层实现)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05c-MVCC%20Internals：undo%20log%20&%20read%20view%20(MVCC底层实现).md)
