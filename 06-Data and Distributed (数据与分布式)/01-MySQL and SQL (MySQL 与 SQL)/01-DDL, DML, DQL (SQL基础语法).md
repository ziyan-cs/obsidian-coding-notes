---
tags:
  - database/sql
status: 🌱
---

> [!abstract] 核心考点：DDL 数据库/表/字段操作、DML 增删改、DQL 基础查询/条件过滤/聚合/分组/排序分页
# SQL Fundamentals — SQL 基础语法

## DDL（数据定义）
### 1. 数据库操作

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

### 2. 数据表操作

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

### 3. 字段操作

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

## DML（数据操作）

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

## DQL（数据查询）

### 1. 基础查询

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

### 2. 条件过滤

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

### 3. 聚合函数

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

### 4. 分组查询

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

### 5. 排序与分页

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

### 6. 常用内置函数

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
