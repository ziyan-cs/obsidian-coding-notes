> **核心考点**：索引条件下推 ICP 减少回表、覆盖索引避免回表、索引合并优化

## 索引条件下推（ICP）

在没有 ICP 之前，MySQL 通过二级索引找到记录后，必须回表检查 WHERE 条件中的非索引列。

```
无 ICP（Index Condition Pushdown 关闭）：
  name 索引(name, age) → 找到 name='Zhang' 的记录 N 条 → 回表 N 次 → 过滤 age > 30

有 ICP（MySQL 5.6+ 默认开启）：
  name 索引(name, age) → 找到 name='Zhang' → **在索引层面检查 age > 30**
                        → 只对符合条件的记录回表 → 减少回表次数
```

**ICP 的条件：**

```sql
-- ICP 生效的场景
CREATE INDEX idx_name_age ON user(name, age);

SELECT * FROM user WHERE name LIKE 'Z%' AND age > 30;
-- name 使用索引范围，age 在索引中但无法直接用于索引查找
-- ICP 在遍历索引时检查 age > 30，减少回表

-- ICP 不生效的场景
SELECT * FROM user WHERE name LIKE 'Z%' AND address = 'CN';
-- address 不在索引中，ICP 无法过滤
```

**EXPLAIN 中的表现：**
```
Extra: Using index condition    ← 表示 ICP 生效
```

## 覆盖索引

当二级索引包含查询所需的所有列时，MySQL 可以直接从索引获取数据，完全避免回表：

```sql
-- 覆盖索引生效：idx_name_age 包含 name 和 age
SELECT name, age FROM user WHERE name = 'Bob';
-- EXPLAIN Extra: Using index

-- 需要回表：idx_name_age 不包含 address
SELECT name, age, address FROM user WHERE name = 'Bob';
-- EXPLAIN Extra: (无 Using index)
```

**设计覆盖索引的原则：**
- 针对高频查询，将查询涉及的列都包含在索引中
- 不要过度覆盖（索引维护有成本），只覆盖关键查询
- 先考虑查询条件列（WHERE），再考虑 SELECT 列

## 索引合并（Index Merge）

MySQL 可以在一个查询中使用多个索引，将结果合并。

```sql
CREATE INDEX idx_age ON user(age);
CREATE INDEX idx_name ON user(name);

-- 使用两个索引，取交集
SELECT * FROM user WHERE name = 'Bob' AND age = 25;
-- 可能使用 index_merge: idx_name∩idx_age

-- 使用两个索引，取并集
SELECT * FROM user WHERE name = 'Bob' OR name = 'Alice';
```

**索引合并的三种方式：**
| 方式 | EXPLAIN Extra 信息 | 适用条件 |
|------|-------------------|---------|
| 交集（Intersection） | Using intersect(idx1,idx2) | AND 条件，主键交集 |
| 并集（Union） | Using union(idx1,idx2) | OR 条件，主键并集 |
| 排序并集（Sort-Union） | Using sort_union(idx1,idx2) | OR 条件+范围查询 |

**索引合并 vs 联合索引：**
```
索引合并：
  idx_age + idx_name 独立 → 分别查找 → 合并结果
  需要 2 次 B+ 树遍历 + 内存合并

联合索引：
  idx_name_age 一次性查找
  1 次 B+ 树遍历

结论：联合索引通常优于索引合并（少一次遍历，无合并开销）
```

## 索引失效场景

```sql
-- 1. 索引列使用函数
WHERE YEAR(create_time) = 2024     → 失效
WHERE create_time >= '2024-01-01' AND create_time < '2025-01-01'   → 有效

-- 2. 隐式类型转换
WHERE phone = 13800138000          → phone 是 VARCHAR，隐式转换导致失效

-- 3. 前导模糊查询
WHERE name LIKE '%zhang'           → 失效
WHERE name LIKE 'zhang%'           → 有效

-- 4. 联合索引不满足最左前缀
-- 索引 (a, b, c)
WHERE b = 1 AND c = 2              → 失效（跳过 a）
WHERE a = 1 AND c = 2              → 只用到 a（中间跳过 b）
```

> **工程要点**：ICP 是 MySQL 5.6 的里程碑优化——它将 WHERE 过滤下推到存储引擎层，减少回表次数。但 ICP 并不能完全替代覆盖索引，最彻底的优化仍然是"索引列覆盖查询的所有 SELECT 列"。实践中，先用覆盖索引满足高频查询，再通过 ICP 减少低频查询的回表代价。
