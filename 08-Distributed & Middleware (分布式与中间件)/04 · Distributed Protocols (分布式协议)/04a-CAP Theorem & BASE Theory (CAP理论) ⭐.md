---
tags:
  - distributed
  - distributed-protocol
---

> **核心考点**：CAP 定理（一致性/可用性/分区容错）、BASE 理论、CP vs AP 权衡、PACELC

## CAP 定理

分布式系统中，一致性（Consistency）、可用性（Availability）、分区容错性（Partition Tolerance）三者最多同时满足两个。

```
          Consistency
             /
            /  \
           /    \
          /      \
CP (Redis/Mongo/ZK)   AP (Cassandra/DynamoDB)
          \      /
           \    /
            \  /
             \/
    Availability
          \
           \
            \
             CA（实际上不存在——网络分区时无法同时保证 C 和 A）
```

### 三个属性

| 属性 | 说明 |
|------|------|
| **C（Consistency）** | 所有节点在同一时刻看到相同的数据（强一致性） |
| **A（Availability）** | 每次请求都能获得非错误的响应（但不保证数据最新） |
| **P（Partition Tolerance）** | 网络分区（节点间通信中断）时系统仍能正常运行 |

### 核心洞察

**P 是必选的**——只要有网络就有分区可能。所以实际只能在 CP 和 AP 中选择：

```
网络分区发生时：
CP（如 ZooKeeper）：选择一致性，牺牲可用性
  -> 少数节点停止服务，保证多数节点数据一致
  
AP（如 Cassandra）：选择可用性，牺牲一致性
  -> 所有节点继续服务，但数据可能不一致（最终一致）
```

---

## BASE 理论

BASE 是对 CAP 中 AP 场景的延伸——"基本可用 + 软状态 + 最终一致性"：

| 要素 | 说明 |
|------|------|
| **BA（Basically Available）** | 系统出现故障时，允许损失部分可用性（如降级、限流） |
| **S（Soft State）** | 允许数据中间状态（不一致），不要求强一致性 |
| **E（Eventually Consistent）** | 经过一段时间后，数据最终达到一致 |

### ACID vs BASE

```
ACID（传统数据库）            BASE（分布式系统）
  ────────────                ────────────
  强一致性                   最终一致性
  悲观（锁）                 乐观（补偿）
  追求正确性                 追求可用性
  难以水平扩展               天然可扩展
```

---

## PACELC 模型

CAP 只考虑了分区时的取舍，PACELC 补充了正常情况下的取舍：

```
P（分区）发生时 → 在 A（可用性）和 C（一致性）间选择
E（正常）时     → 在 L（延迟）和 C（一致性）间选择
```

```
典型系统：
DynamoDB/Cassandra：  PC/EC（分区选一致性，正常选低延迟）
                          -> 其实就是 AP + 最终一致
MongoDB（默认）：     PA/EL（分区选可用，正常选低延迟）
                          -> 最终一致
MongoDB（强一致）：   PC/EC（写关注 majority）
                          -> CP + 高延迟
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| CAP 选两个 | P 是必须的，实际在 CP 和 AP 之间选 |
| 为什么不选 CA | 网络分区是必然的，CA 就是没有 P |
| BASE 的本质 | 用最终一致性换可用性 |
| ACID vs BASE | 传统单体用 ACID，分布式系统用 BASE |
| PACELC 的 E | 正常时在延迟和一致间取舍 |
| CP 系统举例 | ZooKeeper、etcd、HBase |
| AP 系统举例 | Cassandra、DynamoDB、Eureka |

> **工程要点**：CAP 不是"三选二"的静态决策，而是**可以在不同操作级别做选择**（如关键数据强一致，非关键数据最终一致）。微服务中通常 BASE 为主，仅对支付等关键操作用强一致。

---

## 关联笔记

- [Consistency Models：Strong, Eventual (一致性模型)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04b-Consistency%20Models：Strong,%20Eventual%20(一致性模型).md)
- [Paxos Overview (Paxos概念了解)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04d-Paxos%20Overview%20(Paxos概念了解).md)
- [Distributed Transaction：2PC & Saga (分布式事务)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04e-Distributed%20Transaction：2PC%20&%20Saga%20(分布式事务).md)
- [01a1-SDS：Simple Dynamic String (简单动态字符串)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md)
- [01a2-ziplist & listpack (压缩列表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md)
