---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# 30 秒回答

**核心结论**：学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# SQL Basics (SQL 基础语法)

> [!note] 本节重点：核心考点：DDL 数据库/表/字段操作、DML 增删改、DQL 基础查询/条件过滤/聚合/分组/排序分页

# DDL（数据定义）
## 1. 数据库操作

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

## 2. 数据表操作

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

## 3. 字段操作

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

# DML（数据操作）

## 1. 增

```sql
-- 插入单条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2);

-- 插入多条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2), (值3, 值4);

-- 插入数据（所有字段按顺序）
INSERT INTO [表名] VALUES (值1, 值2);
```

## 2. 删

```sql
-- 删除指定数据
DELETE FROM [表名] WHERE [条件];

-- 清空表所有数据
DELETE FROM [表名];
```

## 3. 改

```sql
-- 更新指定数据
UPDATE [表名] SET [字段1]=[值1] WHERE [条件];

-- 更新多条数据
UPDATE [表名] SET [字段1]=[值1], [字段2]=[值2] WHERE [条件];

-- 更新所有数据
UPDATE [表名] SET [字段1]=[值1];
```

## 4. 去重

```sql
-- 统计重复数据（找出重复值）
SELECT [字段名], COUNT(*) FROM [表名] GROUP BY [字段名] HAVING COUNT(*) > 1;

-- 删除重复数据（保留主键最小的一行）
DELETE t1 FROM [表名] t1 JOIN [表名] t2 ON t1.[重复字段] = t2.[重复字段] AND t1.[主键] > t2.[主键];

-- 删除重复数据（按条件删除指定重复值）
DELETE FROM [表名] WHERE [重复字段] = '重复值' AND [主键] <> (SELECT MIN([主键]) FROM [表名] WHERE [重复字段] = '重复值');
```

## 5. 复制

```sql
-- 只复制表结构
CREATE TABLE [新表名] LIKE [原表名];

-- 复制表结构 + 复制所有数据
CREATE TABLE [新表名] LIKE [原表名];
INSERT INTO [新表名] SELECT * FROM [原表名];
```

# DQL（数据查询）

## 1. 基础查询

```sql
-- 查看当前数据库
SELECT DATABASE();

-- 查看当前用户
SELECT USER();

-- 查看数据库版本
SELECT VERSION();

-- 查询所有字段
SELECT * FROM [表名];

-- 指定字段查询
SELECT [字段名1], [字段名2] FROM [表名];

-- 字段运算查询
SELECT [字段1] + [字段2] FROM [表名];

-- 别名
SELECT [字段名] AS [别名] FROM [表名];

-- 去重查询
SELECT DISTINCT [字段名] FROM [表名];
```

## 2. 条件过滤

```sql
-- 基础条件查询
SELECT * FROM [表名] WHERE [条件];

-- 比较运算符 > < >= <= = <> !=
SELECT * FROM [表名] WHERE [字段] > [值];
SELECT * FROM [表名] WHERE [字段] <> [值];

-- BETWEEN ... AND 区间查询（包含首尾）
SELECT * FROM [表名] WHERE [字段] BETWEEN [值1] AND [值2];

-- IN 匹配指定值
SELECT * FROM [表名] WHERE [字段] IN ([值1], [值2], [值3]);

-- LIKE 模糊查询（%匹配任意字符，_匹配单个字符）
SELECT * FROM [表名] WHERE [字段] LIKE '%关键词%';
SELECT * FROM [表名] WHERE [字段] LIKE 'A_';

-- IS NULL / IS NOT NULL 判断空值
SELECT * FROM [表名] WHERE [字段] IS NULL;
SELECT * FROM [表名] WHERE [字段] IS NOT NULL;

-- 逻辑运算符 AND OR NOT
SELECT * FROM [表名] WHERE [条件1] AND [条件2];
SELECT * FROM [表名] WHERE [条件1] OR [条件2];
SELECT * FROM [表名] WHERE NOT [条件];
```

## 3. 聚合函数

```sql
-- 统计行数
SELECT COUNT(*) FROM [表名];
SELECT COUNT([字段名]) FROM [表名];
SELECT COUNT(DISTINCT [字段名]) FROM [表名];

-- 求和
SELECT SUM([字段名]) FROM [表名];

-- 求平均值
SELECT AVG([字段名]) FROM [表名];

-- 求最大值
SELECT MAX([字段名]) FROM [表名];

-- 求最小值
SELECT MIN([字段名]) FROM [表名];
```

## 4. 分组查询

- group by

```sql
-- 按指定字段分组 
SELECT [分组字段], [聚合函数]([字段名]) FROM [表名] GROUP BY [分组字段];
```

- having

