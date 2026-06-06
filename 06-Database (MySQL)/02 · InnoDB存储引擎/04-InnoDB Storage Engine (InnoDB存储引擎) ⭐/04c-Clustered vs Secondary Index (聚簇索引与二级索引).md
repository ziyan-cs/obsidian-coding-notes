---
tags:
  - database
  - innodb
---

> **核心考点**：聚簇索引（主键索引即数据）与二级索引的结构差异、回表查询与覆盖索引

## 聚簇索引（Clustered Index）

InnoDB 的聚簇索引将索引与数据存储在一起——叶节点直接包含整行数据。

```
聚簇索引结构（以主键为索引）：

  根节点       [PK: 1, PK: 50, PK: 100]
                   /       |        \
  内部节点  [1-20]   [50-80]   [100-150]
               |         |           |
  叶节点（数据） ┌─────┐  ┌─────┐   ┌─────┐
               │PK=1 │  │PK=50│   │PK=100│
               │name=│  │name=│   │name= │
               │...  │  │...  │   │...   │
               └─────┘  └─────┘   └─────┘
                ↑ 叶节点直接存储整行数据

特点：
  - 每个表只有一个聚簇索引（数据只能按一种方式物理排序）
  - 如果没有定义主键，InnoDB 自动选择唯一索引或生成隐藏 ROW_ID
  - 叶节点 = 数据页，数据按主键顺序排列
```

## 二级索引（Secondary Index）

二级索引的叶节点存储**主键值**而非整行数据。

```
二级索引（以 name 列为例）：

  根节点       [name: A, name: M, name: S]
                   /        |           \
  内部节点  [A-L]    [M-R]      [S-Z]
               |          |           |
  叶节点  ┌───────┐  ┌───────┐   ┌───────┐
          │name = │  │name = │   │name = │
          │"Bob"  │  │"Mike" │   │"Tom"  │
          │PK = 32│  │PK = 5 │   │PK = 18│  ← 存储主键值
          └───────┘  └───────┘   └───────┘
               ↓          ↓          ↓
          ┌──────────────┴──────────────┐
          │ 回表查询：根据 PK 到聚簇索引查找完整数据 │
          └─────────────────────────────┘
```

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

> **工程要点**：回表是二级索引查询的主要成本。优化思路：1) 利用覆盖索引避免回表；2) 尽量用自增主键保证聚簇索引紧凑（非随机写入）。EXPLAIN 看到 `Using index` 是好事，看到 `Using index condition` 表示用了索引下推但仍然需要回表。

---

## 关联笔记

- [Page Structure & Buffer Pool (页结构与缓冲池)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04a-Page%20Structure%20&%20Buffer%20Pool%20(页结构与缓冲池).md)
- [B+ Tree Index Structure (B+树索引结构)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04b-B+%20Tree%20Index%20Structure%20(B+树索引结构).md)
- [Index Pushdown & Covering Index (索引下推与覆盖索引)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04d-Index%20Pushdown%20&%20Covering%20Index%20(索引下推与覆盖索引).md)
- [DDL, DML, DQL (SQL基础语法)](/06-Database%20(MySQL)/01%20·%20SQL基础/01-DDL,%20DML,%20DQL%20(SQL基础语法).md)
- [Joins & Subqueries (多表查询与子查询)](/06-Database%20(MySQL)/01%20·%20SQL基础/02-Joins%20&%20Subqueries%20(多表查询与子查询).md)
