> **核心考点**：索引选择性与区分度、联合索引最左前缀、索引覆盖与回表、索引下推优化
# 基础概念

- 索引是数据库中**加速查询**的一种数据结构，相当于表的「目录」
- 查询时间复杂度从 `O(n)` 降低到 `O(log n)`
- 通过预排序，减少查询时的全表扫描次数
- 占用额外存储空间；增删改操作需要维护索引，性能会有轻微损耗

```sql
-- 查看表中所有索引
SHOW INDEX FROM [表名];
```

---

# 索引类型

- 主键索引（Primary Key Index）
	
	- 基于主键自动创建，一张表只能有一个 
	- 天然具备 `NOT NULL + UNIQUE` 约束 
	- InnoDB 中为聚簇索引，决定数据物理存储顺序

```sql
-- 创建表时定义主键索引（自动创建聚簇索引）
CREATE TABLE [表名] (
    id INT PRIMARY KEY AUTO_INCREMENT
);

-- 表创建完成后添加主键索引
ALTER TABLE [表名] ADD   ;
```

- 唯一索引（Unique Index）
	
	- 字段值不可重复（允许一个 NULL）
	- 一张表可建多个，非聚簇索引

```sql
-- 创建表时定义唯一索引
CREATE TABLE [表名] (
    phone VARCHAR(11) UNIQUE
);

-- 表创建完成后添加唯一索引
ALTER TABLE [表名] ADD UNIQUE INDEX idx_phone(phone);
```

- 普通索引（Normal Index）
	
	- 基于主键自动创建，一张表只能有一个 
	- 天然具备 `NOT NULL + UNIQUE` 约束 
	- InnoDB 中为聚簇索引，决定数据物理存储顺序

```sql
-- 创建表时定义普通索引
CREATE TABLE [表名] (
    name VARCHAR(50),
    INDEX idx_name(name)
);

-- 表创建完成后添加普通索引
ALTER TABLE [表名] ADD INDEX idx_name(name);
```

- 复合索引（联合索引）
	
	- 由多个字段组合而成的索引，遵循**最左匹配原则**
	- 适用于多条件查询场景，可覆盖多个查询条件

```sql
-- 创建复合索引
CREATE TABLE [表名] (
    user_id INT,
    goods_id INT,
    INDEX idx_user_goods(user_id, goods_id)
);

-- 表创建完成后添加复合索引
ALTER TABLE [表名] ADD INDEX idx_user_goods(user_id, goods_id);
```

- 全文索引（FULLTEXT）
	
	- 用于文本字段的模糊查询（`MATCH AGAINST`）
	- 支持中文分词，比 `LIKE '%xxx%'` 效率高很多

```sql
-- 创建全文索引（MySQL 5.6+ InnoDB 支持）
CREATE TABLE [表名] (
    content TEXT,
    FULLTEXT INDEX idx_content(content)
);

-- 全文索引查询
SELECT * FROM [表名] WHERE MATCH(content) AGAINST('关键词');
```

---

# InnoDB 索引底层原理（B + 树）

- 索引的底层数据结构为 **B + 树**，所有数据都存储在叶子节点
- 聚簇索引：叶子节点直接存储完整的行数据，数据按主键顺序物理存储
- 非聚簇索引：叶子节点存储主键值，查询时需通过主键回表获取完整数据
- 特点：非叶子节点仅存储索引值，不存储数据，树的高度低，查询效率稳定

---

# 索引设计原则（面试高频）

- 优先为 **WHERE、JOIN、ORDER BY、GROUP BY** 条件中的字段创建索引
- 复合索引遵循**最左匹配原则**，将区分度高的字段放在前面
- 避免在低基数列（如性别、状态）上创建索引，优化效果几乎为 0
- 索引字段类型尽量小，优先使用 `INT` 而非 `VARCHAR`
- 避免创建过多索引，增删改操作的维护成本会显著增加

---

# 索引失效场景（必背）

- 使用 `OR` 连接条件，且非所有条件都包含索引字段
- 索引列参与运算、使用函数（如 `DATE(create_time)`）
- 隐式类型转换（如 `VARCHAR` 字段和数字比较）
- 使用 `LIKE '%xxx'` 或 `LIKE '%xxx%'`，无法利用索引
- 范围查询（`>、<、BETWEEN`）会导致复合索引后续字段失效

---

# 索引相关 SQL 操作

```sql
-- 1. 创建索引（表已存在）
ALTER TABLE [表名] ADD INDEX idx_xxx([字段名]);
ALTER TABLE [表名] ADD UNIQUE INDEX idx_xxx([字段名]);
ALTER TABLE [表名] ADD PRIMARY KEY (id);

-- 2. 删除索引
ALTER TABLE [表名] DROP INDEX idx_xxx;
ALTER TABLE [表名] DROP PRIMARY KEY; -- 删除主键索引

-- 3. 查看索引使用情况（分析查询语句）
EXPLAIN SELECT * FROM [表名] WHERE [条件];
```

---

# 索引优化实践

```sql
-- 查看查询执行计划，判断索引是否生效
EXPLAIN SELECT * FROM `purchase` WHERE customer_id = 1 AND goods_id = 100;

-- 为高频查询条件创建复合索引
ALTER TABLE `purchase` ADD INDEX idx_customer_goods(customer_id, goods_id);
```
