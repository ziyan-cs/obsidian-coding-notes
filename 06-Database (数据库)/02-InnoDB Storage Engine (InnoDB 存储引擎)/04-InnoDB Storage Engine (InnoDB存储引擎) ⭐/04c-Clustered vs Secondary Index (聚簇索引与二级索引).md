---
tags:
  - database/innodb
status: seed
review_due: 2026-10-03
confidence: 1
verified: stable
---

> [!important] **核心考点**：聚簇索引（主键索引即数据）与二级索引的结构差异、回表查询与覆盖索引

## 聚簇索引（Clustered Index）

InnoDB 的聚簇索引将索引与数据存储在一起——叶节点直接包含整行数据。

```text
Clustered Index (Primary Key)        Secondary Index (Non-Primary Key)
┌────────────────────────────┐      ┌─────────────────────────────────┐
│ Root Node (Buffer Pool)    │      │ Root Node                       │
└──────────┬─────────────────┘      └──────────┬──────────────────────┘
           │                                   │
           ▼                                   ▼
┌────────────────────────────┐      ┌─────────────────────────────────┐
│ Leaf Node                  │      │ Leaf Node                       │
│ (full row data)            │      │ (key + primary key value)       │
└──────┬─────────────────────┘      └──────────┬──────────────────────┘
       │                                       │
       │◄───── Table Lookup  ──────────────────┘
       │          (回表)
       ▼
┌────────────────────────────┐
│ Query Result               │
│ (Covering Index:           │
│  skips table lookup)       │
└────────────────────────────┘

Note: Secondary index leaf nodes store index columns + primary key value,
      NOT the full row data!
```

## 二级索引（Secondary Index）

二级索引的叶节点存储**主键值**而非整行数据。


## 回表查询

通过二级索引查询需要**两次 B+ 树遍历**：

```
SELECT * FROM t WHERE name = 'Bob';

Step 1: name 二级索引
  查找 name = 'Bob' → 得到 PK = 32

Step 2: 聚簇索引回表
  查找 PK = 32 → 得到完整行数据

代价：两次 B+ 树查找 = 2 × (树高) 次 IO
```

**回表 vs 不回表：**
```sql
-- 需要回表（二级索引不包含所需列）
SELECT * FROM t WHERE name = 'Bob';

-- 不需要回表（索引覆盖）
SELECT name, age FROM t WHERE name = 'Bob' AND age = 25;
-- 如果联合索引 (name, age) 已包含所有列，无需回表
```

## 覆盖索引

如果二级索引的叶节点包含查询所需的所有列，则无需回表：

```sql
CREATE INDEX idx_name_age ON t(name, age);

-- 查询 name 和 age，idx_name_age 已包含 → 覆盖索引
SELECT name, age FROM t WHERE name = 'Bob';

-- 查询 *，idx_name_age 不包含 address → 需要回表
SELECT * FROM t WHERE name = 'Bob';
```

**覆盖索引的工程意义：**
- 减少 IO（少一次 B+ 树遍历）
- 某些查询可以完全由索引满足（Index-Only Scan）
- EXPLAIN 中 Extra 字段显示 `Using index` 表示使用覆盖索引

## 主键选择对聚簇索引的影响

```
自增 INT 主键：
  插入总是在 B+ 树最右叶节点 → 页分裂少，顺序 IO

UUID/VARCHAR 主键：
  插入位置随机 → 页分裂频繁，页碎片多，性能差

区别在百万行级别 -> 3 倍以上写入性能差距
```

> [!tip]- **工程要点**：回表是二级索引查询的主要成本。优化思路：1) 利用覆盖索引避免回表；2) 尽量用自增主键保证聚簇索引紧凑（非随机写入）。EXPLAIN 看到 `Using index` 是好事，看到 `Using index condition` 表示用了索引下推但仍然需要回表。

---



B+树结构详解见 → [B+ Tree Index Structure (B+树索引结构)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04b-B+%20Tree%20Index%20Structure%20(B+树索引结构).md) · [Index Pushdown & Covering Index (索引下推与覆盖索引)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04d-Index%20Pushdown%20&%20Covering%20Index%20(索引下推与覆盖索引).md)
