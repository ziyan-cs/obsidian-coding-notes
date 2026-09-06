---
tags:
  - database/innodb
status: 🌱
---

# Table Lock vs Row Lock — 表锁与行锁

> [!important] **核心考点**：表锁与行锁的开销与并发粒度对比、InnoDB 行锁基于索引实现、意向锁的作用

## 表锁 vs 行锁

| 特性 | 表锁（Table Lock） | 行锁（Row Lock） |
|------|-------------------|-----------------|
| 锁定粒度 | 整张表 | 单行记录 |
| 并发度 | 低（锁住整表） | 高（不同行不冲突） |
| 加锁开销 | 小 | 大（维护每行锁信息） |
| 死锁概率 | 低（不会交叉） | 高（行间循环等待） |
| MySQL 引擎 | MyISAM、InnoDB | 仅 InnoDB |
| DDL 操作 | 使用表锁 | 使用表锁（如 ALTER TABLE）|

**MyISAM vs InnoDB 锁差异：**
```
MyISAM：只支持表锁
  SELECT：共享读锁（不影响其他读，阻塞写）
  INSERT/UPDATE/DELETE：排他写锁（阻塞一切）

InnoDB：行锁 + 表锁
  SELECT：MVCC 快照读，不加锁（默认）
  SELECT ... FOR UPDATE：行级排他锁
  SELECT ... LOCK IN SHARE MODE：行级共享锁
  UPDATE/DELETE：行级排他锁（自动）
```

## InnoDB 行锁类型

```sql
-- 共享锁（S Lock）：允许其他事务读，阻塞写
SELECT * FROM t WHERE id = 1 LOCK IN SHARE MODE;

-- 排他锁（X Lock）：阻塞其他事务的读和写
SELECT * FROM t WHERE id = 1 FOR UPDATE;
UPDATE t SET val = 10 WHERE id = 1;   -- 自动加 X 锁
DELETE FROM t WHERE id = 1;            -- 自动加 X 锁

-- 兼容性：
--    S 和 S 兼容：两个事务可以同时持有同一行的 S 锁
--    S 和 X 互斥
--    X 和 X 互斥
```

## 行锁基于索引

InnoDB 的行锁不是锁"行"，而是锁**索引记录**。

```
场景：表 t 有主键索引 id 和普通索引 name

UPDATE t SET val = 10 WHERE id = 1;
  → 锁住 id=1 的聚簇索引记录（通过主键定位）

UPDATE t SET val = 10 WHERE name = 'Bob';
  → 锁住二级索引 name='Bob' + 对应的聚簇索引记录

UPDATE t SET val = 10 WHERE name = 'Bob' AND age = 20;
  → 如果没有索引，锁住整张表的所有行（退化为表锁！）
```

**关键工程结论：**
- **没有索引的 WHERE 条件 → 行锁升级为表锁**（所有行都被锁定）
- 通过二级索引加锁时，InnoDB 还需要锁住对应的聚簇索引记录
- 索引设计直接影响锁的粒度

## 意向锁（Intention Lock）

意向锁是**表级锁**，用于快速判断表中是否有行级锁，避免逐行检查。

```
事务 A 锁了行 1（行级 X 锁）：
  行 1: X 锁
  表: 意向排他锁（IX）← 自动添加

事务 B 想锁整张表（表级 X 锁）：
  检查到表上已有 IX → 立即知道有行被锁 → 等待

如果不有意向锁：
  事务 B 需要检查表中每一行 → O(n) 开销
```

**意向锁兼容性：**

| 锁类型 | IS | IX | S（表） | X（表） |
|--------|----|----|---------|---------|
| IS（意向共享） | 兼容 | 兼容 | 兼容 | 互斥 |
| IX（意向排他） | 兼容 | 兼容 | 互斥 | 互斥 |
| S（表共享） | 兼容 | 互斥 | 兼容 | 互斥 |
| X（表排他） | 互斥 | 互斥 | 互斥 | 互斥 |

## 行锁的实现开销

```
行锁的内存结构（lock_t）：
  - 事务 ID
  - 锁类型（S/X）
  - 锁模式（Record/Gap/Next-Key）
  - 索引表空间 ID + 页号
  - 堆栈号（heap_no，定位到行）

每个事务的行锁数量受 innodb_lock_wait_timeout 约束（默认 50s）
每行锁内存开销约 20-40 字节
```

```sql
-- 查看当前锁信息
SELECT * FROM performance_schema.data_locks\G
-- 查看锁等待
SELECT * FROM performance_schema.data_lock_waits\G
```

> [!tip]- **工程要点**：InnoDB 的行锁只有通过索引才能生效——没有索引的 WHERE 条件会退化为表锁，这是性能灾难的常见原因。大表 DELETE/UPDATE 操作尤其需要注意：一次操作影响 1 万行的范围更新会在短时间内加大量行锁，可能耗尽锁内存或引发大量锁等待。建议分批处理（如 LIMIT 1000 循环）。

---



间隙锁与临键锁详解见 → [Gap Lock & Next-Key Lock (间隙锁与临键锁)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06b-Gap%20Lock%20&%20Next-Key%20Lock%20(间隙锁与临键锁).md) · [Deadlock Detection & avoidance (死锁检测)](/06-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06c-Deadlock%20Detection%20&%20avoidance%20(死锁检测).md)
