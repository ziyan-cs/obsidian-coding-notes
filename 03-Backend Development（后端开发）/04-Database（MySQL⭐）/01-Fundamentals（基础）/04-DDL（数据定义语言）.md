
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

-- 删除数据库
DROP DATABASE [库名];

-- 使用/切换数据库
USE [库名];

-- 查看当前使用的数据库
SELECT DATABASE();
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
    age INT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 修改表名
RENAME TABLE [旧表名] TO [新表名];

-- 删除表
DROP TABLE [表名];
```

# 3. 字段操作

```sql
ALTER TABLE [表名] ADD COLUMN [字段名] [类型];

-- 添加字段（带约束，推荐）
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] [NOT NULL/DEFAULT/COMMENT];

-- 示例1：在末尾添加字段（默认） 
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] COMMENT [字段名];

-- 示例2：在指定字段后添加（MySQL 支持，其他数据库不通用）
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] AFTER [指定字段名] COMMENT '邮箱';

-- 示例3：添加到表开头
ALTER TABLE [表名] ADD COLUMN [字段名] [类型] FIRST COMMENT '身份证号';


-- 修改字段类型
ALTER TABLE [表名] MODIFY COLUMN [字段名] [新类型];

-- 修改字段名（必须用 CHANGE COLUMN）
ALTER TABLE [表名] CHANGE COLUMN [旧字段名] [新字段名] [类型];

-- 删除字段
ALTER TABLE [表名] DROP COLUMN [字段名];
```