```sql
-- 分组后过滤结果
SELECT [分组字段], [聚合函数]([字段名]) FROM [表名] GROUP BY [分组字段] 
	HAVING [聚合函数]([字段名]) [条件];
```

## 5. 排序与分页

 - order by

```sql
-- ASC: 升序 (默认)
SELECT * FROM [表名] ORDER BY [字段名] ASC;
-- DESC: 降序
SELECT * FROM [表名] ORDER BY [字段名] DESC;

```

- limit

```sql
-- 取前 n 条数据
SELECT * FROM [表名] LIMIT n;

-- 分页查询：从第 offset 条开始，取 n 条
SELECT * FROM [表名] LIMIT offset, n;
```

## 6. 常用内置函数

- 字符串函数

```sql
-- 拼接字符串
SELECT CONCAT([字符串1], [字符串2], [...]) FROM [表名];

-- 截取子串
SELECT SUBSTRING([字段], [起始位置], [长度]) FROM [表名];

-- 去除两端空格
SELECT TRIM([字段]) FROM [表名];

-- 去除左侧空格
SELECT LTRIM([字段]) FROM [表名];

-- 去除右侧空格
SELECT RTRIM([字段]) FROM [表名];

-- 字符串长度（按字节）
SELECT LENGTH([字段]) FROM [表名];

-- 字符串长度（按字符）
SELECT CHAR_LENGTH([字段]) FROM [表名];

-- 转大写
SELECT UPPER([字段]) FROM [表名];

-- 转小写
SELECT LOWER([字段]) FROM [表名];

-- 替换指定字符串
SELECT REPLACE([字段], [旧内容], [新内容]) FROM [表名];
```

- 了解
- `CHARSET()`、`STRCMP()`、`INSTR()`、`LEFT()`/`RIGHT()`、`LPAD()`/`RPAD()`

- 数学函数

```sql
-- 四舍五入
SELECT ROUND([数值], [小数位数]) FROM [表名];

-- 数字格式化（带千分位，返回字符串）
SELECT FORMAT([数值], [小数位数]) FROM [表名];

-- 任意进制互转
SELECT CONV([数值/字符串], [原进制], [目标进制]) FROM [表名];

-- 向上取整（两种写法）
SELECT CEIL([数值]) FROM [表名];
SELECT CEILING([数值]) FROM [表名];

-- 向下取整
SELECT FLOOR([数值]) FROM [表名];

-- 取绝对值
SELECT ABS([数值]) FROM [表名];

-- 取模运算
SELECT MOD([被除数], [除数]) FROM [表名];

-- 生成 0~1 随机数
SELECT RAND() FROM [表名];

-- 求平方根
SELECT SQRT([数值]) FROM [表名];
```

- 了解
- `POW()`、`EXP()`、`LOG()`、`SIN()`、`COS()`、`TAN()`

- 日期时间函数

```sql
-- 获取当前日期时间
SELECT CURRENT_DATE() FROM DUAL;      -- 当前日期（YYYY-MM-DD）
SELECT CURRENT_TIME() FROM DUAL;      -- 当前时间（HH:MM:SS）
SELECT NOW() / CURRENT_TIMESTAMP() FROM DUAL; -- 当前完整日期时间

-- 提取日期时间 / 年月日时分秒
SELECT DATE([datetime]) FROM DUAL;    -- 提取日期（YYYY-MM-DD）
SELECT TIME([datetime]) FROM DUAL;    -- 提取时间（HH:MM:SS）
SELECT YEAR([datetime]), MONTH([datetime]), DAY([datetime]) FROM DUAL; -- 提取年/月/日
SELECT HOUR([datetime]), MINUTE([datetime]), SECOND([datetime]) FROM DUAL; -- 提取时/分/秒

-- 日期加减计算
SELECT DATE_ADD([date], INTERVAL [数值] [单位]) FROM DUAL;  -- 日期加
SELECT DATE_SUB([date], INTERVAL [数值] [单位]) FROM DUAL;  -- 日期减

-- 日期差 / 时间差
SELECT DATEDIFF([date1], [date2]) FROM DUAL;   -- 计算两个日期相差天数（忽略时间）
SELECT TIMEDIFF([time1], [time2]) FROM DUAL;   -- 计算两个时间差

-- 日期 ↔ 字符串 互逆转换
SELECT DATE_FORMAT([datetime], '[格式串]') FROM DUAL; -- 日期→格式字符串
SELECT STR_TO_DATE('[字符串]', '[格式串]') FROM DUAL;   -- 字符串→格式日期

-- 日期 ↔ 时间戳 互逆转换
SELECT UNIX_TIMESTAMP([datetime]) FROM DUAL;    -- 日期→Unix时间戳
SELECT FROM_UNIXTIME([时间戳]) FROM DUAL;        -- Unix时间戳→日期
```

