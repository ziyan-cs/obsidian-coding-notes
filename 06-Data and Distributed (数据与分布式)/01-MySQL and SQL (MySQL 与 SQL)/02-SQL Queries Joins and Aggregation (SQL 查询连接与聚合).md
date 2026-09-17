---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习目标：从 SELECT 执行目标出发组织过滤、连接、子查询、分组与聚合。

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

多表查询见 → Joins & Subqueries (多表查询与子查询) · Aggregate Functions & Group By (聚合与分组)

---

# Joins and Subqueries (多表查询与子查询)

> [!note] 本节重点：INNER/LEFT/RIGHT/FULL JOIN 区别与用法、子查询 EXISTS/IN、多表关联优化要点

## 联合查询

```sql
-- UNION：合并结果集并去重
SELECT [字段列表] FROM [表1] UNION SELECT [字段列表] FROM [表2];

-- UNION ALL：合并结果集不去重（推荐：性能更好）
SELECT [字段列表] FROM [表1] UNION ALL SELECT [字段列表] FROM [表2];
```

## 连接查询

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

## 子查询

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

## EXISTS vs IN 性能

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

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | JOIN vs 子查询 | JOIN 通常更快（优化器做连接顺序优化） |
> | LEFT JOIN 条件陷阱 | 右表条件放 WHERE 会使 LEFT 退化为 INNER |
> | EXISTS 优化原理 | Semi Join 半连接，找到匹配即停止扫描 |
> | 多表 JOIN 顺序 | MySQL 优化器决定，不按书写顺序 |
>
> > [!tip]- **工程要点**：多表查询关联字段必须有索引（EXPLAIN 看 type 列：`ref` 好于 `ALL`）。超过 3 张表 JOIN 考虑拆分 SQL 或冗余字段。
>
> ---
>
>
> ---
