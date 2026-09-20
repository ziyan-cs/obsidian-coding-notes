---
study_stage: backlog
---

> [!summary] 核心摘要
>
> InnoDB 的行锁锁的是索引记录；范围当前读在 RR 下可能使用 gap lock 或 next-key lock 保护区间。死锁不是靠等待自行解决，数据库会检测环并回滚一个事务，应用必须缩短事务、统一加锁顺序并正确重试。

# 表锁、行锁与死锁

> [!note] 本节重点：表锁与行锁的开销与并发粒度对比、InnoDB 行锁基于索引实现、意向锁的作用

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


---

# Gap and Next Key Locks (间隙锁与临键锁)

> [!note] 本节重点：间隙锁解决幻读、Next-Key Lock 行锁+间隙锁组合、临键锁对 RR 级别的保障

## 为什么需要 Gap Lock

Gap Lock 解决**幻读**问题——在 RR 级别下防止其他事务插入新行：

> [!warning] 锁范围不是按这页图示机械推导
> 实际 record/gap/next-key 锁取决于隔离级别、语句是否为 locking read、索引是否命中、唯一性、范围边界和 MySQL 版本。图示用于理解区间模型；排障必须查看 `performance_schema.data_locks` 与实际执行计划。

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
- 在 MySQL 8.x 的 `READ COMMITTED` 下，普通搜索和索引扫描通常不使用 gap locking，但外键约束检查与重复键检查仍可能使用；RR 下的锁定读、UPDATE、DELETE 通常使用 next-key locking，精确命中完整唯一索引时可退化为 record lock

官方规则以 [InnoDB Locking](https://dev.mysql.com/doc/refman/8.4/en/innodb-locking.html) 为准；具体加锁范围还取决于隔离级别、访问索引和扫描范围。

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

> [!tip]- **工程要点**：范围条件可能锁住比业务直觉更大的索引区间。排查时用 `performance_schema.data_locks`、事务信息和执行计划确认实际锁范围；切换 RC 可能减少部分 gap locking，但并不保证所有场景都没有 gap lock，须结合当前版本与约束验证。

> [!summary] 核心摘要
> - **常见误区**：以为唯一索引查询一定会退化（范围查询不退化）；以为 Gap Lock 锁的是记录本身（实际锁间隙，所以同间隙可共存）；忽略"无索引"导致全表逐行加锁。
> - **自测**：1) 为什么唯一索引等值查询能退化为 Record Lock？ 2) 索引 1,5,10 上 `WHERE id=5 FOR UPDATE` 的 Next-Key Lock 锁哪些范围？
>
> ---
>
>
> ---

# Deadlock Detection and Avoidance (死锁检测与避免)

> [!note] 本节重点：死锁检测机制（等待图）、InnoDB 死锁处理策略（回滚代价较小的事务）、预防死锁方法

## 死锁的产生条件

死锁需要满足四个必要条件（CoFF 条件）：
1. **互斥**：资源一次只能被一个事务占用
2. **持有并等待**：事务持有锁的同时等待其他锁
3. **不可剥夺**：锁只能由持有者释放
4. **循环等待**：事务之间形成等待环

```
MySQL 典型死锁场景：

事务 A:                             事务 B:
UPDATE t SET val=1 WHERE id=1;      UPDATE t SET val=2 WHERE id=2;
UPDATE t SET val=1 WHERE id=2;      UPDATE t SET val=2 WHERE id=1;
                                    ↑
          事务 A 持有 id=1 的锁，等待 id=2
          事务 B 持有 id=2 的锁，等待 id=1
          → 形成循环等待 → 死锁！
```

## InnoDB 死锁检测

InnoDB 通过**等待图（Wait-for Graph）** 检测死锁：

```
等待图：
  节点 = 事务
  边 T1 → T2 = T1 等待 T2 释放锁

  例子：
    T1 → T2
    ↑     ↓
    └─────┘    ← 形成环 → 死锁
```

**检测机制：**
- 每次事务请求锁且需要等待时，InnoDB 检测是否形成环
- 如果在等待图中发现环 → 死锁
- InnoDB 选择**回滚代价最小的事务**（undo log 较少的那个）作为牺牲品

## 死锁信息查看

