---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# CAP BASE and Consistency (CAP BASE与一致性)

> [!note] 本节重点：核心考点：> CAP 定理（一致性/可用性/分区容错）、BASE 理论、CP vs AP 权衡、PACELC

# CAP 定理

分布式系统中，一致性（Consistency）、可用性（Availability）、分区容错性（Partition Tolerance）三者最多同时满足两个。

```
          Consistency
             /    \
            /      \
           /        \
          /          \
CP (Redis/Mongo/ZK)   AP (Cassandra/DynamoDB)
          \          /
           \        /
            \      /
             \    /
         Availability
                \
                 \
                  \
             CA（实际上不存在——网络分区时无法同时保证 C 和 A）
```

## 三个属性

| 属性 | 说明 |
|------|------|
| **C（Consistency）** | 所有节点在同一时刻看到相同的数据（强一致性） |
| **A（Availability）** | 每次请求都能获得非错误的响应（但不保证数据最新） |
| **P（Partition Tolerance）** | 网络分区（节点间通信中断）时系统仍能正常运行 |

## 核心洞察

分区是必须面对的故障模型；**只有分区发生时**，系统才必须在一致性与可用性之间做取舍。CAP 不是给整个产品贴永久的“CP/AP 标签”，不同读写操作可有不同语义：

```
网络分区发生时：
CP（如 ZooKeeper）：选择一致性，牺牲可用性
  -> 少数节点停止服务，保证多数节点数据一致
  
AP（如 Cassandra）：选择可用性，牺牲一致性
  -> 所有节点继续服务，但数据可能不一致（最终一致）
```

---

# BASE 理论

BASE 是对 CAP 中 AP 场景的延伸——"基本可用 + 软状态 + 最终一致性"：

| 要素 | 说明 |
|------|------|
| **BA（Basically Available）** | 系统出现故障时，允许损失部分可用性（如降级、限流） |
| **S（Soft State）** | 允许数据中间状态（不一致），不要求强一致性 |
| **E（Eventually Consistent）** | 经过一段时间后，数据最终达到一致 |

## ACID vs BASE

```
ACID and BASE are not opposite product categories.
  ACID: transaction properties for a defined data boundary.
  BASE: a design vocabulary often associated with availability and convergence.
  A system can use local ACID transactions and eventual consistency between services.
```

---

# PACELC 模型

CAP 只考虑了分区时的取舍，PACELC 补充了正常情况下的取舍：

```
P（分区）发生时 → 在 A（可用性）和 C（一致性）间选择
E（正常）时     → 在 L（延迟）和 C（一致性）间选择
```

```
示例应按具体操作、读写一致性设置和故障模型分析：
- 某个异步复制读路径可能偏向可用性与低延迟；
- 某个 quorum 写或线性一致读路径可能为了正确性拒绝部分请求；
- 不要只凭产品名称推导固定 CAP/PACELC 标签（NEEDS_VERIFY）。
```

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| CAP 选两个 | P 是必须的，实际在 CP 和 AP 之间选 |
| 为什么不选 CA | 网络分区是必然的，CA 就是没有 P |
| BASE 的本质 | 用最终一致性换可用性 |
| ACID vs BASE | 传统单体用 ACID，分布式系统用 BASE |
| PACELC 的 E | 正常时在延迟和一致间取舍 |
| CP 系统举例 | ZooKeeper、etcd、HBase |
| AP 系统举例 | Cassandra、DynamoDB、Eureka |

> [!tip]- **工程要点**
> CAP 不是"三选二"的静态决策，而是**可以在不同操作级别做选择**（如关键数据强一致，非关键数据最终一致）。微服务中通常 BASE 为主，仅对支付等关键操作用强一致。

# 30 秒回答

CAP 讨论的是网络分区发生时的取舍：若要保证线性一致，某些请求可能必须失败或等待；若要持续响应，可能返回旧数据或接受稍后收敛。它不等于“平时只能选两个”，也不能用系统名字替代对具体读写路径、quorum 和故障行为的分析。

# 自测

1. CAP 的 C 为什么不能简单等同于 ACID 里的所有“一致性”？
2. 网络没有分区时，CAP 是否要求系统牺牲 A 或 C？
3. 同一服务如何让余额写入与推荐列表读取采用不同一致性策略？

---

一致性模型与分布式事务详解见 → [Consistency Models：Strong, Eventual (一致性模型)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04b-Consistency%20Models：Strong,%20Eventual%20(一致性模型).md) · [Distributed Transaction：2PC & Saga (分布式事务)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04e-Distributed%20Transaction：2PC%20&%20Saga%20(分布式事务).md)

