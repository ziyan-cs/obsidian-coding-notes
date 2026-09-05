---
tags:
  - distributed/protocol
status: 🌱
---

> [!important] **核心考点**
> 分布式事务方案（2PC/XA、TCC、Saga、本地消息表）、Seata AT 模式、CAP 权衡

## 分布式事务场景

```
跨库转账：
账户 A（银行 A）-> 减 100 元
账户 B（银行 B）-> 加 100 元

要么同时成功，要么同时回滚 -> 跨库事务
```

---

## 方案一：2PC（两阶段提交）

```
协调者（Coordinator / Transaction Manager）

阶段 1（Prepare）：
协调者 -> 参与者 1: CanCommit? 参与者 1 -> 准备资源，日志写 undo/redo
协调者 -> 参与者 2: CanCommit? 参与者 2 -> 准备资源
参与者全部回复 Yes -> 进入阶段 2
任一回复 No -> 全局中止（回滚）

阶段 2（Commit）：
协调者 -> 参与者 1: DoCommit
协调者 -> 参与者 2: DoCommit
参与者执行 Commit，释放资源，回复 ACK
协调者收到全部 ACK -> 事务完成
```

### 2PC 的问题

| 问题 | 说明 |
|------|------|
| 阻塞 | 参与者阶段 1 后持有资源锁，等待协调者决策 |
| 单点故障 | 协调者崩溃则所有参与者阻塞等待 |
| 脑裂 | 阶段 2 中部分参与者收不到 Commit 指令（数据不一致） |
| 无法恢复 | 协调者日志丢失，无法恢复事务状态 |

### 3PC 的改进

```
3PC = 2PC + 超时机制 + 中间阶段（CanCommit -> PreCommit -> DoCommit）

参与者增加超时机制：等待协调者超时后自动提交
解决：协调者阻塞问题

但仍无法解决网络分区时的数据一致性问题
```

---

## 方案二：TCC（Try-Confirm-Cancel）

```
业务层面的两阶段（非数据库层面）：

Try：    预留资源（如冻结库存 10 件）
Confirm：确认执行业务（扣减冻结库存）
Cancel： 取消（释放冻结库存）

示例（下单）：
  Try:    库存服务 -> 冻结 1 件商品（状态：已冻结）
  Confirm: 库存服务 -> 扣减冻结库存（状态：已扣减）
  Cancel:  库存服务 -> 释放冻结库存（状态：已释放）
```

| 优点 | 缺点 |
|------|------|
| 不依赖数据库 XA | 业务侵入性强 |
| 性能好（无锁） | 每个操作需实现 Try/Confirm/Cancel |
| 可应用在任何存储上 | 空回滚、幂等需自行处理 |

---

## 方案三：Saga（长事务）

Saga 将一个大事务拆分为 N 个子事务，每个子事务有对应的补偿操作：

```
正向：T1 -> T2 -> T3 -> T4（正常完成）
回滚：T1 -> T2 -> T3 失败 -> C3 -> C2 -> C1（逐个补偿）

示例（下单 + 扣库存 + 支付）：
  T1: 创建订单
  T2: 扣减库存（C2: 释放库存）
  T3: 扣减余额（C3: 恢复余额）
  T4: 确认订单

  如果 T3 失败：
  -> T4 跳过
  -> C3: 恢复余额
  -> C2: 释放库存
  -> T1 创建的是"失败"订单
```

**Saga 的隔离性：** 子事务提交后对其他事务可见——Saga 不保证隔离性，需要业务层做防护（如"预留"语义）。

---

## 方案四：本地消息表（最终一致）

```
服务 A (Producer)                   服务 B (Consumer)
  │                                  │
  ├─ 1. Execute business logic +     |
  |     Insert message into          |
  |      local table ───────────────→|
  │    (Single local DB transaction) |
  │                                  │
  ├─ 2. Timed task polls             |
  |      message table +             │
  │     Send unfinished messages ───→│
  │                                  ├─ 3. Process message +
  │◄─────────────────────────────────┤     Return ACK response
  │                                  │
  ├─ 4. Mark message as completed    │
  │                                  │
```

**优点：** 无 2PC 的阻塞问题，实现简单
**缺点：** 业务代码与消息耦合，最终一致

---

## 方案对比

| 方案 | 一致性 | 性能 | 业务侵入 | 适用场景 |
|------|--------|------|---------|---------|
| 2PC/XA | 强一致 | 低（锁） | 低 | 银行、支付（小并发） |
| TCC | 较强 | 中 | 高 | 高并发、每个操作可预留 |
| Saga | 最终一致 | 高 | 中 | 长事务、跨服务流程 |
| 本地消息表 | 最终一致 | 高 | 中 | 简单最终一致场景 |
| Seata AT | 较强 | 中 | 低 | 适合微服务（自动代理） |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 2PC 最大问题 | 阻塞 + 单点故障 + 脑裂 |
| TCC 空回滚 | Try 未执行但 Cancel 被调用（需防重复） |
| Saga 补偿 | 补偿操作必须幂等 |
| Seata AT 原理 | 代理 SQL，自动生成 undo/redo 日志 |
| 分布式事务选型 | 强一致选 2PC/Seata，最终一致选 Saga/消息表 |
| 是否所有场景都需要分布式事务 | 不是——很多场景可接受最终一致性+补偿 |

> [!tip]- **工程要点**
> 分布式事务的最大原则是**能不用的场景尽量不用**——通过业务设计规避分布式事务（如数据分区、本地事务合并）。如果必须用，优先考虑 Seata AT（低侵入）或 Saga（高性能）。支付等强一致场景考虑 TCC 或 2PC，但设计好降级和补偿机制。

---

分布式事务的 CAP 权衡与一致性模型详解见 → [CAP Theorem & BASE Theory (CAP理论)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) · [Consistency Models：Strong, Eventual (一致性模型)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04b-Consistency%20Models：Strong,%20Eventual%20(一致性模型).md)
