
# 1. 数据库操作

```sql
-- 查看所有数据库
SHOW DATABASES;

-- 创建数据库
CREATE DATABASE [库名];

-- 创建数据库（指定字符集）
CREATE DATABASE [库名] 
DEFAULT CHARACTER SET utf8mb4;

-- 创建数据库（同时指定字符集和校验规则）
CREATE DATABASE [库名] 
DEFAULT CHARACTER SET utf8mb4 
COLLATE utf8mb4_general_ci;

-- 修改数据库字符集与校验规则
ALTER DATABASE [库名] DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 删除数据库
DROP DATABASE [库名];

-- 使用/切换数据库
USE [库名];

-- 查看当前使用的数据库
SELECT DATABASE();

-- 查看指定数据库的字符集、校验规则 
SELECT DEFAULT_CHARACTER_SET_NAME, DEFAULT_COLLATION_NAME FROM information_schema.SCHEMATA WHERE SCHEMA_NAME = '库名';
```

# 2. 数据表操作

```sql
-- 查看当前库的所有表
SHOW TABLES;

-- 查看表结构
DESC [表名];
SHOW CREATE TABLE [表名];

-- 创建表（完整规范写法）
CREATE TABLE [表名] (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL,
    age INT,
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 修改表的字段集与校验规则
ALTER TABLE [表名] DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 修改表名
RENAME TABLE [旧表名] TO [新表名];

-- 删除表
DROP TABLE [表名];

-- 查看指定数据表的设置
SHOW TABLE STATUS LIKE '表名';
```

# 3. 字段操作

```sql
ALTER TABLE [表名] ADD COLUMN [字段名] [类型];

-- 添加字段（带约束，推荐）
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] [NOT NULL/DEFAULT/COMMENT];

-- 示例1：在末尾添加字段（默认） 
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] COMMENT '字段说明';

-- 示例2：在指定字段后添加
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] AFTER [指定字段名] COMMENT '邮箱';

-- 示例3：添加到表开头
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] FIRST COMMENT '身份证号';

-- 修改字段类型
ALTER TABLE [表名] MODIFY COLUMN [字段名] [新类型];

-- 修改字段名
ALTER TABLE [表名] CHANGE COLUMN [旧字段名] [新字段名] [类型];

-- 删除字段
ALTER TABLE [表名] DROP COLUMN [字段名];

-- 查字段全套信息
SHOW FULL COLUMNS FROM [表名];
```


### 1. 增

```sql
-- 插入单条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2);

-- 插入多条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2), (值3, 值4);

-- 插入数据（所有字段按顺序）
INSERT INTO [表名] VALUES (值1, 值2);
```

### 2. 删

```sql
-- 删除指定数据
DELETE FROM [表名] WHERE [条件];

-- 清空表所有数据
DELETE FROM [表名];
```

### 3. 改

```sql
-- 更新指定数据
UPDATE [表名] SET [字段1]=[值1] WHERE [条件];

-- 更新多条数据
UPDATE [表名] SET [字段1]=[值1], [字段2]=[值2] WHERE [条件];

-- 更新所有数据
UPDATE [表名] SET [字段1]=[值1];
```

### 4. 去重

```sql
-- 统计重复数据（找出重复值）
SELECT [字段名], COUNT(*) FROM [表名] GROUP BY [字段名] HAVING COUNT(*) > 1;

-- 删除重复数据（保留主键最小的一行）
DELETE t1 FROM [表名] t1 JOIN [表名] t2 ON t1.[重复字段] = t2.[重复字段] AND t1.[主键] > t2.[主键];

-- 删除重复数据（按条件删除指定重复值）
DELETE FROM [表名] WHERE [重复字段] = '重复值' AND [主键] <> (SELECT MIN([主键]) FROM [表名] WHERE [重复字段] = '重复值');
```

### 5. 复制

```sql
-- 只复制表结构
CREATE TABLE [新表名] LIKE [原表名];

-- 复制表结构 + 复制所有数据
CREATE TABLE [新表名] LIKE [原表名];
INSERT INTO [新表名] SELECT * FROM [原表名];
```