- 了解
- `HOUR()`、`MINUTE()`、`SECOND()`、`DAYNAME()`、`MONTHNAME()`、`WEEK()`

- 流程控制函数

```sql
-- 条件判断
SELECT IF([条件], [成立值], [不成立值]) FROM [表名];

-- 空值替换
SELECT IFNULL([字段], [默认值]) FROM [表名];

-- 多条件分支判断
SELECT CASE WHEN [条件1] THEN [结果1] WHEN [条件2] THEN [结果2] ELSE [默认结果] END FROM [表名];

-- 空值判断
SELECT NULLIF([表达式1], [表达式2]) FROM [表名];
```

---

多表查询见 → [Joins & Subqueries (多表查询与子查询)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/02-Joins%20&%20Subqueries%20(多表查询与子查询).md) · [Aggregate Functions & Group By (聚合与分组)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/03-Aggregate%20Functions%20&%20Group%20By%20(聚合与分组).md)

---

# Joins and Subqueries (多表查询与子查询)

> [!note] 本节重点：核心考点：INNER/LEFT/RIGHT/FULL JOIN 区别与用法、子查询 EXISTS/IN、多表关联优化要点

# 联合查询

```sql
-- UNION：合并结果集并去重
SELECT [字段列表] FROM [表1] UNION SELECT [字段列表] FROM [表2];

-- UNION ALL：合并结果集不去重（推荐：性能更好）
SELECT [字段列表] FROM [表1] UNION ALL SELECT [字段列表] FROM [表2];
```

# 连接查询

```sql
-- 自连接：一张表自己和自己连接（取别名）
-- 场景：员工表查出每个员工的上级姓名
SELECT e1.name AS employee, e2.name AS manager
FROM emp e1 LEFT JOIN emp e2 ON e1.manager_id = e2.id;

-- 内连接：取两表交集数据
SELECT t1.字段, t2.字段 FROM 表1 t1
INNER JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 左外连接：左表全量 + 右表匹配项（无匹配为 NULL）
SELECT t1.字段, t2.字段 FROM 表1 t1
LEFT JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 右外连接：右表全量 + 左表匹配项（无匹配为 NULL）
SELECT t1.字段, t2.字段 FROM 表1 t1
RIGHT JOIN 表2 t2 ON t1.关联字段 = t2.关联字段;

-- 等值连接简化写法（两表关联字段名相同时）
SELECT * FROM 表1 JOIN 表2 USING(关联字段);
```

# 子查询

```sql
-- 标量子查询：返回单个值
SELECT * FROM 表1 WHERE 字段 = (SELECT 字段 FROM 表2 WHERE 条件);

-- 多行子查询：返回单列多行
SELECT * FROM 表1 WHERE 字段 IN (SELECT 字段 FROM 表2 WHERE 条件);

-- ANY 子查询：满足其一
SELECT * FROM 表1 WHERE 字段 > ANY (SELECT 字段 FROM 表2 WHERE 条件);

-- ALL 子查询：全部满足
SELECT * FROM 表1 WHERE 字段 > ALL (SELECT 字段 FROM 表2 WHERE 条件);

-- 表子查询（临时表）：必须加别名
SELECT * FROM (SELECT 字段, COUNT(*) FROM 表1 GROUP BY 分组) AS tmp WHERE cnt > 10;
```

# EXISTS vs IN 性能

```sql
-- EXISTS：找到第一个匹配即停止（半连接），适合外表小、内表大
SELECT * FROM orders o WHERE EXISTS (
    SELECT 1 FROM users u WHERE u.id = o.user_id AND u.level = 'vip'
);

-- IN：先执行子查询再匹配，适合外表大、子查询结果集小
SELECT * FROM orders WHERE user_id IN (
    SELECT id FROM users WHERE level = 'vip'
);

-- NOT EXISTS 通常优于 NOT IN（NOT IN 需要额外处理 NULL）
```

| 写法 | 执行逻辑 | 适合场景 |
|------|---------|---------|
| `IN (...)` | 先执行子查询生成临时表，再外层匹配 | 子查询结果集小 |
| `EXISTS (...)` | 对外层每行执行子查询，找到即停 | 外表小、子查询表大 |
| `JOIN` | 哈希/归并连接 | 关联字段有索引 |

# 经典题型速查

| 题型 | 要点 |
|------|------|
| JOIN vs 子查询 | JOIN 通常更快（优化器做连接顺序优化） |
| LEFT JOIN 条件陷阱 | 右表条件放 WHERE 会使 LEFT 退化为 INNER |
| EXISTS 优化原理 | Semi Join 半连接，找到匹配即停止扫描 |
| 多表 JOIN 顺序 | MySQL 优化器决定，不按书写顺序 |

