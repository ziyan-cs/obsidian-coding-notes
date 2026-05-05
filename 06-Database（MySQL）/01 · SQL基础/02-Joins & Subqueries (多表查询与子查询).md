

# 联合查询

```sql
-- UNION：合并结果集并去重
SELECT [字段列表] FROM [表1]UNION SELECT [字段列表] FROM [表2];

-- UNION ALL：合并结果集不去重
SELECT [字段列表] FROM [表1] UNION ALL SELECT [字段列表] FROM [表2];
```

# 连接查询

````sql
-- 自连接：一张表自己和自己连接
SELECT [表别名1].[字段], [表别名2].[字段] FROM [表名] [表别名1], [表名] [表别名2]
WHERE [表别名1].[关联字段] = [表别名2].[关联字段];

-- 内连接：取两表交集数据
SELECT [表1].[字段], [表2].[字段] FROM [表1]
INNER JOIN [表2] ON [表1].[关联字段] = [表2].[关联字段];

-- 左外连接：左表全量 + 右表匹配项（无匹配为NULL）
SELECT [表1].[字段], [表2].[字段] FROM [表1]
LEFT JOIN [表2] ON [表1].[关联字段] = [表2].[关联字段];

-- 右外连接：右表全量 + 左表匹配项（无匹配为NULL）
SELECT [表1].[字段], [表2].[字段] FROM [表1]
RIGHT JOIN [表2] ON [表1].[关联字段] = [表2].[关联字段];

-- 等值连接简化写法（两表关联字段名相同时）
SELECT * FROM [表1] JOIN [表2] USING([关联字段]);
````

# 子查询

```sql
-- 标量子查询：返回单个值
SELECT * FROM [表1] WHERE [字段] = (SELECT [字段] FROM [表2] WHERE [条件]);

-- 多行子查询：返回单列多行
SELECT * FROM [表1] WHERE [字段] IN (SELECT [字段] FROM [表2] WHERE [条件]);

-- ANY子查询：满足其一即可
SELECT * FROM [表1] WHERE [字段] [运算符] ANY (SELECT [字段] FROM [表2] WHERE [条件]);

-- ALL子查询：全部满足才成立
SELECT * FROM [表1] WHERE [字段] [运算符] ALL (SELECT [字段] FROM [表2] WHERE [条件]);

-- 多列子查询：返回多列多行，需字段列表完全匹配
SELECT * FROM [表1] WHERE ([字段1], [字段2]) IN (SELECT [字段1], [字段2] FROM [表2] WHERE [条件]);

-- 表子查询（临时表）：返回临时表（必须加别名）
SELECT * FROM (SELECT [字段], 聚合函数([字段]) FROM [表1] GROUP BY [分组字段]) AS [临时表别名] WHERE [聚合字段] [条件];
```

