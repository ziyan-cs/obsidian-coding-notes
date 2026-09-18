---
status: stable
confidence: high
content_verified: 2026-09-18
---

> [!abstract] 学习目标：掌握表结构定义、约束以及 INSERT、UPDATE、DELETE 的数据变更语义。

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

> [!summary] 核心摘要
>
> SQL 是声明式语言，先描述结果集合，再由优化器选择执行计划。正确性依赖表结构、约束和事务边界；性能要结合索引、扫描行数和真实数据分布验证。

# 基础语法速查

> [!note] 本节重点：DDL 数据库/表/字段操作、DML 增删改、DQL 基础查询/条件过滤/聚合/分组/排序分页

# DDL：结构与约束
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

# DML：集合变更

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
# 用约束表达业务不变量

表结构不是字段容器，而是数据库能够持续执行的一组不变量。优先让数据库拒绝不合法状态，而不是只依赖应用代码“记得检查”。

```sql
CREATE TABLE account (
    id           BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    email        VARCHAR(254) NOT NULL,
    balance_cent BIGINT NOT NULL DEFAULT 0,
    status       VARCHAR(16) NOT NULL DEFAULT 'active',
    created_at   TIMESTAMP(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
    PRIMARY KEY (id),
    UNIQUE KEY uk_account_email (email),
    CONSTRAINT chk_balance_nonnegative CHECK (balance_cent >= 0),
    CONSTRAINT chk_account_status CHECK (status IN ('active', 'frozen', 'closed'))
) ENGINE = InnoDB;
```

- `NOT NULL` 表达“缺失不合法”；不要用空字符串冒充未知值。
- `DEFAULT` 只在未提供列值时生效，不等于数据修复策略。
- `UNIQUE`、外键与 `CHECK` 应对应真实业务规则；同时理解 MySQL 版本与存储引擎对约束的支持。
- 金额优先使用最小货币单位的整数或精确 `DECIMAL`，不要用浮点数承担财务等值判断。
- 字符集和 collation 会影响可表示字符、排序和唯一性；应在建模阶段确定，而非出现乱码后补救。

# 安全的数据修改流程

`UPDATE` 和 `DELETE` 的风险不在语法，而在集合范围。执行生产变更时使用同一谓词先读取、估算影响行数，再在事务中修改并验证：

```sql
START TRANSACTION;

SELECT id, status
FROM account
WHERE status = 'frozen' AND created_at < '2025-01-01'
ORDER BY id
LIMIT 100
FOR UPDATE;

UPDATE account
SET status = 'closed'
WHERE status = 'frozen' AND created_at < '2025-01-01'
ORDER BY id
LIMIT 100;

SELECT ROW_COUNT() AS affected_rows;
-- 核对结果后 COMMIT；异常则 ROLLBACK
```

真实批处理还要考虑锁等待、复制延迟、审计、重试幂等性和分批游标。不要把 `LIMIT` 当作完整安全保证：没有稳定排序或断点记录时，重试可能漏处理或重复处理。

# Schema 迁移不是一次 DDL

向线上系统加列、索引或约束时，先回答：

1. 旧应用与新 Schema 是否双向兼容？
2. 操作会持有什么 metadata lock，是否复制整表，磁盘空间是否足够？
3. 回填如何限速、断点续跑和验证？
4. 失败如何回滚；回滚是否同样昂贵？
5. 读写流量、复制延迟和错误率用什么门槛控制发布？

常见安全顺序是 expand-and-contract：先添加兼容结构，再发布同时支持新旧结构的应用，完成回填与核验，切换读路径，最后删除旧结构。具体 DDL 能否 online/in-place 取决于 MySQL 版本、表结构和操作类型，必须在同版本测试环境用官方文档与实际执行计划确认。

# 建模与验证练习

为“订单—订单项—支付尝试”设计表结构，明确主键、业务唯一键、外键策略、金额类型和状态约束。然后构造重复请求、空值、并发更新与迁移回填，证明非法状态会被拒绝，合法重试不会重复扣款。

# 参考资料

- [MySQL 8.4 Reference Manual: Data Definition Statements](https://dev.mysql.com/doc/refman/8.4/en/sql-data-definition-statements.html)
- [MySQL 8.4 Reference Manual: InnoDB and Online DDL](https://dev.mysql.com/doc/refman/8.4/en/innodb-online-ddl.html)
- [MySQL 8.4 Reference Manual: Transactional and Locking Statements](https://dev.mysql.com/doc/refman/8.4/en/sql-transactional-statements.html)
