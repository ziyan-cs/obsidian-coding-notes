---
tags:
  - database/optimization
status: seed
review_due: 2026-10-03
confidence: 1
verified: NEEDS_VERIFY
---

> [!important] **核心考点**：EXPLAIN 输出解读（type/key/rows/Extra）、全表扫描 vs 索引扫描、慢 SQL 定位

## EXPLAIN 输出详解

```sql
EXPLAIN SELECT * FROM user WHERE age > 20\G
```

**核心字段速查表：**

| 字段 | 含义 | 关键值（好→差） |
|------|------|----------------|
| type | 访问类型 | system > const > eq_ref > ref > range > index > ALL |
| key | 使用的索引 | 实际使用的索引名，NULL = 未使用索引 |
| rows | 扫描行数估计 | 越小越好，与实际行数差距大说明统计信息不准 |
| Extra | 额外信息 | Using index（好）、Using filesort（坏）、Using temporary（坏）|

## type 访问类型详解

```
从最优到最差：

system       表只有一行（系统表），const 的特例
const        主键/唯一索引等值查询，最多返回一行
eq_ref       联表查询时，被驱动表通过主键/唯一索引访问
ref          普通索引等值查询
ref_or_null  普通索引等值查询 + NULL 值
range        索引范围查询（>, <, BETWEEN, IN）
index        索引全扫描（遍历整个索引树）
ALL          全表扫描（最差）
```

```sql
-- const（主键等值）
EXPLAIN SELECT * FROM user WHERE id = 1;
-- type: const, key: PRIMARY

-- ref（普通索引等值）
EXPLAIN SELECT * FROM user WHERE name = 'Bob';
-- type: ref, key: idx_name

-- range（索引范围）
EXPLAIN SELECT * FROM user WHERE age > 20;
-- type: range, key: idx_age

-- ALL（全表扫描）
EXPLAIN SELECT * FROM user WHERE address = 'CN';
-- type: ALL, key: NULL（无索引）
```

## Extra 信息解读

**好兆头：**

| Extra | 含义 |
|-------|------|
| Using index | 覆盖索引，无需回表 |
| Using index condition | 索引条件下推（ICP） |
| Using where | 在存储引擎层过滤 |

**坏兆头：**

| Extra | 含义 | 优化方向 |
|-------|------|---------|
| Using filesort | 文件排序（非索引排序） | 添加排序字段到索引 |
| Using temporary | 使用临时表（GROUP BY 无索引） | 添加 GROUP BY 字段到索引 |
| Using join buffer | 联表未使用索引 | 为联表字段添加索引 |

## 常见慢 SQL 分析与优化

**场景 1：未使用索引**
```sql
-- 慢（type: ALL，全表扫描）
EXPLAIN SELECT * FROM order WHERE status = 1;

-- 优化：添加索引
CREATE INDEX idx_status ON `order`(status);
```

**场景 2：文件排序**
```sql
-- 慢（Extra: Using filesort）
EXPLAIN SELECT * FROM user WHERE age > 20 ORDER BY create_time;

-- 优化：建立复合索引（过滤 + 排序一起满足）
CREATE INDEX idx_age_create ON user(age, create_time);
```

**场景 3：大表 LIMIT 分页**
```sql
-- 越往后越慢（OFFSET 需要扫描并丢弃前 N 行）
SELECT * FROM user ORDER BY id LIMIT 100000, 20;

-- 优化：基于游标的分页（利用主键索引直接定位）
SELECT * FROM user WHERE id > 100000 ORDER BY id LIMIT 20;

-- 或延迟关联（先走索引找到 id，再回表取数据）
SELECT * FROM user
INNER JOIN (SELECT id FROM user ORDER BY id LIMIT 100000, 20) AS tmp
USING(id);
```

**场景 4：LIKE 模糊查询**
```sql
-- 无法使用索引
EXPLAIN SELECT * FROM user WHERE name LIKE '%zhang%';

-- 尾部模糊可以用索引
EXPLAIN SELECT * FROM user WHERE name LIKE 'zhang%';

-- 优化：全文索引
ALTER TABLE user ADD FULLTEXT INDEX ft_name(name);
SELECT * FROM user WHERE MATCH(name) AGAINST('zhang');
```

**场景 5：OR 条件**
```sql
-- OR 可能不走索引
SELECT * FROM user WHERE name = 'Bob' OR age = 20;

-- 优化：改为 UNION ALL
SELECT * FROM user WHERE name = 'Bob'
UNION ALL
SELECT * FROM user WHERE age = 20;

-- 或使用索引合并（需要 MySQL 的 index merge 优化）
```

## 实战 EXPLAIN 检查清单

```sql
-- 1. type 是否为 ALL 或 index（全表/全索引扫描）？
-- 2. key 是否为 NULL（无可用索引）？
-- 3. rows 是否远大于预期？
-- 4. Extra 是否有 Using filesort / Using temporary？
-- 5. 联表时被驱动表的 type 是否为 eq_ref / ref？
```

> [!tip]- **工程要点**：EXPLAIN 是 SQL 优化的第一工具，但 rows 是估计值不一定精确，可用 `ANALYZE TABLE` 更新统计信息。MySQL 8.0 的 `EXPLAIN ANALYZE` 可以给出实际执行时间：`EXPLAIN ANALYZE SELECT * FROM user WHERE age > 20`——比传统 EXPLAIN 更准确。

---



索引设计原则见 → [Index Design Principles (索引设计原则)](/06-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08b-Index%20Design%20Principles%20(索引设计原则).md) · [Slow Query Log & Optimization (慢查询分析)](/06-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08c-Slow%20Query%20Log%20&%20Optimization%20(慢查询分析).md)
