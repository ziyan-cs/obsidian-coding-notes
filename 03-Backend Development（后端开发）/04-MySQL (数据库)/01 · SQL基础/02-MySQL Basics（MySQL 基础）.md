

# 1. 核心配置与目录

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
# 服务监听端口，默认 3306
port=3306

# 数据文件根目录
datadir=C:/ProgramData/MySQL/MySQL Server 8.0/Data/  

# 服务端默认字符集，支持中文和 emoji
character-set-server=utf8mb4

# 绑定IP，127.0.0.1=仅本地；0.0.0.0=允许远程
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

# 2. 系统自带数据库（4 个）

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

# 3. MySQL 连接与退出


```sql
# 测试网络联通性
ping [主机IP]

# 测试端口连通性
Test-NetConnection [主机IP] -Port 3306

# 远程连接 MySQL
mysql -h [主机IP] -P [端口号] -u [用户名] -p

# 本地登录 MySQL
mysql -u root -p

# 退出 MySQL
quit;
exit;
```

# 4. ### 配置与状态查询

```sql
# 查看当前端口号 (默认3306)
show variables like 'port';

# 查看字符集
show variables like 'character_set_server';

# 查看数据目录
show variables like 'datadir';

# 查看当前数据库
select database();

# 查看当前用户
select user();

# 查看数据库版本
select version();
```
