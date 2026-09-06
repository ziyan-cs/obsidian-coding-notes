---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Transactions MVCC and Locks (事务 MVCC 与锁)

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

## 30 秒回答

事务保证一组读写以明确的隔离语义完成；MVCC 让读通常不阻塞写，但不消除锁和冲突。设计时先写出业务不变量，再选择隔离级别、索引和重试策略；不要把默认 RR 误认为所有并发问题都会自动解决。

## 排查顺序

1. 写出事务中的读写序列和必须保持的不变量。
2. 检查访问路径是否命中合适索引；锁范围由索引与查询条件决定。
3. 区分一致性快照读与当前读；后者可能加记录锁、gap lock 或 next-key lock。
4. 对死锁保留日志并重试完整事务，而不是只重试其中一条 SQL。



## 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

## 常见误区

- MVCC 不是完全无锁；写操作与当前读仍需协调。
- 长事务并不更安全；它会占用 undo、阻塞清理并扩大锁冲突。
- 死锁不是数据库故障；并发系统中必须识别并有限重试。

## 自测

1. 快照读和当前读的可见性与加锁行为有什么不同？
2. 为什么缺失合适索引会扩大锁冲突？
3. 为什么事务重试必须从业务边界开始？

## Transaction Isolation Levels (事务隔离级别)

> [!note] 本节重点心考点：四种隔离级别（RU/RC/RR/Serializable）的并发问题防护能力、MySQL InnoDB 默认 RR 级别

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

## Transaction Anomalies (事务并发异常)

> [!note] 本节重点心考点：脏读（未提交数据）、不可重复读（同一行前后不同）、幻读（行数变化）三种并发问题

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

> [!tip]- **工程要点**：脏读在实践中很少遇到（RU 极少使用）。不可重复读在高并发写入场景很常见，RC 级别下如果业务要求同一事务内读一致性，需要应用层加锁。幻读最容易在"检查且插入"的场景出现（如注册时检查用户名是否存在），Gap Lock 在 RR 级别下防护，但在 RC 级别下需要用应用层唯一约束或悲观锁来防止。

---



四种隔离级别详解见 → [Isolation Levels：RU, RC, RR, Serializable (四种隔离级别)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05a-Isolation%20Levels：RU,%20RC,%20RR,%20Serializable%20(四种隔离级别).md) · [MVCC Internals：undo log & read view (MVCC底层实现)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05c-MVCC%20Internals：undo%20log%20&%20read%20view%20(MVCC底层实现).md)

---

## MVCC Internals (MVCC 底层实现)

> [!note] 本节重点心考点：> MVCC 通过 undo log 实现一致性读、Read View 可见性判断、快照读与当前读

## MVCC 核心思想

MVCC（Multi-Version Concurrency Control，多版本并发控制）允许**读不阻塞写，写不阻塞读**。

```
传统锁机制：
  事务 A 写行 1 → 行 1 被锁 → 事务 B 读行 1 → 等待

MVCC 机制：
  事务 A 写行 1 → 创建新版本（undo log 保留旧版本）
  事务 B 读行 1 → 读取快照版本（旧版本）→ 无需等待
```

## 三列隐藏字段

InnoDB 的每行记录有三个隐藏列：

```
行记录结构（隐藏列）：
  ┌─────────────┬─────────────┬──────────────────┐
  │ DB_TRX_ID   │ DB_ROLL_PTR │ DB_ROW_ID        │
  ├─────────────┼─────────────┼──────────────────┤
  │ Last Modify │ Pointer to  │ Auto-inc Row ID  │
  │ Transaction │ Undo Log    │ (No Primary Key) │
  └─────────────┴─────────────┴──────────────────┘

  ┌──────────────────────────────────────┐
  │  Business Columns (col1, col2, ...)  |
  └──────────────────────────────────────┘

DB_TRX_ID：最近修改此行的事务 ID（递增）
DB_ROLL_PTR：指向回滚段中的 undo log 记录（可找到历史版本）
```

## undo log 版本链

