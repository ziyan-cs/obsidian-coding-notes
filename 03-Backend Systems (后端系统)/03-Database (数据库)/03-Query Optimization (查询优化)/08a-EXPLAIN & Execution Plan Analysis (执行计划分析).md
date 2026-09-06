---
tags:
  - database/optimization
status: seed
review_due: 2026-10-03
confidence: 1
verified: NEEDS_VERIFY
---

# EXPLAIN & Execution Plan Analysis — 执行计划分析

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
| Extra | 额外信息 | 结合语句与数据量解释；`Using index`、`filesort`、temporary 都不是脱离上下文的好坏标签 |

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

**需要理解的信号：**

| Extra | 含义 |
|-------|------|
| Using index | 覆盖索引，无需回表 |
| Using index condition | 索引条件下推（ICP） |
| Using where | MySQL Server 层仍需按条件过滤一部分行；不必然是问题 |

**坏兆头：**

| Extra | 含义 | 优化方向 |
|-------|------|---------|
| Using filesort | 额外排序步骤，不等于一定落盘 | 结合行数、`ORDER BY`、LIMIT 与耗时判断 |
| Using temporary | 中间结果需要临时表 | 检查聚合/排序/数据量，而非机械加索引 |
| Using join buffer | join 未能直接使用理想索引访问 | 检查连接条件、驱动表与索引选择 |

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

-- 注意：age 是范围条件时，(age, create_time) 通常不能同时消除 create_time 排序。
-- 先用 EXPLAIN ANALYZE 比较候选索引与实际代价，再按业务的筛选/排序模式设计。
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

-- 仅当两个分支语义不重叠，或你能接受/显式处理重复行时才考虑 UNION ALL
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

> [!tip]- **工程要点**：EXPLAIN 是提出假设的起点，`rows` 是估计而非测量。可在合适环境用 `EXPLAIN ANALYZE`、慢日志与真实参数验证，再决定是否改索引或 SQL；避免为了消除某个 Extra 字段而制造更差的写入代价。

## 30 秒回答

读 EXPLAIN 先看访问路径、估算扫描行数、连接顺序与过滤/排序是否符合预期，而不是背 `type` 排名或见到 `filesort` 就建索引。索引必须同时服务真实的 WHERE、JOIN、ORDER BY 和写入成本；用实际执行数据验证优化是否减少端到端延迟。

---



索引设计原则见 → [Index Design Principles (索引设计原则)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08b-Index%20Design%20Principles%20(索引设计原则).md) · [Slow Query Log & Optimization (慢查询分析)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08c-Slow%20Query%20Log%20&%20Optimization%20(慢查询分析).md)