```sql
-- 查看最近一次死锁信息
SHOW ENGINE INNODB STATUS\G

-- 输出示例（LATEST DETECTED DEADLOCK 部分）：
------------------------
LATEST DETECTED DEADLOCK
------------------------
2024-01-01 12:00:00 0x7f1234
*** (1) TRANSACTION:
TRANSACTION 12345, ACTIVE 10 sec
mysql tables in use 1, locked 1
LOCK WAIT 2 lock struct(s)
TABLE: `db`.`t`, index: PRIMARY
*** (1) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 10 page no 3 n bits 72
*** (2) TRANSACTION:
TRANSACTION 12346, ACTIVE 5 sec
*** (2) HOLDS THE LOCK(S):
RECORD LOCKS space id 10 page no 3 n bits 72
*** (2) WAITING FOR THIS LOCK TO BE GRANTED:
RECORD LOCKS space id 10 page no 3 n bits 72
*** WE ROLL BACK TRANSACTION (2)  ← InnoDB 选择了事务 2 回滚
```

## 死锁后的事务处理

```sql
-- 应用层死锁重试
int retry_count = 3;
while (retry_count > 0) {
    try {
        // 执行事务
        conn->begin();
        conn->execute("UPDATE t SET val=1 WHERE id=1");
        conn->execute("UPDATE t SET val=2 WHERE id=2");
        conn->commit();
        break;  // 成功
    } catch (DeadlockException &e) {
        retry_count--;
        if (retry_count == 0) throw;
        // 回滚后重试
        conn->rollback();
        usleep(100 * (rand() % 10));  // 随机延迟，减少再次死锁概率
    }
}
```

## 死锁预防策略

**1. 统一加锁顺序**
```sql
-- 不好的做法（A 先锁 id=1，B 先锁 id=2）
事务 A: UPDATE t SET val=1 WHERE id=1; UPDATE t SET val=1 WHERE id=2;
事务 B: UPDATE t SET val=2 WHERE id=2; UPDATE t SET val=2 WHERE id=1;

-- 好的做法（所有事务按 id 升序加锁）
事务 A: UPDATE t SET val=1 WHERE id=1; UPDATE t SET val=2 WHERE id=2;
事务 B: UPDATE t SET val=2 WHERE id=1; UPDATE t SET val=2 WHERE id=2;
-- 先锁小 id，再锁大 id → 不会形成循环等待
```

**2. 减少锁持有时间**
```sql
-- 不好的做法：事务中做慢查询
BEGIN;
SELECT * FROM t WHERE ... FOR UPDATE;  -- 加锁
-- ... 复杂的业务计算，耗时 1 秒 ...
UPDATE t SET val=1 WHERE id=1;         -- 锁被长时间持有
COMMIT;

-- 好的做法：先计算再加锁
-- ... 业务计算 ...
BEGIN;
UPDATE t SET val=1 WHERE id=1;         -- 加锁后立即提交
COMMIT;
```

**3. 按语义评估隔离级别**
- RC 在许多普通范围操作中会减少 gap locking，但外键、重复键检查等场景仍可能涉及间隙锁
- 是否使用 RC、以及复制格式，取决于一致性语义、当前版本和实际死锁模式

**4. 合理设计索引**
- 确保 UPDATE/DELETE 的 WHERE 条件有合适索引，否则会扫描并锁住大量记录，扩大冲突范围
- InnoDB 不会因为“未走索引”自动把行锁升级成传统表锁；表锁同样可能参与死锁

## 死锁监控

```sql
-- 开启死锁日志
SET GLOBAL innodb_print_all_deadlocks = ON;
-- 死锁信息写入 MySQL 错误日志

-- 监控锁等待超时
SHOW VARIABLES LIKE 'innodb_lock_wait_timeout';
-- 默认 50 秒，超过自动回滚

-- 查看当前锁等待
SELECT * FROM sys.innodb_lock_waits\G
```

> [!tip]- **工程要点**：死锁是并发写入中需要设计处理的正常失败路径，但重试次数、退避策略和是否可安全重试必须由业务幂等性决定。统一锁顺序、缩短事务、优化索引能降低概率；检测代价和响应时间取决于负载与锁图，不能承诺“毫秒级”。