每次 UPDATE 产生一条 undo log，通过 DB_ROLL_PTR 串联成版本链：

```text
┌───────────────────────┐   DB_ROLL_PTR    ┌───────────────────────┐
│  Latest Version       │─────────────────►│  Undo Log             │
│  (current row data)   │                  │  TRX_ID = 80          │
│  DB_TRX_ID = 100      │                  │  balance = 100        |
│  balance = 0          │                  │  (previous version)   │
└───────────────────────┘                  └───────────┬───────────┘
                                                       │ DB_ROLL_PTR
                                                       ▼
                                           ┌───────────────────────────┐
                                           │  Undo Log                 │
                                           │  TRX_ID = 50              │
                                           │  balance = 200            │
                                           │  (older version)          │
                                           └───────────────────────────┘
```

## Read View 可见性判断

Read View 是 MVCC 实现的核心——它定义了"哪些事务的修改对当前事务可见"。

```text
┌─────────────────────────────────────────────────────────────────┐
│  Read View Structure                                            │
├─────────────────────────────────────────────────────────────────┤
│  creator_trx_id: Transaction ID that created this Read View     │
│  m_ids: List of active (uncommitted) transaction IDs            │
│  min_trx_id: Minimum value in m_ids                             │
│  max_trx_id: Next transaction ID to be assigned                 │
│              (greater than all active transactions)             │
└─────────────────────────────────────────────────────────────────┘
```

**可见性判断规则（判断 DB_TRX_ID）：**

```text
┌─────────────────────────────────────────────────────────────────┐
│  Visibility Rules (evaluated against DB_TRX_ID)                 │
├─────────────────────────────────────────────────────────────────┤
│  Rule 1: DB_TRX_ID == creator_trx_id                            │
│    → Modification by current transaction → VISIBLE              │
│                                                                 │
│  Rule 2: DB_TRX_ID < min_trx_id                                 │
│    → Committed before Read View creation → VISIBLE              │
│                                                                 │
│  Rule 3: DB_TRX_ID IN m_ids                                     │
│    → Modified by another active transaction → INVISIBLE         │
│                                                                 │
│  Rule 4: DB_TRX_ID >= max_trx_id                                │
│    → Transaction started after Read View creation → INVISIBLE   │
│                                                                 │
│  Rule 5: Otherwise (not in m_ids, < max_trx_id)                 │
│    → Committed before Read View creation → VISIBLE              │
└─────────────────────────────────────────────────────────────────┘
```

**图示（事务时间线与可见性）：**

```text
Transaction Timeline (ordered by DB_TRX_ID)

  Txn 1: committed, < min_trx_id → VISIBLE
    │
    │ min_trx_id boundary
    ▼
  Txn 2: active (IN m_ids) → INVISIBLE
    │
    ▼
  Txn 3: not in m_ids, < max_trx_id → VISIBLE
    │
    ▼
  Txn 4: active (IN m_ids) → INVISIBLE
    │
    │ max_trx_id boundary
    ▼
  Txn 5: current transaction
    │
    ▼
  Txn 6: >= max_trx_id → INVISIBLE
```

## 快照读 vs 当前读

```sql
-- 快照读（Snapshot Read）：读 MVCC 历史版本，不加锁
SELECT * FROM t WHERE id = 1;
SELECT * FROM t WHERE id = 1 LOCK IN SHARE MODE;  -- 注意：这是当前读！

-- 当前读（Current Read）：读最新版本，加锁
SELECT * FROM t WHERE id = 1 FOR UPDATE;           -- 排他锁
SELECT * FROM t WHERE id = 1 LOCK IN SHARE MODE;   -- 共享锁
UPDATE t SET val=10 WHERE id=1;                    -- 先当前读，再写
DELETE FROM t WHERE id=1;                          -- 先当前读，再删
INSERT INTO t VALUES(1, 10);                       -- 直接写
```

