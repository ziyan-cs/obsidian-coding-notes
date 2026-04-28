

### 增加

```sql
-- 插入单条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2);

-- 插入多条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2), (值3, 值4);

-- 插入数据（所有字段按顺序）
INSERT INTO [表名] VALUES (值1, 值2);
```

### 删除数据

```sql
-- 删除指定数据
DELETE FROM [表名] WHERE [条件];

-- 清空表所有数据
DELETE FROM [表名];
```

### 修改

```sql
-- 更新指定数据
UPDATE [表名] SET [字段1]=[值1] WHERE [条件];

-- 更新多条数据
UPDATE [表名] SET [字段1]=[值1], [字段2]=[值2] WHERE [条件];

-- 更新所有数据
UPDATE [表名] SET [字段1]=[值1];
```