> [!tip]- **工程要点**：多表查询关联字段必须有索引（EXPLAIN 看 type 列：`ref` 好于 `ALL`）。超过 3 张表 JOIN 考虑拆分 SQL 或冗余字段。

---

基础语法见 → [DDL, DML, DQL (SQL基础语法)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/01-DDL,%20DML,%20DQL%20(SQL基础语法).md) · [Aggregate Functions & Group By (聚合与分组)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/03-Aggregate%20Functions%20&%20Group%20By%20(聚合与分组).md)

---

# Aggregation and Grouping (聚合与分组)

> [!note] 本节重点：核心考点：COUNT/SUM/AVG/MAX/MIN 聚合函数、GROUP BY 分组与 HAVING 过滤、SQL 执行顺序

# 数据库操作

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

# 数据表操作

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

# 字段操作

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

## 1. 增

```sql
-- 插入单条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2);

-- 插入多条数据
INSERT INTO [表名](字段1, 字段2) VALUES (值1, 值2), (值3, 值4);

-- 插入数据（所有字段按顺序）
INSERT INTO [表名] VALUES (值1, 值2);
```

## 2. 删

```sql
-- 删除指定数据
DELETE FROM [表名] WHERE [条件];

-- 清空表所有数据
DELETE FROM [表名];
```

## 3. 改

```sql
-- 更新指定数据
UPDATE [表名] SET [字段1]=[值1] WHERE [条件];

-- 更新多条数据
UPDATE [表名] SET [字段1]=[值1], [字段2]=[值2] WHERE [条件];

-- 更新所有数据
UPDATE [表名] SET [字段1]=[值1];
```

## 4. 去重

```sql
-- 统计重复数据（找出重复值）
SELECT [字段名], COUNT(*) FROM [表名] GROUP BY [字段名] HAVING COUNT(*) > 1;

-- 删除重复数据（保留主键最小的一行）
DELETE t1 FROM [表名] t1 JOIN [表名] t2 ON t1.[重复字段] = t2.[重复字段] AND t1.[主键] > t2.[主键];

-- 删除重复数据（按条件删除指定重复值）
DELETE FROM [表名] WHERE [重复字段] = '重复值' AND [主键] <> (SELECT MIN([主键]) FROM [表名] WHERE [重复字段] = '重复值');
```

## 5. 复制

```sql
-- 只复制表结构
CREATE TABLE [新表名] LIKE [原表名];

-- 复制表结构 + 复制所有数据
CREATE TABLE [新表名] LIKE [原表名];
INSERT INTO [新表名] SELECT * FROM [原表名];
```

---

基础语法见 → [DDL, DML, DQL (SQL基础语法)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/01-DDL,%20DML,%20DQL%20(SQL基础语法).md) · [Joins & Subqueries (多表查询与子查询)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/02-Joins%20&%20Subqueries%20(多表查询与子查询).md)

---

# MySQL Overview (MySQL 基础)

> [!note] 本节重点：核心考点：MySQL 安装配置与目录结构、系统数据库功能、连接管理与状态查询

> [!warning] **平台/版本说明（NEEDS_VERIFY）**：以下路径以 **Windows 本机 MySQL 8.0** 为例，路径含版本号 `MySQL Server 8.0`，升级版本后需同步更新。Linux（vm-ubuntu）下的对应关系：程序 `mysqld`/`mysql`、配置文件 `/etc/mysql/mysql.conf.d/mysqld.cnf`、数据目录 `datadir`（用 `SHOW VARIABLES LIKE 'datadir'` 查询）、服务管理用 `systemctl` 而非 Windows 服务。

# 核心配置与目录

## 程序安装目录

- `C:\Program Files\MySQL\MySQL Server 8.0\`

- 子目录
    - `bin/`：`mysqld.exe` 服务端、`mysql.exe` 客户端等可执行文件
    - `include/` & `lib/`：开发相关头文件和库文件

## my.ini 配置文件

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

## data 数据目录

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

# 系统自带数据库

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

# MySQL 连接与退出

```sql
Test-NetConnection [主机IP] -Port 3306

mysql -h [主机IP] -P [端口号] -u [用户名] -p

```

# 配置与状态查询

```sql
show variables like 'port';

show variables like 'character_set_server';

show variables like 'datadir';

```

---

SQL基础语法详解见 → [DDL, DML, DQL (SQL基础语法)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/01-DDL,%20DML,%20DQL%20(SQL基础语法).md)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-SQL Fundamentals (SQL 基础)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
