
# 1. 数据查询

### 基础查询

```sql
-- 查询表中所有字段
SELECT * FROM [表名];

-- 查询指定字段
SELECT [字段名1], [字段名2] FROM [表名];

-- 给字段起别名
SELECT [字段名] AS [别名] FROM [表名];

-- 字段运算查询
SELECT [字段1] + [字段2] FROM [表名];

-- 去重查询
SELECT DISTINCT [字段名] FROM [表名];
```

### 条件查询

```sql
-- 基础条件查询
SELECT * FROM [表名] WHERE [条件];

-- 排序查询（ASC：默认升序 ; DESC：降序）
SELECT * FROM [表名] ORDER BY [字段名] ASC;
SELECT * FROM [表名] ORDER BY [字段名] DESC;
```

# 2. where 子句运算符

- 比较运算符
	- `> < >= <= =`、`<> / !=`
	- `BETWEEN ... AND ...`：区间查询（包含首尾）
	- `IN( 值1, 值2)`：包含指定值
	- `LIKE`：模糊查询
	- `IS NULL`：判断为空

- 逻辑运算符
	- `AND`、`OR`、`NOT`

### 3. 模糊查询通配符

- `%`：匹配任意个字符（包含 0 个）
- `_`：匹配单个字符