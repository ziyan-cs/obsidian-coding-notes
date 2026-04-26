

# 1. 基础查询

```sql
-- 所有
SELECT * FROM [表名];

-- 指定
SELECT [字段名1], [字段名2] FROM [表名];

-- 字段运算
SELECT [字段1] + [字段2] FROM [表名];

-- 别名
SELECT [字段名] AS [别名] FROM [表名];

-- 去重
SELECT DISTINCT [字段名] FROM [表名];
```

# 2. 条件查询

### where 子句


### 运算符
- 比较运算符
	- `> < >= <= =`、`<> / !=`
	- `BETWEEN ... AND ...`：区间查询（包含首尾）
	- `IN( 值1, 值2)`：包含指定值
	- `LIKE`：模糊查询
	- `IS NULL`：判断为空

- 逻辑运算符
	- `AND`、`OR`、`NOT`

```sql
-- 基础条件查询
SELECT * FROM [表名] WHERE [条件];
```
- `%`：匹配任意个字符（包含 0 个）
- `_`：匹配单个字符





# 3. 排序与分类

### order by

```sql
-- ASC: 升序 (默认)
SELECT * FROM [表名] ORDER BY [字段名] ASC;
-- DESC: 降序
SELECT * FROM [表名] ORDER BY [字段名] DESC;

```

### limit

```sql

```

# 4. 聚合函数

### count


### sum


### avg


### max


### min
