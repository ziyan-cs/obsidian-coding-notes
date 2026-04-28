

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
-- 统计行数
SELECT COUNT(*) FROM [表名];
SELECT COUNT([字段名]) FROM [表名];
SELECT COUNT(DISTINCT [字段名]) FROM [表名];

-- 求和
SELECT SUM([字段名]) FROM [表名];

-- 求平均值
SELECT AVG([字段名]) FROM [表名];

-- 求最大值
SELECT MAX([字段名]) FROM [表名];

-- 求最小值
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
SELECT CONCAT([字符串1], [字符串2], [...]) FROM [表名];

-- 截取子串
SELECT SUBSTRING([字段], [起始位置], [长度]) FROM [表名];

-- 去除两端空格
SELECT TRIM([字段]) FROM [表名];

-- 去除左侧空格
SELECT LTRIM([字段]) FROM [表名];

-- 去除右侧空格
SELECT RTRIM([字段]) FROM [表名];

-- 字符串长度（按字节）
SELECT LENGTH([字段]) FROM [表名];

-- 字符串长度（按字符）
SELECT CHAR_LENGTH([字段]) FROM [表名];

-- 转大写
SELECT UPPER([字段]) FROM [表名];

-- 转小写
SELECT LOWER([字段]) FROM [表名];

-- 替换指定字符串
SELECT REPLACE([字段], [旧内容], [新内容]) FROM [表名];
```

- 了解
- `CHARSET()`、`STRCMP()`、`INSTR()`、`LEFT()`/`RIGHT()`、`LPAD()`/`RPAD()`

### 数学函数

```sql
-- 四舍五入
SELECT ROUND([数值], [小数位数]) FROM [表名];

-- 数字格式化（带千分位，返回字符串）
SELECT FORMAT([数值], [小数位数]) FROM [表名];

-- 任意进制互转
SELECT CONV([数值/字符串], [原进制], [目标进制]) FROM [表名];

-- 向上取整（两种写法）
SELECT CEIL([数值]) FROM [表名];
SELECT CEILING([数值]) FROM [表名];

-- 向下取整
SELECT FLOOR([数值]) FROM [表名];

-- 取绝对值
SELECT ABS([数值]) FROM [表名];

-- 取模运算
SELECT MOD([被除数], [除数]) FROM [表名];

-- 生成 0~1 随机数
SELECT RAND() FROM [表名];

-- 求平方根
SELECT SQRT([数值]) FROM [表名];
```

- 了解
- `POW()`、`EXP()`、`LOG()`、`SIN()`、`COS()`、`TAN()`

### 日期时间函数

```sql
-- 获取当前日期时间
SELECT CURRENT_DATE() FROM DUAL;      -- 当前日期（YYYY-MM-DD）
SELECT CURRENT_TIME() FROM DUAL;      -- 当前时间（HH:MM:SS）
SELECT NOW() / CURRENT_TIMESTAMP() FROM DUAL; -- 当前完整日期时间

-- 提取日期时间 / 年月日时分秒
SELECT DATE([datetime]) FROM DUAL;    -- 提取日期（YYYY-MM-DD）
SELECT TIME([datetime]) FROM DUAL;    -- 提取时间（HH:MM:SS）
SELECT YEAR([datetime]), MONTH([datetime]), DAY([datetime]) FROM DUAL; -- 提取年/月/日
SELECT HOUR([datetime]), MINUTE([datetime]), SECOND([datetime]) FROM DUAL; -- 提取时/分/秒

-- 日期加减计算
SELECT DATE_ADD([date], INTERVAL [数值] [单位]) FROM DUAL;  -- 日期加
SELECT DATE_SUB([date], INTERVAL [数值] [单位]) FROM DUAL;  -- 日期减

-- 日期差 / 时间差
SELECT DATEDIFF([date1], [date2]) FROM DUAL;   -- 计算两个日期相差天数（忽略时间）
SELECT TIMEDIFF([time1], [time2]) FROM DUAL;   -- 计算两个时间差

-- 日期 ↔ 字符串 互逆转换
SELECT DATE_FORMAT([datetime], '[格式串]') FROM DUAL; -- 日期→格式字符串
SELECT STR_TO_DATE('[字符串]', '[格式串]') FROM DUAL;   -- 字符串→格式日期

-- 日期 ↔ 时间戳 互逆转换
SELECT UNIX_TIMESTAMP([datetime]) FROM DUAL;    -- 日期→Unix时间戳
SELECT FROM_UNIXTIME([时间戳]) FROM DUAL;        -- Unix时间戳→日期
```

- 了解
- `HOUR()`、`MINUTE()`、`SECOND()`、`DAYNAME()`、`MONTHNAME()`、`WEEK()`

### 流程控制函数

```sql
-- 条件判断
SELECT IF([条件], [成立值], [不成立值]) FROM [表名];

-- 空值替换
SELECT IFNULL([字段], [默认值]) FROM [表名];

-- 多条件分支判断
SELECT CASE WHEN [条件1] THEN [结果1] WHEN [条件2] THEN [结果2] ELSE [默认结果] END FROM [表名];

-- 空值判断
SELECT NULLIF([表达式1], [表达式2]) FROM [表名];
```

# 7. 多表查询

### 连接查询

````sql
-- 内连接：取两表交集数据
SELECT [表1].[字段], [表2].[字段]
FROM [表1]
INNER JOIN [表2]
ON [表1].[关联字段] = [表2].[关联字段];

-- 左外连接：左表全量 + 右表匹配项（无匹配为NULL）
SELECT [表1].[字段], [表2].[字段]
FROM [表1]
LEFT JOIN [表2]
ON [表1].[关联字段] = [表2].[关联字段];

-- 右外连接：右表全量 + 左表匹配项（无匹配为NULL）
SELECT [表1].[字段], [表2].[字段]
FROM [表1]
RIGHT JOIN [表2]
ON [表1].[关联字段] = [表2].[关联字段];

-- 等值连接简化写法（两表关联字段名相同时）
SELECT * FROM [表1] JOIN [表2] USING([关联字段]);

-- 自连接：一张表自己和自己连接（比如员工和经理都在emp表中）
SELECT [表别名1].[字段], [表别名2].[字段]
FROM [表名] [表别名1], [表名] [表别名2]
WHERE [表别名1].[关联字段] = [表别名2].[关联字段];
````

### 子查询

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

### 联合查询

```sql
-- UNION：合并结果集并自动去重
SELECT [字段列表] FROM [表1]
UNION
SELECT [字段列表] FROM [表2];

-- UNION ALL：合并结果集不去重（性能更高）
SELECT [字段列表] FROM [表1]
UNION ALL
SELECT [字段列表] FROM [表2];
```