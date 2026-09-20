---
study_stage: backlog
tags: [database/mysql, operations/configuration]
---

> [!abstract] 学习目标：不背固定安装路径，而是从运行实例查询配置来源、数据目录、字符集、连接状态和元数据，并知道哪些文件不能手工操作。

# 先确认正在连接哪个实例

同一机器可能有多个 MySQL 实例、容器或远端服务。排障先确认服务端身份与当前会话：

```sql
SELECT VERSION(), @@hostname, @@port, CONNECTION_ID();
SELECT CURRENT_USER(), USER(), DATABASE();
SHOW VARIABLES WHERE Variable_name IN
  ('basedir','datadir','socket','pid_file','log_error');
```

`USER()` 表示客户端提供的账号与来源，`CURRENT_USER()` 表示权限检查实际匹配的账户，二者不同有助于解释授权结果。不要仅凭客户端路径推断服务端版本和数据目录。

# 配置来源与生效范围

MySQL 可从编译默认、option file、命令行和持久化系统变量等来源获得配置，优先级及可动态修改性取决于变量。先查询实际值与 scope，再修改配置：

```sql
SHOW GLOBAL VARIABLES LIKE 'max_connections';
SHOW SESSION VARIABLES LIKE 'transaction_isolation';
SHOW GLOBAL STATUS LIKE 'Threads_connected';
```

`SET SESSION` 只影响当前会话，`SET GLOBAL` 通常只影响之后的新会话；是否可持久化、是否需要重启以目标 MySQL 版本文档为准。变更前记录旧值、预期影响、回滚和观测指标。

Windows 常见 option file 位于 ProgramData，Linux 发行版常拆分到 `/etc/mysql/` 下，容器通常通过挂载或启动参数提供。固定路径不是知识点，`mysqld --verbose --help`、服务定义和运行变量才是可靠入口。

# 数据目录不是手工管理接口

`datadir` 中包含 InnoDB 表空间、redo/undo、二进制日志、系统表和其他实例状态；具体文件布局受版本、配置和表空间模式影响。不要手工移动、复制、删除 `.ibd`、`ibdata` 或 redo 文件，也不要把“停止服务后复制目录”当成通用备份方案。

备份必须选择受支持的方法并完成恢复演练：逻辑备份适合可移植导出，物理备份适合较大实例但要求工具、版本与恢复流程匹配。复制、备份与高可用解决的问题不同。

# 系统 schema 与元数据

| Schema | 用途 |
| --- | --- |
| `information_schema` | SQL 标准风格的库、表、列、约束等元数据视图 |
| `mysql` | 账户、权限、组件和服务端内部系统表；通过 SQL 管理，不手工改文件 |
| `performance_schema` | 低层事件、等待、语句、锁和资源观测 |
| `sys` | 基于 performance_schema 的可读诊断视图 |

```sql
SELECT table_schema, table_name, engine
FROM information_schema.tables
WHERE table_schema = 'app';

SELECT * FROM sys.session
WHERE conn_id <> CONNECTION_ID();
```

`SHOW PROCESSLIST`/`performance_schema` 用于观察连接和执行状态，但完整 SQL 可能包含敏感值，开放权限和采集日志时要控制访问。

# 连接诊断

在 PowerShell 中先测试 TCP，再使用客户端连接；它们是 shell 命令，不是 SQL：

```powershell
Test-NetConnection db.example.internal -Port 3306
mysql -h db.example.internal -P 3306 -u app_user -p
```

“端口可达”只证明 TCP 建连，不证明 TLS、认证、授权、目标 schema 或查询可用。连接后继续检查证书要求、账户来源匹配、权限和字符集：

```sql
SHOW GRANTS;
SHOW VARIABLES LIKE 'character_set%';
SHOW VARIABLES LIKE 'collation%';
```

常见排障顺序：DNS/路由与端口 → TLS → 认证 → 授权 → 默认 schema → 连接池与服务端容量。不要为解决连接失败临时把 `bind-address` 改成公网监听或给应用账户授予全局权限。

# 实践

- [ ] 查询当前实例版本、hostname、port、datadir、socket 和错误日志位置。
- [ ] 比较 `USER()` 与 `CURRENT_USER()`，解释权限实际匹配对象。
- [ ] 建立最小权限只读账户，验证越权写入被拒绝并产生审计证据。
- [ ] 做一次逻辑备份并恢复到独立实例，校验表数、行数和关键约束。
- [ ] 使用 performance_schema/sys 找出一个慢查询或连接等待，而不是只看进程是否存在。

# 官方参考

- [MySQL 8.4 Reference Manual: Using Option Files](https://dev.mysql.com/doc/refman/8.4/en/option-files.html)
- [MySQL 8.4 Reference Manual: Data Dictionary](https://dev.mysql.com/doc/refman/8.4/en/data-dictionary.html)
- [MySQL 8.4 Reference Manual: Performance Schema](https://dev.mysql.com/doc/refman/8.4/en/performance-schema.html)
