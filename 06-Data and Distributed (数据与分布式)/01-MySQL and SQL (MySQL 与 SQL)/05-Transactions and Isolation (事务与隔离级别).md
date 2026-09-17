---
status: stable
confidence: high
verified: 2026-09-17
---

> [!summary]- 复述检查：学完后再展开
>
> **回答**：事务用原子性、一致性、隔离性和持久性约束一组读写；隔离级别决定并发事务能观察到哪些中间状态。选择级别时要把脏读、不可重复读、幻读与业务不变量对应起来，而不是默认隔离越强越好。

# 隔离级别与并发现象

> [!note] 本节重点：四种隔离级别（RU/RC/RR/Serializable）的并发问题防护能力、MySQL InnoDB 默认 RR 级别

## SQL 标准隔离级别

SQL 标准定义了四种隔离级别，从低到高依次递增防护能力：

```text
SQL isolation, from lower to higher isolation
  - READ UNCOMMITTED: dirty / non-repeatable / phantom reads possible
  - READ COMMITTED: non-repeatable and phantom reads possible
  - REPEATABLE READ: snapshot reads stay consistent in an InnoDB transaction
  - SERIALIZABLE: strongest isolation, generally least concurrent

InnoDB range protection for locking reads and writes involves Next-Key Lock.
Always distinguish snapshot reads from locking reads when discussing phantoms.
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

# Transaction Anomalies (事务并发异常)

> [!note] 本节重点：脏读（未提交数据）、不可重复读（同一行前后不同）、幻读（行数变化）三种并发问题

## 脏读（Dirty Read）

事务读到另一个事务**未提交**的数据。

```
时间线：
  事务 A                   事务 B
  ├── BEGIN                 │
  ├── UPDATE account        │
  │   SET balance=0         │
  │   WHERE id=1            │
  │                         │
  │── (Uncommitted) ───────→│── SELECT balance FROM account WHERE id=1
  │                         │    → Reads 0 (Dirty Read!)
  │                         │
  ├── ROLLBACK              │
  │   balance roll back 100 │
  │                         │
  │                         │── Business logic makes wrong decisions 
  |                         |   based on balance=0
```

**发生条件：** 隔离级别为 RU（Read Uncommitted）。

**解决：** 升级到 RC 或更高——只读已提交的数据。

## 不可重复读（Non-repeatable Read）

同一事务内两次读取**同一行**数据，结果不同（因为被其他事务修改并提交了）。

```
时间线：
  事务 A                   事务 B
  ├── BEGIN                 │
  ├── SELECT balance        │
  │   WHERE id=1            │
  │   → 100                 │
  │                         │
  │                         ├── UPDATE account SET balance=0 WHERE id=1
  │                         ├── COMMIT
  │                         │
  ├── SELECT balance        │
  │   WHERE id=1            │
  │   → 0                   |  ← (Different from previous read!)
  |                         |
  ├── COMMIT                │
```

**影响：** 如果事务 A 第一次读 balance=100 时判断余额充足，第二次读变成 0，业务逻辑不一致。

**发生条件：** RC 或更低隔离级别。

**解决：** RR 或更高——通过 MVCC 快照隔离同一事务的多次读取。

## 幻读（Phantom Read）

同一事务内两次查询**同一条件**，返回的行数不同（其他事务插入了新行）。

```
时间线：
  事务 A                   事务 B
  ├── BEGIN                 │
  ├── SELECT * FROM user    │
  │   WHERE age > 20        │
  │   → 10 rows             │
  │                         │
  │                         ├── INSERT INTO user(name, age) VALUES('Bob', 25)
  │                         ├── COMMIT
  │                         │
  ├── SELECT * FROM user    │
  │   WHERE age > 20        │
  │   → 11 rows             |  ← One more new row (Phantom Read!)
  |                         |
  ├── COMMIT                │
```

**不可重复读 vs 幻读：**
```
不可重复读：同一行数据内容变化（UPDATE）
幻读：      行数变化（INSERT/DELETE）
```

**发生条件：** RR 或更低（标准 SQL 定义中，RR 允许幻读）。

**InnoDB 的解决方案：** Gap Lock（间隙锁），在 RR 级别也防止了幻读。但对于快照读（普通 SELECT），MVCC 本身已经避免了幻读——只有当前读（SELECT ... FOR UPDATE/LOCK IN SHARE MODE）才需要 Gap Lock 防护。

## 三类并发问题的对比

| 问题 | 本质 | 操作类型 | 避免级别 | InnoDB 解决方式 |
|------|------|---------|---------|----------------|
| 脏读 | 读到未提交数据 | UPDATE 后未 COMMIT | RC 及以上 | MVCC Read View 过滤未提交事务 |
| 不可重复读 | 同行的数据不一致 | UPDATE + COMMIT | RR 及以上 | MVCC 事务级快照 |
| 幻读 | 行数不一致 | INSERT/DELETE + COMMIT | Serializable | Gap Lock + MVCC |

# 实战排查

```sql
-- 检查当前会话的隔离级别
SELECT @@transaction_isolation;

-- 检查全局隔离级别
SELECT @@global.transaction_isolation;

-- 监控当前是否有长时间运行的事务
SELECT trx_id, trx_state, trx_started, trx_mysql_thread_id
FROM information_schema.innodb_trx
WHERE trx_started < NOW() - INTERVAL 5 SECOND;

-- 查看当前锁等待
SELECT * FROM performance_schema.data_lock_waits\G
```

> [!tip]- **工程要点**：脏读在实践中很少遇到（RU 极少使用）。不可重复读在高并发写入场景很常见，RC 级别下如果业务要求同一事务内读一致性，需要应用层加锁。幻读最容易在"检查且插入"的场景出现（如注册时检查用户名是否存在），Gap Lock 在 RR 级别下防护，但在 RC 级别下需要用应用层唯一约束或悲观锁来防止。

---

四种隔离级别详解见 → Isolation Levels：RU, RC, RR, Serializable (四种隔离级别) · MVCC Internals：undo log & read view (MVCC底层实现)

---
