---
tags:
  - database
  - innodb
---

> **核心考点**：脏读（未提交数据）、不可重复读（同一行前后不同）、幻读（行数变化）三种并发问题

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
  │── (未提交) ────────────→│── SELECT balance FROM account WHERE id=1
  │                         │    → 读到 0（脏数据！）
  │                         │
  ├── ROLLBACK（回滚）       │
  │   balance 恢复到 100    │
  │                         │
  │                         │── 业务逻辑基于 balance=0 做了错误的决策
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
  │   WHERE id=1              │
  │   → 100                   │
  │                           │
  │                         ├── UPDATE account SET balance=0 WHERE id=1
  │                         ├── COMMIT
  │                           │
  ├── SELECT balance        │
  │   WHERE id=1              │
  │   → 0（和上次不一样！）   │
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
  │   WHERE age > 20          │
  │   → 10 行                 │
  │                           │
  │                         ├── INSERT INTO user(name, age) VALUES('Bob', 25)
  │                         ├── COMMIT
  │                           │
  ├── SELECT * FROM user    │
  │   WHERE age > 20          │
  │   → 11 行（多了一行！）   │  ← 幻读
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

## 实战排查

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

> **工程要点**：脏读在实践中很少遇到（RU 极少使用）。不可重复读在高并发写入场景很常见，RC 级别下如果业务要求同一事务内读一致性，需要应用层加锁。幻读最容易在"检查且插入"的场景出现（如注册时检查用户名是否存在），Gap Lock 在 RR 级别下防护，但在 RC 级别下需要用应用层唯一约束或悲观锁来防止。

---

## 关联笔记

- [Isolation Levels：RU, RC, RR, Serializable (四种隔离级别)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05a-Isolation%20Levels：RU,%20RC,%20RR,%20Serializable%20(四种隔离级别).md)
- [MVCC Internals：undo log & read view (MVCC底层实现)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05c-MVCC%20Internals：undo%20log%20&%20read%20view%20(MVCC底层实现).md)
- [DDL, DML, DQL (SQL基础语法)](/06-Database%20(MySQL)/01%20·%20SQL基础/01-DDL,%20DML,%20DQL%20(SQL基础语法).md)
- [Joins & Subqueries (多表查询与子查询)](/06-Database%20(MySQL)/01%20·%20SQL基础/02-Joins%20&%20Subqueries%20(多表查询与子查询).md)
- [MySQL Basics (MySQL 基础)](/06-Database%20(MySQL)/01%20·%20SQL基础/02-MySQL%20Basics%20(MySQL%20基础).md)
