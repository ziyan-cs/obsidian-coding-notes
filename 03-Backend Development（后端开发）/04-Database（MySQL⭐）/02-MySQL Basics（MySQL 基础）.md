


# 1. 核心配置与目录

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

### 程序安装目录

- 路径：`C:\Program Files\MySQL\MySQL Server 8.0\`
- 核心子目录
    - `bin/`：存放 `mysqld.exe`（服务端）、`mysql.exe`（客户端）等可执行文件。
    - `include/`、`lib/`：开发相关头文件和库文件，日常使用一般不用管。

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

```shell
# 登录 MySQL
mysql -u root -p

# 退出 MySQL
exit;
```

### 数据库操作

```bash
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

### 

# 
`select * from [表名]`;

# 查看端口号 ( 默认 3306 )
show variables like 'port';




# 4. 远程连接 MySQL 命令

```shell
mysql -h [主机IP] -P [端口] -u [用户名] -p


- `-h`：远程主机 IP 地址
- `-P`（大写）：端口号，默认 3306
- `-u`：用户名
- `-p`：密码

# 5. 测试端口连通性

```shell
Test-NetConnection IP -Port 3306
