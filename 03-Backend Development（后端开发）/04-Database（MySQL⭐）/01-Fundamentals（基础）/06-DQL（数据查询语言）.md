

# 1. 基础查询

```sql
-- 查询所有字段
SELECT * FROM [表名];

-- 查询指定字段
SELECT [字段名1], [字段名2] FROM [表名];

-- 字段运算查询
SELECT [字段1] + [字段2] FROM [表名];

-- 字段别名
SELECT [字段名] AS [别名] FROM [表名];

-- 去重查询
SELECT DISTINCT [字段名] FROM [表名];
```

# 2. 条件过滤

- 在数据分组 / 聚合之前，按行过滤原始数据

```sql
-- 基础条件查询
SELECT * FROM [表名] WHERE [条件];
```

- 比较运算符
	- `> < >= <= =`、`<> / !=`
	- `BETWEEN ... AND ...`：区间查询（包含首尾）
	- `IN( 值1, 值2)`：匹配指定值
	- `LIKE`：模糊查询
		- `%`：匹配任意个字符（包含 0 个）
		- `_`：匹配单个字符
	- `IS NULL`：判断是否为空

- 逻辑运算符
	- `AND`、`OR`、`NOT`

# 3. 聚合函数

```sql
-- count：统计行数
SELECT COUNT(*) FROM [表名];
SELECT COUNT([字段名]) FROM [表名];
SELECT COUNT(DISTINCT [字段名]) FROM [表名];

-- sum：求和
SELECT SUM([字段名]) FROM [表名];

-- avg：求平均值
SELECT AVG([字段名]) FROM [表名];

-- max：求最大值
SELECT MAX([字段名]) FROM [表名];

-- min：求最小值
SELECT MIN([字段名]) FROM [表名];
```

# 4. 分组查询

### group by

```sql
-- 按指定字段分组 
SELECT [分组字段], 聚合函数([字段名]) FROM [表名] GROUP BY [分组字段];
```

### having

```sql
-- 分组后过滤结果
SELECT [分组字段], 聚合函数([字段名]) 
FROM [表名] 
GROUP BY [分组字段] 
HAVING 聚合函数([字段名]) [条件];
```

# 5. 排序与分页

### order by

```sql
-- ASC: 升序 (默认)
SELECT * FROM [表名] ORDER BY [字段名] ASC;
-- DESC: 降序
SELECT * FROM [表名] ORDER BY [字段名] DESC;

```

### limit

```sql
-- 取前 n 条数据
SELECT * FROM [表名] LIMIT n;

-- 分页查询：从第 offset 条开始，取 n 条
SELECT * FROM [表名] LIMIT offset, n;
```

# 6. 常用内置函数

### 字符串函数



### 数学函数

### 日期时间函数

### 流程控制函数