---

# Consistency Models (一致性模型)

> [!note] 本节重点：核心考点：> 一致性模型分级（强/弱/最终）、Quorum 机制、读写模型、Dynamo 风格

# 一致性模型分级

| 模型 | 说明 | 延迟 | 典型场景 |
|------|------|------|---------|
| **强一致** | 写完成后立即对所有读可见 | 高 | 银行转账、库存扣减 |
| **线性一致性** | 所有操作按全局时钟排序，最强 | 最高 | 分布式锁（etcd） |
| **顺序一致性** | 单个进程内操作有序，全局不一定 | 中 | 聊天消息 |
| **因果一致性** | 有因果关系的操作有序 | 中 | 社交网络评论 |
| **最终一致性** | 无更新后最终同意 | 低 | DNS、CDN 缓存 |
| **读己之写** | 总能读到自己的写入 | 低 | 用户资料更新 |
| **单调读** | 后续读不会看到更旧的值 | 低 | 绝大多数应用 |

---

# Quorum 机制

N = 副本数，W = 写确认数，R = 读取数

```
强一致条件：W + R > N
示例（3 副本）：
  W=3, R=1: 写全部节点，读任意（写代价高）
  W=2, R=2: 读写各过半数（推荐）
  W=1, R=3: 写一个节点，读全部（读代价高）

实际常用：N=3, W=2, R=2（读写各保证多数一致）
```

```cpp
// Quorum 读写的关键约束
// 写时 W 个节点确认 -> 读时 R 个节点中最新的版本
// 因为 W + R > N，所以 R 和 W 一定有交集

// 示例：N=3, W=2, R=2
// 写时写入节点 A, B（确认）
// 读时读取节点 A, C
// A 上有最新数据 -> 保证读到最新
```

---

# 读写模型

## 强一致写

```cpp
// 强一致写：W 个副本确认才返回成功（Quorum 机制）
bool write(const string& key, const string& value) {
    int acks = 0;
    for (auto& replica : replicas)
        if (replica.write(key, value)) acks++;
    return acks >= W_;
}
```

## 最终一致读

```cpp
// 动态决策读取策略（DynamoDB 风格）
string read(const string& key, bool strong = false) {
    if (strong) {
        // 强一致读：读所有副本，选最新版本
        vector<string> results;
        for (auto& r : allReplicas)
            results.push_back(r.read(key));
        return resolveConflict(results);   // Vector Clock 解决冲突
    }
    // 最终一致读：读任意副本
    return anyReplica().read(key);
}
```

## Vector Clock（向量时钟）

```
追踪更新因果关系的机制，用于解决冲突：

D1 ([A:1])         <- A 写入 v1
D2 ([A:2])         <- A 更新 v2
D3 ([A:2, B:1])    <- B 读取 D2 后更新（有 A 的因果关系）
D4 ([A:3])         <- A 更新 v3（与 D3 冲突！需要合并）
```

---

# 实现对比

| 系统 | 一致性模型 | 实现方式 |
|------|-----------|---------|
| etcd / ZooKeeper | 线性一致 | Raft + 多数派写入 |
| Cassandra | 可调一致 | N=3, W/R 可配置 |
| DynamoDB | 最终一致/强一致可选 | Vector Clock + Quorum |
| Redis Cluster | 最终一致 | 异步主从复制 |
| MySQL Group Replication | 强一致 | Paxos-like |

---

# 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| W+R > N | Quorum 保证读写有重叠，读能读到最新 |
| 强一致 vs 最终一致 | 一致性越强，延迟越高，可用性越低 |
| Vector Clock 用途 | 检测和解决分布式系统中的写冲突 |
| 读己之写 | 用户总能读到自己的更新（重要用户体验） |
| 单调读 | 读到新数据后不会读到旧数据（防止"时光倒流"） |

> [!tip]- **工程要点**
> 实际系统大多用最终一致性 + 关键路径强一致。如社交平台：发帖（最终一致），点赞数（最终一致），支付（强一致）。Quorum 的 W+R > N 是权衡一致性和性能的关键参数。

---

一致性模型与分布式理论基础详解见 → [CAP Theorem & BASE Theory (CAP理论)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) · [Paxos Overview (Paxos概念了解)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04d-Paxos%20Overview%20(Paxos概念了解).md)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Consistency and CAP (一致性与 CAP)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
