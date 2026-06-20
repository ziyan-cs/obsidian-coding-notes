---
tags:
  - database/innodb
status: 🌱
---

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

Note: InnoDB's RR level prevents phantom reads via MVCC + Next-Key Lock.
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

**InnoDB 默认隔离级别**。RR 在标准 SQL 中允许幻读，但 InnoDB 通过 Gap Lock 在 RR 级别也防止了幻读。

## Serializable（可串行化）

```sql
SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;
```

**行为：** 所有事务串行执行，通过锁实现。读加共享锁，写加排他锁。

```
事务 A: SELECT * FROM t WHERE id=1;  -- 加共享锁
事务 B: UPDATE t SET val=20 WHERE id=1;  -- 等待事务 A 释放锁！
```

**问题：** 并发性能极低。只有数据一致性要求极严格的场景使用。

## InnoDB 隔离级别对照

| 隔离级别 | MVCC 快照 | 使用的锁 | 常见场景 |
|---------|----------|---------|---------|
| RU | 无 | 无（读不加锁） | 极少使用 |
| RC | 每语句 Read View | Record Lock | 大部分业务系统 |
| RR | 事务级 Read View | Record Lock + Gap Lock | 默认级别，复杂查询 |
| Serializable | 无快照 | 所有读加共享锁 | 银行/金融类操作 |

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

> [!tip]- **工程要点**：绝大多数业务系统选 RC 就足够了。InnoDB 默认 RR 是历史原因（MySQL 主从复制基于 binlog，RC 在 statement 格式下主从不一致），但 MySQL 5.7+ 的 ROW 格式 binlog 下 RC 也安全。**RR 的 Gap Lock 是双刃剑——避免幻读的同时也增加了锁竞争和死锁概率。**

---



三大并发问题详解见 → [Dirty Read, Non-repeatable Read, Phantom Read (三大并发问题)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05b-Dirty%20Read,%20Non-repeatable%20Read,%20Phantom%20Read%20(三大并发问题).md) · [MVCC Internals：undo log & read view (MVCC底层实现)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05c-MVCC%20Internals：undo%20log%20&%20read%20view%20(MVCC底层实现).md)