**Read View 的创建时机：**
```
RC（Read Committed）：
  每条语句执行前都创建新的 Read View
  → 同一事务内两次 SELECT 可能结果不同（不可重复读）

RR（Repeatable Read）：
  事务中第一个 SELECT 时创建 Read View，一直使用到事务结束
  → 同一事务内多次 SELECT 结果一致（可重复读）
```

## MVCC 实战场景

```
RR 级别下：
  事务 A: BEGIN;                      -- 尚未创建 Read View（RR 在首次快照读时创建）
  事务 A: SELECT balance FROM account; -- 读取 Read View 时最新的已提交版本
  事务 B: UPDATE balance; COMMIT;      -- 修改并提交（新版本，但事务 A 的 Read View 不可见）
  事务 A: SELECT balance FROM account; -- 仍读到旧版本（RR 快照隔离）
  事务 A: COMMIT;                      -- Read View 释放
```

> [!tip]- **工程要点**
> MVCC 的核心优势是"读不阻塞写"——这在 OLTP 系统中极其重要。快照读是 MVCC 的主角，不加任何锁。而当前读（SELECT ... FOR UPDATE）不走 MVCC，直接读最新数据并加锁——这在高并发下容易成为瓶颈。在线业务中，尽量用快照读，只在"检测并更新"（如扣库存）的原子操作中使用当前读。

## 30 秒回答 / 自测

- **30 秒回答**：MVCC 靠 undo log 版本链 + 隐藏列（DB_TRX_ID/DB_ROLL_PTR）实现读不阻塞写；快照读通过 Read View 判断可见性（creator/min/max + m_ids 规则），不加锁；当前读（FOR UPDATE）读最新版并加锁。RC 每条语句建新 Read View，RR 事务首个快照读建一次。
- **常见误区**：以为 RR 下 `BEGIN` 时就建 Read View（实际首次快照读才建）；把 `LOCK IN SHARE MODE` 当成快照读（它是当前读）。
- **自测**：1) Read View 里 min_trx_id / max_trx_id / m_ids 分别怎么判断可见性？ 2) 为什么 RR 靠 MVCC 只能防"快照读"的不可重复读，仍需 Next-Key Lock 防当前读的幻读？

---

RR 级别下 MVCC 搭配 Next-Key Lock 解决幻读 → [隔离级别](05a-Isolation%20Levels：RU,%20RC,%20RR,%20Serializable%20(四种隔离级别).md) · [Next-Key Lock](../06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06b-Gap%20Lock%20&%20Next-Key%20Lock%20(间隙锁与临键锁).md)

---

## Table and Row Locks (表锁与行锁)

> [!note] 本节重点心考点：表锁与行锁的开销与并发粒度对比、InnoDB 行锁基于索引实现、意向锁的作用

## 表锁 vs 行锁

| 特性 | 表锁（Table Lock） | 行锁（Row Lock） |
|------|-------------------|-----------------|
| 锁定粒度 | 整张表 | 单行记录 |
| 并发度 | 低（锁住整表） | 高（不同行不冲突） |
| 加锁开销 | 小 | 大（维护每行锁信息） |
| 死锁概率 | 低（不会交叉） | 高（行间循环等待） |
| MySQL 引擎 | MyISAM、InnoDB | 仅 InnoDB |
| DDL 操作 | 使用表锁 | 使用表锁（如 ALTER TABLE）|

**MyISAM vs InnoDB 锁差异：**
```
MyISAM：只支持表锁
  SELECT：共享读锁（不影响其他读，阻塞写）
  INSERT/UPDATE/DELETE：排他写锁（阻塞一切）

InnoDB：行锁 + 表锁
  SELECT：MVCC 快照读，不加锁（默认）
  SELECT ... FOR UPDATE：行级排他锁
  SELECT ... LOCK IN SHARE MODE：行级共享锁
  UPDATE/DELETE：行级排他锁（自动）
```

## InnoDB 行锁类型

