---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 05-Query Analysis and Optimization (查询分析与优化)

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

## EXPLAIN and Execution Plans (执行计划分析)

> [!note] 本节重点心考点：EXPLAIN 输出解读（type/key/rows/Extra）、全表扫描 vs 索引扫描、慢 SQL 定位

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

---

## Index Design Principles (索引设计原则)

> [!note] 本节重点心考点：索引选择性与区分度、联合索引最左前缀、索引覆盖与回表、索引下推优化

## 基础概念

- 索引是数据库中**加速查询**的一种数据结构，相当于表的「目录」
- 查询时间复杂度从 `O(n)` 降低到 `O(log n)`
- 通过预排序，减少查询时的全表扫描次数
- 占用额外存储空间；增删改操作需要维护索引，性能会有轻微损耗

```sql
-- 查看表中所有索引
SHOW INDEX FROM [表名];
```

---

## 索引类型

- 主键索引（Primary Key Index）
	
	- 基于主键自动创建，一张表只能有一个 
	- 天然具备 `NOT NULL + UNIQUE` 约束 
	- InnoDB 中为聚簇索引，决定数据物理存储顺序

```sql
-- 创建表时定义主键索引（自动创建聚簇索引）
CREATE TABLE [表名] (
    id INT PRIMARY KEY AUTO_INCREMENT
);

-- 表创建完成后添加主键索引
ALTER TABLE [表名] ADD   ;
```

- 唯一索引（Unique Index）
	
	- 字段值不可重复（允许一个 NULL）
	- 一张表可建多个，非聚簇索引

```sql
-- 创建表时定义唯一索引
CREATE TABLE [表名] (
    phone VARCHAR(11) UNIQUE
);

-- 表创建完成后添加唯一索引
ALTER TABLE [表名] ADD UNIQUE INDEX idx_phone(phone);
```

- 普通索引（Normal Index）
	
	- 基于主键自动创建，一张表只能有一个 
	- 天然具备 `NOT NULL + UNIQUE` 约束 
	- InnoDB 中为聚簇索引，决定数据物理存储顺序

```sql
-- 创建表时定义普通索引
CREATE TABLE [表名] (
    name VARCHAR(50),
    INDEX idx_name(name)
);

-- 表创建完成后添加普通索引
ALTER TABLE [表名] ADD INDEX idx_name(name);
```

- 复合索引（联合索引）
	
	- 由多个字段组合而成的索引，遵循**最左匹配原则**
	- 适用于多条件查询场景，可覆盖多个查询条件

```sql
-- 创建复合索引
CREATE TABLE [表名] (
    user_id INT,
    goods_id INT,
    INDEX idx_user_goods(user_id, goods_id)
);

-- 表创建完成后添加复合索引
ALTER TABLE [表名] ADD INDEX idx_user_goods(user_id, goods_id);
```

- 全文索引（FULLTEXT）
	
	- 用于文本字段的模糊查询（`MATCH AGAINST`）
	- 支持中文分词，比 `LIKE '%xxx%'` 效率高很多

```sql
-- 创建全文索引（MySQL 5.6+ InnoDB 支持）
CREATE TABLE [表名] (
    content TEXT,
    FULLTEXT INDEX idx_content(content)
);

-- 全文索引查询
SELECT * FROM [表名] WHERE MATCH(content) AGAINST('关键词');
```

---

## InnoDB 索引底层原理（B + 树）

- 索引的底层数据结构为 **B + 树**，所有数据都存储在叶子节点
- 聚簇索引：叶子节点直接存储完整的行数据，数据按主键顺序物理存储
- 非聚簇索引：叶子节点存储主键值，查询时需通过主键回表获取完整数据
- 特点：非叶子节点仅存储索引值，不存储数据，树的高度低，查询效率稳定

---

## 索引设计原则（面试高频）

- 优先为 **WHERE、JOIN、ORDER BY、GROUP BY** 条件中的字段创建索引
- 复合索引遵循**最左匹配原则**，将区分度高的字段放在前面
- 避免在低基数列（如性别、状态）上创建索引，优化效果几乎为 0
- 索引字段类型尽量小，优先使用 `INT` 而非 `VARCHAR`
- 避免创建过多索引，增删改操作的维护成本会显著增加

---

## 索引失效场景（必背）

- 使用 `OR` 连接条件，且非所有条件都包含索引字段
- 索引列参与运算、使用函数（如 `DATE(create_time)`）
- 隐式类型转换（如 `VARCHAR` 字段和数字比较）
- 使用 `LIKE '%xxx'` 或 `LIKE '%xxx%'`，无法利用索引
- 范围查询（`>、<、BETWEEN`）会导致复合索引后续字段失效

---

## 索引相关 SQL 操作

```sql
-- 1. 创建索引（表已存在）
ALTER TABLE [表名] ADD INDEX idx_xxx([字段名]);
ALTER TABLE [表名] ADD UNIQUE INDEX idx_xxx([字段名]);
ALTER TABLE [表名] ADD PRIMARY KEY (id);

-- 2. 删除索引
ALTER TABLE [表名] DROP INDEX idx_xxx;
ALTER TABLE [表名] DROP PRIMARY KEY; -- 删除主键索引

-- 3. 查看索引使用情况（分析查询语句）
EXPLAIN SELECT * FROM [表名] WHERE [条件];
```

---

## 索引优化实践

```sql
-- 查看查询执行计划，判断索引是否生效
EXPLAIN SELECT * FROM `purchase` WHERE customer_id = 1 AND goods_id = 100;

-- 为高频查询条件创建复合索引
ALTER TABLE `purchase` ADD INDEX idx_customer_goods(customer_id, goods_id);
```

