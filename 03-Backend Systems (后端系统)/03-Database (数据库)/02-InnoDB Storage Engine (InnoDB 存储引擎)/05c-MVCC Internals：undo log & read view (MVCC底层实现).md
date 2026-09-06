---
tags:
  - database/innodb
status: seed
review_due: 2026-10-03
confidence: 1
verified: stable
---

# MVCC Internals：undo log & read view — MVCC底层实现

> [!important] **核心考点**
> MVCC 通过 undo log 实现一致性读、Read View 可见性判断、快照读与当前读

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
