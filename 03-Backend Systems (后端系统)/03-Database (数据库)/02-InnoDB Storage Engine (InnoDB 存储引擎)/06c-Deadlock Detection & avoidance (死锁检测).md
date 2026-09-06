---
tags:
  - database/innodb
status: 🌱
---

# Deadlock Detection & avoidance — 死锁检测

> [!important] **核心考点**：死锁检测机制（等待图）、InnoDB 死锁处理策略（回滚代价较小的事务）、预防死锁方法

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

## 30 秒回答

**死锁怎么处理？** 数据库发现等待环后会回滚一个事务；应用应捕获可重试错误、先回滚，再对幂等操作做带退避的有限重试。根治方向是统一加锁顺序、缩短事务并减少无谓的锁扫描，而不是盲目提高超时。

---



表锁与行锁基础见 → [Table Lock vs Row Lock (表锁与行锁)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06a-Table%20Lock%20vs%20Row%20Lock%20(表锁与行锁).md) · [Gap Lock & Next-Key Lock (间隙锁与临键锁)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06b-Gap%20Lock%20&%20Next-Key%20Lock%20(间隙锁与临键锁).md)
