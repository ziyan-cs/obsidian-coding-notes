---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 08-Sharding and Partitioning (分库分表)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## Sharding and Partitioning (分库分表)

> [!abstract] 核心考点：分库分表策略（水平/垂直拆分）、Sharding Key 选择、跨节点查询与分布式事务问题

## 何时需要分库分表

当单库/单表达到性能瓶颈时考虑分片：

```
应根据查询延迟、写入放大、索引/数据大小、CPU/IO、容量增长和运维恢复目标综合判断；不存在统一的“行数 / QPS / 容量阈值”。
```

## 垂直拆分

**垂直分库：** 按业务模块拆分到不同数据库。

```
垂直分库前：
  所有表在同一个库：user, order, product, payment

垂直分库后：
  User 库：user, user_address
  Order 库：order, order_item
  Product 库：product, category
  Payment 库：payment, refund

优点：业务隔离，每个库独立扩展
缺点：跨库 JOIN 复杂，分布式事务
```

**垂直分表：** 将大表按列拆分为宽表和窄表。

```
垂直分表前：
  user(id, name, age, avatar_url, bio, last_login_ip, last_login_time, ...40 列)

垂直分表后：
  user_base(id, name, age)                    ← 频繁访问的列
  user_profile(id, avatar_url, bio)           ← 低频访问的列
  user_login(id, last_login_ip, last_login_time) ← 登录相关列

优点：减少单行宽度，一页存更多行，减少 IO
```

## 水平拆分

**水平分表：** 按某种规则将数据行分散到多个表/库中。

```
拆分前：order 表（1 亿行）

拆分后（按 user_id % 16）：

  order_0   ← user_id % 16 == 0
  order_1   ← user_id % 16 == 1
  ...
  order_15  ← user_id % 16 == 15

或者分到不同库：
  db_order_0.order_0 ~ order_7
  db_order_1.order_0 ~ order_7
```

## 分片策略

| 策略 | 方法 | 优点 | 缺点 | 适用场景 |
|------|------|------|------|---------|
| 哈希取模 | `key % N` | 数据分布均匀 | 扩缩容需要重新分布 | 用户 ID 分片 |
| 范围分片 | 按 ID 范围 | 扩缩容简单 | 数据可能倾斜 | 时间序列数据 |
| 一致性哈希 | Hash 环 | 扩缩容影响小 | 实现复杂 | 缓存分片 |
| 地理分片 | 按地域 | 数据本地化 | 分布不均 | 国际化业务 |

**Sharding Key 选择原则：**
```
好的 Sharding Key：
  - Most Queries 都带该字段 → 避免跨节点查询
  - 数据分布均匀 → 避免数据倾斜
  - 不可变 → Sharding Key 修改成本极高

常见选择：
  用户 ID（user_id） → 查询都带用户上下文
  商家 ID（shop_id）→ 电商场景
  时间（按月/日）   → 日志/流水数据
```

## 跨节点查询问题

**1. 跨节点 JOIN：**
```sql
-- 无法在数据库层面 JOIN
-- 方案：应用层做 multiple queries + merge
List<User> users = orderDb.query("SELECT user_id FROM order_0 WHERE ...");
List<Long> userIds = users.stream().map(u -> u.userId).toList();
Map<Long, User> userMap = userDb.query("SELECT * FROM user_0 WHERE id IN ?", userIds);
```

**2. 全局排序 + 分页：**
```sql
-- 跨 16 个分片取第 5 页
-- 每个分片查 20 条 → 应用层合并排序 → 取第 5 页
-- 弊端：越往后需要取的条数越多（深分页问题放大）

-- 方案：使用"二次查询"或禁止深分页
```

**3. 分布式事务：**
```
跨分片事务的挑战：
  T1: 扣 order_0 的库存
  T2: 在 order_1 创建订单
  如果 T1 成功、T2 失败 → 数据不一致

方案：
  - 两阶段提交（XA）：强一致但性能差
  - TCC（Try-Confirm-Cancel）：业务侵入性强
  - 最终一致性 + 补偿机制（实际工程中最常用）
```

**4. 全局主键（ID）：**
```sql
-- 分片后不能用自增主键（会重复）
-- 方案：
--   Snowflake（雪花算法）：64 位 = 1bit符号 + 41bit时间戳 + 10bit机器ID + 12bit序列号
--   Redis Incr：利用 Redis 自增生成 ID
--   Leaf（美团）：号段模式，预取 ID 段
```

## 分库分表 vs 分区表

```sql
-- MySQL 分区表（与分库分表不同）
CREATE TABLE order (
    id BIGINT,
    user_id INT,
    amount DECIMAL(10,2),
    create_time DATETIME
) PARTITION BY RANGE (YEAR(create_time)) (
    PARTITION p2022 VALUES LESS THAN (2023),
    PARTITION p2023 VALUES LESS THAN (2024),
    PARTITION p2024 VALUES LESS THAN (2025)
);

-- 分区 VS 分片的区别：
--   分区：对应用透明，底层是同一台机器的不同文件
--   分片：应用感知，分布在不同的机器上
```

> [!tip]- **工程要点**：**不要以固定数据量阈值触发分库分表。** 先通过索引、查询模型、归档/分区、读写架构和容量规划解决已测到的瓶颈。真正分片前必须验证分片键覆盖主查询、倾斜处理、扩容迁移、全局 ID、跨分片事务与运维恢复；中间件只是实现选项，不替代这些设计。

## 30 秒回答

**何时考虑分片？** 当单库在真实 SLO 下已出现可测瓶颈，且索引、查询改造、归档/分区、读写架构等手段不足时，再评估分片。核心不是“多少行”，而是分片键能否覆盖主路径、是否均匀、能否迁移与处理跨分片操作。

---



主从复制原理见 → [Master-Slave Replication (主从复制原理)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/10-Master-Slave%20Replication%20(主从复制原理).md) · [MySQL vs Redis：Caching Strategy (缓存策略对比)](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/12-MySQL%20vs%20Redis：Caching%20Strategy%20(缓存策略对比).md)

## 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
08-Sharding and Partitioning (分库分表)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
