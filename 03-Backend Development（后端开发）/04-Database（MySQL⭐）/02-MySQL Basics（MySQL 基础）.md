


# 1. 核心配置与目录

### 程序安装目录

- 路径：`C:\Program Files\MySQL\MySQL Server 8.0\`
- 核心子目录
    - `bin/`：存放 `mysqld.exe`（服务端）、`mysql.exe`（客户端）等可执行文件
    - `include/`、`lib/`：开发相关头文件和库文件，日常使用一般不用管

### my.ini 配置文件

- MySQL 服务的全局配置中心
	- 控制端口、字符集、数据路径、绑定地址等核心行为

- 默认路径（Windows）：`C:\ProgramData\MySQL\MySQL Server 8.0\my.ini`

```shell
port=3306                          # 服务监听端口，默认3306
datadir=C:/ProgramData/MySQL/MySQL Server 8.0/Data/  # 数据文件根目录
character-set-server=utf8mb4       # 服务端默认字符集，支持中文和emoji
bind-address=127.0.0.1             # 绑定IP，127.0.0.1=仅本地；0.0.0.0=允许远程
```

- **操作注意**：
    - 修改前必须停止 `MySQL80` 服务，否则不生效或启动失败。
    - 修改前建议备份原文件，避免配置错误无法恢复。

### data 数据目录

- 存放所有数据库、表、日志的物理文件，是 MySQL 的「数据仓库」
- 默认路径（Windows）**：`C:\ProgramData\MySQL\MySQL Server 8.0\Data\`
    
    - `mysql/`、`information_schema/`、`performance_schema/`、`sys/`：4 个系统内置数据库，**严禁手动修改 / 删除**
    - `[自建库名]/`：自己创建的数据库对应文件夹，表数据以 `.ibd` 文件形式存储
    - `ibdata1`：共享表空间文件，存储表结构、回滚日志等
    - `*.err`：错误日志文件
    
- **操作注意**：
    
    - 所有库表操作必须通过 SQL 命令 / Workbench 执行，**禁止直接修改文件**，否则会导致数据库损坏。
    - 备份数据库时，可在停止服务后直接备份整个 `data` 目录。

# 2. 系统自带数据库（4 个）

- **information_schema**
    - 存储数据库、表、字段的元信息
    - 只读，不能修改删除

- **mysql**
    - 核心系统库
    - 存储用户账号、密码、权限

- **performance_schema**
    - 用于监控数据库性能

- **sys**
    - 简化性能查看的系统库

# 3. MySQL 常用命令

### 连接与退出

```cpp
# 登录 MySQL
mysql -u root -p

# 退出 MySQL
quit;
exit;
```

### 数据库操作

```cpp
# 查看所有数据库
show databases;

# 创建数据库
create database 库名;

# 创建数据库（指定字符集） 
create database 库名 default character set utf8mb4;

# 删除数据库
drop database [库名];

# 用/切换数据库
use [库名];

# 查看当前使用的数据库
select database();
```

### 数据表操作

```cpp
# 查看当前库的所有表
show tables;

# 查看表结构
desc 表名;
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

### 数据增删改查（CRUD）

```cpp
# 增：插入数据
insert into [表名] (字段1, 字段2) values (值1, 值2);

# 删：删除数据
delete from [表名] where [条件];

# 改：更新数据
update [表名] set [字段1]=[值1] where [条件];

# 查：查询数据
select * from [表名];                  # 查询所有
select [字段1],[字段2] from [表名];         # 查询指定字段
select * from [表名] where [条件];        # 条件查询
select * from [表名] order by [字段] asc; # 排序 (asc升序/desc降序)
select * from [表名] limit 10;          # 限制条数
```

### 配置与状态查询

```cpp
# 查看当前端口号 (默认3306)
show variables like 'port';

# 查看字符集
show variables like 'character_set_server';

# 查看数据目录
show variables like 'datadir';

# 查看当前用户
select user();

# 查看数据库版本
select version();
```

### 远程连接命令

```cpp
# 测试网络联通性
ping [主机IP]

# 测试端口连通性
Test-NetConnection [主机IP] -Port 3306

# 远程连接 MySQL
mysql -h [主机IP] -P [端口号] -u [用户名] -p
```