```sql
-- 共享锁（S Lock）：允许其他事务读，阻塞写
SELECT * FROM t WHERE id = 1 LOCK IN SHARE MODE;

-- 排他锁（X Lock）：阻塞其他事务的读和写
SELECT * FROM t WHERE id = 1 FOR UPDATE;
UPDATE t SET val = 10 WHERE id = 1;   -- 自动加 X 锁
DELETE FROM t WHERE id = 1;            -- 自动加 X 锁

-- 兼容性：
--    S 和 S 兼容：两个事务可以同时持有同一行的 S 锁
--    S 和 X 互斥
--    X 和 X 互斥
```

## 行锁基于索引

InnoDB 的行锁不是锁"行"，而是锁**索引记录**。

```
场景：表 t 有主键索引 id 和普通索引 name

UPDATE t SET val = 10 WHERE id = 1;
  → 锁住 id=1 的聚簇索引记录（通过主键定位）

UPDATE t SET val = 10 WHERE name = 'Bob';
  → 锁住二级索引 name='Bob' + 对应的聚簇索引记录

UPDATE t SET val = 10 WHERE name = 'Bob' AND age = 20;
  → 如果没有索引，锁住整张表的所有行（退化为表锁！）
```

**关键工程结论：**
- **没有索引的 WHERE 条件 → 行锁升级为表锁**（所有行都被锁定）
- 通过二级索引加锁时，InnoDB 还需要锁住对应的聚簇索引记录
- 索引设计直接影响锁的粒度

## 意向锁（Intention Lock）

意向锁是**表级锁**，用于快速判断表中是否有行级锁，避免逐行检查。

```
事务 A 锁了行 1（行级 X 锁）：
  行 1: X 锁
  表: 意向排他锁（IX）← 自动添加

事务 B 想锁整张表（表级 X 锁）：
  检查到表上已有 IX → 立即知道有行被锁 → 等待

如果不有意向锁：
  事务 B 需要检查表中每一行 → O(n) 开销
```

**意向锁兼容性：**

| 锁类型 | IS | IX | S（表） | X（表） |
|--------|----|----|---------|---------|
| IS（意向共享） | 兼容 | 兼容 | 兼容 | 互斥 |
| IX（意向排他） | 兼容 | 兼容 | 互斥 | 互斥 |
| S（表共享） | 兼容 | 互斥 | 兼容 | 互斥 |
| X（表排他） | 互斥 | 互斥 | 互斥 | 互斥 |

## 行锁的实现开销

```
行锁的内存结构（lock_t）：
  - 事务 ID
  - 锁类型（S/X）
  - 锁模式（Record/Gap/Next-Key）
  - 索引表空间 ID + 页号
  - 堆栈号（heap_no，定位到行）

每个事务的行锁数量受 innodb_lock_wait_timeout 约束（默认 50s）
每行锁内存开销约 20-40 字节
```

```sql
-- 查看当前锁信息
SELECT * FROM performance_schema.data_locks\G
-- 查看锁等待
SELECT * FROM performance_schema.data_lock_waits\G
```

> [!tip]- **工程要点**：InnoDB 的行锁只有通过索引才能生效——没有索引的 WHERE 条件会退化为表锁，这是性能灾难的常见原因。大表 DELETE/UPDATE 操作尤其需要注意：一次操作影响 1 万行的范围更新会在短时间内加大量行锁，可能耗尽锁内存或引发大量锁等待。建议分批处理（如 LIMIT 1000 循环）。

---



间隙锁与临键锁详解见 → [Gap Lock & Next-Key Lock (间隙锁与临键锁)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06b-Gap%20Lock%20&%20Next-Key%20Lock%20(间隙锁与临键锁).md) · [Deadlock Detection & avoidance (死锁检测)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06c-Deadlock%20Detection%20&%20avoidance%20(死锁检测).md)

---

## Gap and Next Key Locks (间隙锁与临键锁)

> [!note] 本节重点心考点：间隙锁解决幻读、Next-Key Lock 行锁+间隙锁组合、临键锁对 RR 级别的保障

## 为什么需要 Gap Lock

Gap Lock 解决**幻读**问题——在 RR 级别下防止其他事务插入新行：

