

### 增

```sql
-- 插入单条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2);

-- 插入多条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2), (值3, 值4);

-- 插入数据（所有字段按顺序）
INSERT INTO [表名] VALUES (值1, 值2);
```

### 删

```sql
-- 删除指定数据
DELETE FROM [表名] WHERE [条件];

-- 清空表所有数据
DELETE FROM [表名];
```

### 改

```sql
-- 更新指定数据
UPDATE [表名] SET [字段1]=[值1] WHERE [条件];

-- 更新多条数据
UPDATE [表名] SET [字段1]=[值1], [字段2]=[值2] WHERE [条件];

-- 更新所有数据
UPDATE [表名] SET [字段1]=[值1];
```

### 去重

```sql
-- 统计重复数据（找出重复值）
SELECT [字段名], COUNT(*) FROM [表名] GROUP BY [字段名] HAVING COUNT(*) > 1;

-- 删除重复数据（保留主键最小的一行）
DELETE t1 FROM [表名] t1 JOIN [表名] t2 ON t1.[重复字段] = t2.[重复字段] AND t1.[主键] > t2.[主键];

-- 删除重复数据（按条件删除指定重复值）
DELETE FROM [表名] WHERE [重复字段] = '重复值' AND [主键] <> (SELECT MIN([主键]) FROM [表名] WHERE [重复字段] = '重复值');
```

### 复制

```sql
-- 只复制表结构
CREATE TABLE [新表名] LIKE [原表名];

-- 复制表结构 + 复制所有数据
CREATE TABLE [新表名] LIKE [原表名];
INSERT INTO [新表名] SELECT * FROM [原表名];
```