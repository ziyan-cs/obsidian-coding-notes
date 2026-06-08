---
tags:
  - database/innodb
status: 🌱
---

> [!important] **核心考点**：间隙锁解决幻读、Next-Key Lock 行锁+间隙锁组合、临键锁对 RR 级别的保障

## 为什么需要 Gap Lock

Gap Lock 解决**幻读**问题——在 RR 级别下防止其他事务插入新行：

```
场景：用户表，id 为主键 1, 5, 10

事务 A:
  SELECT * FROM user WHERE id > 3 FOR UPDATE;
  → 期望只查到 id=5, id=10
  → 如果不加 Gap Lock，事务 B 可以 INSERT INTO user(id=4)

事务 B:
  INSERT INTO user(id=4, name='Bob');
  → 如果没有 Gap Lock，插入成功
  → 事务 A 再次 SELECT → 出现幻读(id=4)
```

## Gap Lock 的工作原理

Gap Lock 锁的是**索引记录之间的间隙**，而不是记录本身：

```
索引记录：1, 5, 10, 20

间隙：
  (-∞, 1)    记录 1 之前的间隙
  (1, 5)     记录 1 和 5 之间的间隙 ← Gap Lock 锁这个范围
  (5, 10)    记录 5 和 10 之间的间隙
  (10, 20)   记录 10 和 20 之间的间隙
  (20, +∞)   记录 20 之后的间隙

SELECT * FROM user WHERE id > 3 FOR UPDATE;
  → 锁住 (1, 5), (5, 10), (10, 20), (20, +∞)
  → 所有 id > 1 的间隙全部被锁
  → 其他事务无法插入任何 id > 1 的新行
```

**Gap Lock 的特点：**
- 只锁间隙，不锁记录本身
- 不同事务的 Gap Lock 可以共存（间隙锁之间不冲突）
- Gap Lock 只在 RR 和 Serializable 级别生效

## Next-Key Lock（临键锁）

Next-Key Lock = **Record Lock（行锁）+ Gap Lock（间隙锁）**。InnoDB 默认的锁机制。

```
索引：1, 5, 10, 20

Next-Key Lock 锁定的范围是左开右闭区间：
  (-∞, 1]   锁定 ≤1 的范围
  (1, 5]    锁定 (1,5] 的范围 ← 包含行 5 和之前的间隙
  (5, 10]   锁定 (5,10] 的范围
  (10, 20]  锁定 (10,20] 的范围
  (20, +∞)  锁定 >20 的范围（实际是 supremum 伪记录）
```

**举例：**
```sql
-- RR 级别下
SELECT * FROM user WHERE id = 5 FOR UPDATE;

-- Next-Key Lock 锁住的范围：(1, 5]
-- 包含两部分：
--   1. Record Lock：锁住 id=5 的记录（阻止其他事务修改/删除）
--   2. Gap Lock：锁住 (1,5) 间隙（阻止其他事务插入 id=2,3,4）
-- 
-- 不锁 (<1) 的间隙：其他事务可以在 id<1 的范围内插入（如 id=0）
```

## 唯一索引的特殊优化

当 WHERE 条件命中**唯一索引**时，Next-Key Lock 退化为 **Record Lock**：

```sql
-- id 是主键（唯一索引）
SELECT * FROM user WHERE id = 5 FOR UPDATE;

-- InnoDB 知道 id=5 是唯一记录，不需要锁间隙来防止幻读
-- 退化为 Record Lock，只锁 id=5 这一行
-- (1,5) 的间隙不被锁，可以插入 id=3

-- 对比：范围查询不会退化
SELECT * FROM user WHERE id > 5 FOR UPDATE;
-- Next-Key Lock 完整生效，锁住 (5, +∞)
```

## Gap Lock 导致的性能问题

Gap Lock 是 RR 级别下锁争用的常见原因：

```
问题场景：
  事务 A: SELECT * FROM t WHERE id > 10 FOR UPDATE;  -- 锁了大量间隙
  事务 B: INSERT INTO t(id=11) VALUES(...);          -- 被 Gap Lock 阻塞！
  事务 C: INSERT INTO t(id=50) VALUES(...);          -- 也被阻塞！

解决方案：
  1. 缩小 WHERE 范围（如 id BETWEEN 11 AND 20）
  2. 如果业务允许，使用 RC 隔离级别 + ROW 格式 binlog
  3. 在代码层面做唯一性校验而非依赖 Gap Lock
```

## 锁升级路径

```
索引类型     | 查询类型    | 锁类型
唯一索引     | 等值查询    | Record Lock
唯一索引     | 范围查询    | Next-Key Lock
普通索引     | 任何查询    | Next-Key Lock
无索引       | 任何查询    | 全表锁（逐行加 Next-Key Lock）
```

> [!tip]- **工程要点**：Gap Lock 的锁定范围比想象的大——一个 WHERE 条件可能会锁住大半个索引树。典型故障：RR 级别下一个大范围 UPDATE 导致整个表无法插入新数据。排查手段：`SELECT * FROM performance_schema.data_locks` 查看具体锁范围。如果业务不要求 RR 的可重复读，使用 RC 级别可以完全避免 Gap Lock。

---



表锁与行锁基础见 → [Table Lock vs Row Lock (表锁与行锁)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06a-Table%20Lock%20vs%20Row%20Lock%20(表锁与行锁).md) · [Deadlock Detection & avoidance (死锁检测)](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06c-Deadlock%20Detection%20&%20avoidance%20(死锁检测).md)