> [!warning] 锁范围不是按这页图示机械推导
> 实际 record/gap/next-key 锁取决于隔离级别、语句是否为 locking read、索引是否命中、唯一性、范围边界和 MySQL 版本。图示用于理解区间模型；排障必须查看 `performance_schema.data_locks` 与实际执行计划。

```
场景：用户表，id 为主键 1, 5, 10

事务 A:
  SELECT * FROM user WHERE id > 3 FOR UPDATE;
  → 期望只查到 id=5, id=10
  → 如果不加 Gap Lock，事务 B 可以 INSERT INTO user(id=4)

事务 B:
  INSERT INTO user(id=4, name='Bob');
  → 如果没有 Gap Lock，插入成功
  → 事务 A 再次 SELECT → 出现幻读(id=4)
```

## Gap Lock 的工作原理

Gap Lock 锁的是**索引记录之间的间隙**，而不是记录本身：

```
索引记录：1, 5, 10, 20

间隙：
  (-∞, 1)    记录 1 之前的间隙
  (1, 5)     记录 1 和 5 之间的间隙 ← Gap Lock 锁这个范围
  (5, 10)    记录 5 和 10 之间的间隙
  (10, 20)   记录 10 和 20 之间的间隙
  (20, +∞)   记录 20 之后的间隙

SELECT * FROM user WHERE id > 3 FOR UPDATE;
  → 锁住 (1, 5), (5, 10), (10, 20), (20, +∞)
  → 所有 id > 1 的间隙全部被锁
  → 其他事务无法插入任何 id > 1 的新行
```

**Gap Lock 的特点：**
- 只锁间隙，不锁记录本身
- 不同事务的 Gap Lock 可以共存（间隙锁之间不冲突）
- Gap Lock 只在 RR 和 Serializable 级别生效

## Next-Key Lock（临键锁）

Next-Key Lock = **Record Lock（行锁）+ Gap Lock（间隙锁）**。InnoDB 默认的锁机制。

```
索引：1, 5, 10, 20

Next-Key Lock 锁定的范围是左开右闭区间：
  (-∞, 1]   锁定 ≤1 的范围
  (1, 5]    锁定 (1,5] 的范围 ← 包含行 5 和之前的间隙
  (5, 10]   锁定 (5,10] 的范围
  (10, 20]  锁定 (10,20] 的范围
  (20, +∞)  锁定 >20 的范围（实际是 supremum 伪记录）
```

**举例：**
```sql
-- RR 级别下
SELECT * FROM user WHERE id = 5 FOR UPDATE;

-- Next-Key Lock 锁住的范围：(1, 5]
-- 包含两部分：
--   1. Record Lock：锁住 id=5 的记录（阻止其他事务修改/删除）
--   2. Gap Lock：锁住 (1,5) 间隙（阻止其他事务插入 id=2,3,4）
-- 
-- 不锁 (<1) 的间隙：其他事务可以在 id<1 的范围内插入（如 id=0）
```

## 唯一索引的特殊优化

当 WHERE 条件命中**唯一索引**时，Next-Key Lock 退化为 **Record Lock**：

```sql
-- id 是主键（唯一索引）
SELECT * FROM user WHERE id = 5 FOR UPDATE;

-- InnoDB 知道 id=5 是唯一记录，不需要锁间隙来防止幻读
-- 退化为 Record Lock，只锁 id=5 这一行
-- (1,5) 的间隙不被锁，可以插入 id=3

-- 对比：范围查询不会退化
SELECT * FROM user WHERE id > 5 FOR UPDATE;
-- Next-Key Lock 完整生效，锁住 (5, +∞)
```

## Gap Lock 导致的性能问题

Gap Lock 是 RR 级别下锁争用的常见原因：

