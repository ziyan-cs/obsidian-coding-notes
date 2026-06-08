---
tags:
  - database
  - optimization
---

> **核心考点**：慢查询日志配置与分析、索引失效常见场景、SQL 重写优化技巧

## 慢查询日志配置

```ini
# my.cnf 配置
slow_query_log = ON
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 1            # 超过 1 秒的 SQL 记录（生产建议 0.5-1 秒）
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
# Time: 2024-01-01T12:00:00.123456Z
# User@Host: root[root] @ localhost [127.0.0.1]
# Query_time: 2.345678  Lock_time: 0.001234 Rows_sent: 100  Rows_examined: 1000000
# Thread_id: 12345  Schema: test  Last_errno: 0
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
# mysqldumpslow：汇总慢查询日志
mysqldumpslow -s t -t 10 /var/log/mysql/slow.log
# -s t：按查询时间排序
# -t 10：取前 10 条

# pt-query-digest（Percona Toolkit）：更高级的分析
pt-query-digest /var/log/mysql/slow.log
# 输出：查询指纹（规范化后的 SQL）、
#       执行次数、总耗时、平均耗时、
#       响应时间分布
```

> **工程要点**：慢查询处理的黄金流程——1) `long_query_time=0.5` 开启慢查询日志；2) `pt-query-digest` 分析并找到 TOP N 慢 SQL；3) `EXPLAIN` 分析执行计划；4) 根据索引失效原因优化索引或重写 SQL；5) 验证优化效果。**不要试图一次优化所有慢查询——投入产出比最高的永远是执行频率高、单次耗时长的查询。**

---



执行计划分析见 → [EXPLAIN & Execution Plan Analysis (执行计划分析)](/06-Database%20(MySQL)/03%20·%20查询优化/08-Query%20Optimization%20(查询优化)%20⭐/08a-EXPLAIN%20&%20Execution%20Plan%20Analysis%20(执行计划分析).md) · [Index Design Principles (索引设计原则)](/06-Database%20(MySQL)/03%20·%20查询优化/08-Query%20Optimization%20(查询优化)%20⭐/08b-Index%20Design%20Principles%20(索引设计原则).md)
