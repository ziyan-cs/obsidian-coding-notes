---
tags:
  - database
  - sql
---

> **核心考点**：INNER/LEFT/RIGHT/FULL JOIN 区别与用法、子查询 EXISTS/IN、多表关联优化要点

## 联合查询

```sql
-- UNION：合并结果集并去重
SELECT [字段列表] FROM [表1] UNION SELECT [字段列表] FROM [表2];

-- UNION ALL：合并结果集不去重（推荐：性能更好）
SELECT [字段列表] FROM [表1] UNION ALL SELECT [字段列表] FROM [表2];
```

## 连接查询

```sql
-- 自连接：一张表自己和自己连接（取别名）
-- 场景：员工表查出每个员工的上级姓名
SELECT e1.name AS employee, e2.name AS manager
FROM emp e1 LEFT JOIN emp e2 ON e1.manager_id = e2.id;

-- 内连接：取两表交集数据
SELECT t1.字段, t2.字段 FROM 表1 t1
INNER JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 左外连接：左表全量 + 右表匹配项（无匹配为 NULL）
SELECT t1.字段, t2.字段 FROM 表1 t1
LEFT JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 右外连接：右表全量 + 左表匹配项（无匹配为 NULL）
SELECT t1.字段, t2.字段 FROM 表1 t1
RIGHT JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 等值连接简化写法（两表关联字段名相同时）
SELECT * FROM 表1 JOIN 表2 USING(关联字段);
```

## 子查询

```sql
-- 标量子查询：返回单个值
SELECT * FROM 表1 WHERE 字段 = (SELECT 字段 FROM 表2 WHERE 条件);

-- 多行子查询：返回单列多行
SELECT * FROM 表1 WHERE 字段 IN (SELECT 字段 FROM 表2 WHERE 条件);

-- ANY 子查询：满足其一
SELECT * FROM 表1 WHERE 字段 > ANY (SELECT 字段 FROM 表2 WHERE 条件);

-- ALL 子查询：全部满足
SELECT * FROM 表1 WHERE 字段 > ALL (SELECT 字段 FROM 表2 WHERE 条件);

-- 表子查询（临时表）：必须加别名
SELECT * FROM (SELECT 字段, COUNT(*) FROM 表1 GROUP BY 分组) AS tmp WHERE cnt > 10;
```

## EXISTS vs IN 性能

```sql
-- EXISTS：找到第一个匹配即停止（半连接），适合外表小、内表大
SELECT * FROM orders o WHERE EXISTS (
    SELECT 1 FROM users u WHERE u.id = o.user_id AND u.level = 'vip'
);

-- IN：先执行子查询再匹配，适合外表大、子查询结果集小
SELECT * FROM orders WHERE user_id IN (
    SELECT id FROM users WHERE level = 'vip'
);

-- NOT EXISTS 通常优于 NOT IN（NOT IN 需要额外处理 NULL）
```

| 写法 | 执行逻辑 | 适合场景 |
|------|---------|---------|
| `IN (...)` | 先执行子查询生成临时表，再外层匹配 | 子查询结果集小 |
| `EXISTS (...)` | 对外层每行执行子查询，找到即停 | 外表小、子查询表大 |
| `JOIN` | 哈希/归并连接 | 关联字段有索引 |

## 经典题型速查

| 题型 | 要点 |
|------|------|
| JOIN vs 子查询 | JOIN 通常更快（优化器做连接顺序优化） |
| LEFT JOIN 条件陷阱 | 右表条件放 WHERE 会使 LEFT 退化为 INNER |
| EXISTS 优化原理 | Semi Join 半连接，找到匹配即停止扫描 |
| 多表 JOIN 顺序 | MySQL 优化器决定，不按书写顺序 |

> **工程要点**：多表查询关联字段必须有索引（EXPLAIN 看 type 列：`ref` 好于 `ALL`）。超过 3 张表 JOIN 考虑拆分 SQL 或冗余字段。

---



基础语法见 → [DDL, DML, DQL (SQL基础语法)](/06-Database%20(MySQL)/01%20·%20SQL基础/01-DDL,%20DML,%20DQL%20(SQL基础语法).md) · [Aggregate Functions & Group By (聚合与分组)](/06-Database%20(MySQL)/01%20·%20SQL基础/03-Aggregate%20Functions%20&%20Group%20By%20(聚合与分组).md)