```
问题场景：
  事务 A: SELECT * FROM t WHERE id > 10 FOR UPDATE;  -- 锁了大量间隙
  事务 B: INSERT INTO t(id=11) VALUES(...);          -- 被 Gap Lock 阻塞！
  事务 C: INSERT INTO t(id=50) VALUES(...);          -- 也被阻塞！

解决方案：
  1. 缩小 WHERE 范围（如 id BETWEEN 11 AND 20）
  2. 如果业务允许，使用 RC 隔离级别 + ROW 格式 binlog
  3. 在代码层面做唯一性校验而非依赖 Gap Lock
```

## 锁升级路径

```
索引类型     | 查询类型    | 锁类型
唯一索引     | 等值查询    | Record Lock
唯一索引     | 范围查询    | Next-Key Lock
普通索引     | 任何查询    | Next-Key Lock
无索引       | 任何查询    | 全表锁（逐行加 Next-Key Lock）
```

> [!tip]- **工程要点**：范围条件可能锁住比业务直觉更大的索引区间。排查时用 `performance_schema.data_locks`、事务信息和执行计划确认实际锁范围；切换 RC 可能减少部分 gap locking，但并不保证所有场景都没有 gap lock，须结合当前版本与约束验证。

## 30 秒回答 / 自测 · 延伸要点 2
- **30 秒回答**：Next-Key Lock = Record Lock + Gap Lock，左开右闭；普通索引/范围查询用它锁住行与行前间隙防止幻读；命中唯一索引等值查询退化为 Record Lock；Gap Lock 只在 RR/Serializable 生效，间隙锁之间可共存。
- **常见误区**：以为唯一索引查询一定会退化（范围查询不退化）；以为 Gap Lock 锁的是记录本身（实际锁间隙，所以同间隙可共存）；忽略"无索引"导致全表逐行加锁。
- **自测**：1) 为什么唯一索引等值查询能退化为 Record Lock？ 2) 索引 1,5,10 上 `WHERE id=5 FOR UPDATE` 的 Next-Key Lock 锁哪些范围？

---



表锁与行锁基础见 → [Table Lock vs Row Lock (表锁与行锁)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06a-Table%20Lock%20vs%20Row%20Lock%20(表锁与行锁).md) · [Deadlock Detection & avoidance (死锁检测)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06c-Deadlock%20Detection%20&%20avoidance%20(死锁检测).md)

---

## Deadlock Detection and Avoidance (死锁检测与避免)

> [!note] 本节重点心考点：死锁检测机制（等待图）、InnoDB 死锁处理策略（回滚代价较小的事务）、预防死锁方法

## 死锁的产生条件

死锁需要满足四个必要条件（CoFF 条件）：
1. **互斥**：资源一次只能被一个事务占用
2. **持有并等待**：事务持有锁的同时等待其他锁
3. **不可剥夺**：锁只能由持有者释放
4. **循环等待**：事务之间形成等待环

```
MySQL 典型死锁场景：

事务 A:                             事务 B:
UPDATE t SET val=1 WHERE id=1;      UPDATE t SET val=2 WHERE id=2;
UPDATE t SET val=1 WHERE id=2;      UPDATE t SET val=2 WHERE id=1;
                                    ↑
          事务 A 持有 id=1 的锁，等待 id=2
          事务 B 持有 id=2 的锁，等待 id=1
          → 形成循环等待 → 死锁！
```

## InnoDB 死锁检测

InnoDB 通过**等待图（Wait-for Graph）** 检测死锁：

```
等待图：
  节点 = 事务
  边 T1 → T2 = T1 等待 T2 释放锁

  例子：
    T1 → T2
    ↑     ↓
    └─────┘    ← 形成环 → 死锁
```

**检测机制：**
- 每次事务请求锁且需要等待时，InnoDB 检测是否形成环
- 如果在等待图中发现环 → 死锁
- InnoDB 选择**回滚代价最小的事务**（undo log 较少的那个）作为牺牲品

## 死锁信息查看