---



表设计与 SQL 基础见 → [MySQL Basics (MySQL 基础)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/02-MySQL%20Basics%20(MySQL%20基础).md) · [EXPLAIN & Execution Plan Analysis (执行计划分析)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08a-EXPLAIN%20&%20Execution%20Plan%20Analysis%20(执行计划分析).md)

---

## Slow Query Optimization (慢查询优化)

> [!note] 本节重点心考点：慢查询日志配置与分析、索引失效常见场景、SQL 重写优化技巧

## 慢查询日志配置

```ini
slow_query_log = ON
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 1            # 示例阈值；按 SLO、负载和日志成本设定
log_queries_not_using_indexes = ON  # 未使用索引的查询也记录
log_slow_admin_statements = ON  # 记录 ALTER TABLE 等慢管理语句
min_examined_row_limit = 100    # 只记录扫描行数 > 100 的查询
```

```sql
-- 运行时动态配置
SET GLOBAL slow_query_log = ON;
SET GLOBAL long_query_time = 0.5;   -- 500ms
SET GLOBAL log_queries_not_using_indexes = ON;
```

## 慢查询日志格式

```
SET timestamp=1704105600;
SELECT * FROM user WHERE YEAR(create_time) = 2024;
                              ↑
                          Query_time: 2.34s ← 查询耗时
                          Rows_examined: 100 万 ← 扫描行数
                          关键指标：扫描行数 / 返回行数 ≈ 10000 → 明显有问题
```

## 索引失效 10 大场景

```sql
-- 1. 函数操作索引列
WHERE YEAR(create_time) = 2024
-- 改成：WHERE create_time >= '2024-01-01' AND create_time < '2025-01-01'

-- 2. 隐式类型转换
WHERE phone = 13800138000  -- phone 是 VARCHAR
-- 改成：WHERE phone = '13800138000'

-- 3. 隐式字符集转换
-- 联表时两个表的 charset 不同（如 utf8mb4 vs latin1）

-- 4. 前导模糊查询
WHERE name LIKE '%zhang'
-- 改成：WHERE name LIKE 'zhang%'（能用索引）

-- 5. 联合索引不满足最左前缀
-- 索引 (a, b, c)
WHERE b = 1           -- 失效
WHERE a = 1 AND c = 2 -- 只用到 a

-- 6. OR 条件
WHERE name = 'Bob' OR age = 20
-- 改成：UNION ALL
SELECT * FROM user WHERE name = 'Bob'
UNION ALL SELECT * FROM user WHERE age = 20

-- 7. IN 条件值过多
WHERE id IN (1,2,3,...,100000)
-- MySQL 优化器可能认为全表扫描比索引查找更快

-- 8. 不等于/NOT IN
WHERE status != 'active'
-- 通常走全表扫描，因为不等于范围太大

-- 9. IS NULL / IS NOT NULL
WHERE name IS NOT NULL
-- 大多数数据不为 NULL 时，MySQL 认为全表扫描更快

-- 10. 数据分布不均
WHERE status = 'inactive'  -- 如果 99% 数据是 inactive
-- MySQL 优化器认为全表扫描更快（即使 status 有索引）
```

## SQL 重写优化技巧

**1. 分页优化**
```sql
-- 差（OFFSET 越大越慢）
SELECT * FROM user ORDER BY id LIMIT 100000, 20;

-- 好（游标分页）
SELECT * FROM user WHERE id > 100000 ORDER BY id LIMIT 20;

-- 好（延迟关联）
SELECT * FROM user INNER JOIN (
    SELECT id FROM user ORDER BY id LIMIT 100000, 20
) AS tmp USING(id);
```

**2. COUNT 优化**
```sql
-- 差（COUNT DISTINCT 需要去重计算）
SELECT COUNT(DISTINCT name) FROM user;

-- 差（COUNT 大字段）
SELECT COUNT(*) FROM user WHERE content LIKE '%keyword%';

-- 好（使用近似值）
SHOW TABLE STATUS LIKE 'user';  -- rows 字段是近似值

-- 好（使用汇总表）
-- 维护一张计数器表，INSERT/DELETE 时更新
```

**3. 子查询优化**
```sql
-- 差（子查询在 MySQL 中可能被重写为相关子查询）
SELECT * FROM user WHERE id IN (
    SELECT user_id FROM `order` WHERE amount > 100
);

-- 好（使用 JOIN）
SELECT DISTINCT u.* FROM user u
INNER JOIN `order` o ON u.id = o.user_id
WHERE o.amount > 100;
```

**4. 批量操作优化**
```sql
-- 差（逐条 UPDATE，每条都提交一个事务）
UPDATE user SET balance=0 WHERE id=1;
UPDATE user SET balance=0 WHERE id=2;
UPDATE user SET balance=0 WHERE id=3;

-- 好（批量 UPDATE）
UPDATE user SET balance=0 WHERE id IN (1, 2, 3);

-- 批量 DELETE 分批处理（避免锁大量行）
DELETE FROM user WHERE create_time < '2020-01-01' LIMIT 1000;
-- 循环执行直到影响行数为 0
```

## 慢查询分析工具

```bash
mysqldumpslow -s t -t 10 /var/log/mysql/slow.log

pt-query-digest /var/log/mysql/slow.log
```

> [!tip]- **工程要点**：慢查询阈值、是否记录未用索引查询、采样与保留周期都要考虑日志量和 SLO。先按总耗时、调用频率、P95/P99、扫描/返回行比筛选，再用执行计划与真实参数验证。不要只为消除 `filesort` 或改变 SQL 形状而盲目加索引。



## 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

## 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **05-Query Analysis and Optimization (查询分析与优化)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
