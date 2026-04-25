
# 1. 数据库操作

```sql
# 查看所有数据库
show databases;

# 创建数据库
create database [库名];

# 创建数据库
create database [库名] default character set utf8mb4;（指定字符集）

# 创建数据库（同时指定字符集和校验规则）
create database [库名] default character set utf8mb4 collate utf8mb4_general_ci;

# 删除数据库
drop database [库名];

# 用/切换数据库
use [库名];

# 查看当前使用的数据库
select database();
```

# 2. 数据表操作

```sql
# 查看当前库的所有表
show tables;

# 查看表结构
desc [表名];
show create table [表名];

# 创建表
create table [表名] (
    id int primary key auto_increment,
    name varchar(50) not null,
    age int
);

# 删除表
drop table [表名];

# 修改表名
rename table [旧表名] to [新表名];

# 修改表结构（添加字段）
alter table [表名] add column [字段名] [类型];

# 修改字段类型
alter table [表名] modify column [字段名] [新类型];

# 删除字段
alter table [表名] drop column [字段名];
```

