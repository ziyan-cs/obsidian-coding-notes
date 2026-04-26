

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

```sql
-- 拼接字符串
SELECT CONCAT([字符串1], [字符串2]) FROM [表名];

-- 截取子串
SELECT SUBSTRING([字段], [起始位置], [长度]) FROM [表名];

-- 去除两端空格
SELECT TRIM([字段]) FROM [表名];

-- 字符串长度（字节）
SELECT LENGTH([字段]) FROM [表名];

-- 大小写转换
SELECT UPPER([字段]) FROM [表名];
SELECT LOWER([字段]) FROM [表名];

-- 替换字符串
SELECT REPLACE([字段], [旧内容], [新内容]) FROM [表名];
```

- 了解
- `CHARSET()`、`STRCMP()`、`INSTR()`、`LEFT()`/`RIGHT()`

### 数学函数

```sql
-- 四舍五入
SELECT ROUND([数值], [小数位数]) FROM [表名];

-- 取绝对值
SELECT ABS([数值]) FROM [表名];

-- 取模
SELECT MOD([被除数], [除数]) FROM [表名];

-- 随机数
SELECT RAND() FROM [表名];
```

- 了解
- `CEIL()`/`FLOOR()`、`SQRT()`

### 日期时间函数

```sql
-- 当前日期时间
SELECT NOW() FROM [表名];

-- 日期格式化
SELECT DATE_FORMAT([日期], '%Y-%m-%d') FROM [表名];

-- 计算日期差
SELECT DATEDIFF([日期1], [日期2]) FROM [表名];

-- 日期加减
SELECT DATE_ADD([日期], INTERVAL [数值] DAY) FROM [表名];
```

- 了解
- `HOUR()`/`MINUTE()`/`SECOND()`、`DATE_SUB()`

### 流程控制函数

```sql
-- 条件判断
SELECT IF([条件], [值1], [值2]) FROM [表名];

-- 空值替换
SELECT IFNULL([字段], [默认值]) FROM [表名];

-- 多条件分支
SELECT CASE WHEN [条件1] THEN [结果1] ELSE [结果2] END FROM [表名];
```
