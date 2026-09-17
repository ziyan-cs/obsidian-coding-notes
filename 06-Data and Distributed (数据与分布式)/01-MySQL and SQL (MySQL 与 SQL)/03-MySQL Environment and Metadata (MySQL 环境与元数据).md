---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习目标：理解 MySQL 安装目录、配置、系统库、连接状态和元数据查询。

# MySQL Overview (MySQL 基础)

> [!note] 本节重点：MySQL 安装配置与目录结构、系统数据库功能、连接管理与状态查询

> [!warning] **平台/版本说明（VERSION_CHECK）**：以下路径以 **Windows 本机 MySQL 8.0** 为例，路径含版本号 `MySQL Server 8.0`，升级版本后需同步更新。Linux（vm-ubuntu）下的对应关系：程序 `mysqld`/`mysql`、配置文件 `/etc/mysql/mysql.conf.d/mysqld.cnf`、数据目录 `datadir`（用 `SHOW VARIABLES LIKE 'datadir'` 查询）、服务管理用 `systemctl` 而非 Windows 服务。

## 核心配置与目录

### 程序安装目录

- `C:\Program Files\MySQL\MySQL Server 8.0\`

- 子目录
    - `bin/`：`mysqld.exe` 服务端、`mysql.exe` 客户端等可执行文件
    - `include/` & `lib/`：开发相关头文件和库文件

### my.ini 配置文件

- `C:\ProgramData\MySQL\MySQL Server 8.0\my.ini`

- **MySQL** 服务的全局配置文件
	- 控制端口、数据路径、字符集、绑定地址等

- 注意
	- 备份原文件，以便配置错误恢复
	- 修改前须停止 `MySQL80` 服务，修改后启动

```shell
port=3306

datadir=C:/ProgramData/MySQL/MySQL Server 8.0/Data/  

character-set-server=utf8mb4

bind-address=127.0.0.1             
```

### data 数据目录

- `C:\ProgramData\MySQL\MySQL Server 8.0\Data\`

- **MySQL** 的数据存储目录
	- 存放所有数据库、表、日志的物理文件

- 子目录
    - 4 个系统内置数据库，严禁手动修改 / 删除
    - `[自建库名]/`：自建数据库对应文件夹
	    - `.ibd` 表数据
    - `ibdata1`：共享表空间文件，存储表结构、回滚日志等
    - `*.err`：错误日志文件

- 注意
    - 只能在停止 `MySQL80` 服务后备份整个 `data` 目录

## 系统自带数据库

- **information_schema**
    - 存储数据库、表、字段的元信息
    - 只读，不能修改或删除

- **mysql**
    - 核心系统库
    - 存储用户账号、密码、权限

- **performance_schema**
    - 用于监控数据库性能

- **sys**
    - 简化性能查看的系统库

## MySQL 连接与退出

```sql
Test-NetConnection [主机IP] -Port 3306

mysql -h [主机IP] -P [端口号] -u [用户名] -p

```

## 配置与状态查询

```sql
show variables like 'port';

show variables like 'character_set_server';

show variables like 'datadir';

```

---

SQL基础语法详解见 → DDL, DML, DQL (SQL基础语法)