```sql
-- 查看最近一次死锁信息
SHOW ENGINE INNODB STATUS\G

-- 输出示例（LATEST DETECTED DEADLOCK 部分）：
------------------------
LATEST DETECTED DEADLOCK
------------------------
2024-01-01 12:00:00 0x7f1234
*** (1) TRANSACTION:
TRANSACTION 12345, ACTIVE 10 sec
mysql tables in use 1, locked 1
LOCK WAIT 2 lock struct(s)
TABLE: `db`.`t`, index: PRIMARY
*** (1) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 10 page no 3 n bits 72
*** (2) TRANSACTION:
TRANSACTION 12346, ACTIVE 5 sec
*** (2) HOLDS THE LOCK(S):
RECORD LOCKS space id 10 page no 3 n bits 72
*** (2) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 10 page no 3 n bits 72
*** WE ROLL BACK TRANSACTION (2)  ← InnoDB 选择了事务 2 回滚
```

## 死锁后的事务处理

```sql
-- 应用层死锁重试
int retry_count = 3;
while (retry_count > 0) {
    try {
        // 执行事务
        conn->begin();
        conn->execute("UPDATE t SET val=1 WHERE id=1");
        conn->execute("UPDATE t SET val=2 WHERE id=2");
        conn->commit();
        break;  // 成功
    } catch (DeadlockException &e) {
        retry_count--;
        if (retry_count == 0) throw;
        // 回滚后重试
        conn->rollback();
        usleep(100 * (rand() % 10));  // 随机延迟，减少再次死锁概率
    }
}
```

## 死锁预防策略

**1. 统一加锁顺序**
```sql
-- 不好的做法（A 先锁 id=1，B 先锁 id=2）
事务 A: UPDATE t SET val=1 WHERE id=1; UPDATE t SET val=1 WHERE id=2;
事务 B: UPDATE t SET val=2 WHERE id=2; UPDATE t SET val=2 WHERE id=1;

-- 好的做法（所有事务按 id 升序加锁）
事务 A: UPDATE t SET val=1 WHERE id=1; UPDATE t SET val=2 WHERE id=2;
事务 B: UPDATE t SET val=2 WHERE id=1; UPDATE t SET val=2 WHERE id=2;
-- 先锁小 id，再锁大 id → 不会形成循环等待
```

**2. 减少锁持有时间**
```sql
-- 不好的做法：事务中做慢查询
BEGIN;
SELECT * FROM t WHERE ... FOR UPDATE;  -- 加锁
-- ... 复杂的业务计算，耗时 1 秒 ...
UPDATE t SET val=1 WHERE id=1;         -- 锁被长时间持有
COMMIT;

-- 好的做法：先计算再加锁
-- ... 业务计算 ...
BEGIN;
UPDATE t SET val=1 WHERE id=1;         -- 加锁后立即提交
COMMIT;
```

**3. 按语义评估隔离级别**
- RC 在许多普通范围操作中会减少 gap locking，但外键、重复键检查等场景仍可能涉及间隙锁
- 是否使用 RC、以及复制格式，取决于一致性语义、当前版本和实际死锁模式

**4. 合理设计索引**
- 确保 UPDATE/DELETE 的 WHERE 条件有合适索引，否则会扫描并锁住大量记录，扩大冲突范围
- InnoDB 不会因为“未走索引”自动把行锁升级成传统表锁；表锁同样可能参与死锁

## 死锁监控

```sql
-- 开启死锁日志
SET GLOBAL innodb_print_all_deadlocks = ON;
-- 死锁信息写入 MySQL 错误日志

-- 监控锁等待超时
SHOW VARIABLES LIKE 'innodb_lock_wait_timeout';
-- 默认 50 秒，超过自动回滚

-- 查看当前锁等待
SELECT * FROM sys.innodb_lock_waits\G
```

> [!tip]- **工程要点**：死锁是并发写入中需要设计处理的正常失败路径，但重试次数、退避策略和是否可安全重试必须由业务幂等性决定。统一锁顺序、缩短事务、优化索引能降低概率；检测代价和响应时间取决于负载与锁图，不能承诺“毫秒级”。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Transactions MVCC and Locks (事务 MVCC 与锁)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
