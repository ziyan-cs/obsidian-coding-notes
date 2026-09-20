---
study_stage: backlog
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

> [!summary] 核心摘要
>
> Paxos 通过 proposal number 和多数派交集保证不会选择两个不同值；Prepare/Promise 与 Accept/Accepted 分别建立提案优先级和接受规则。工程实现通常使用 Multi-Paxos 降低重复协商成本。

# Paxos Overview (Paxos概览)

> [!note] 本节重点： Paxos 核心思想、Basic Paxos、Multi-Paxos、与 Raft 对比

# Paxos 简介

Paxos 是 Leslie Lamport 提出的分布式一致性算法，被认为是"分布式协议的基石"。

## Basic Paxos

Paxos 中三种角色：

```
Proposer（提议者） -> 提出提案
Acceptor（接受者） -> 投票决定是否接受提案
Learner（学习者）  -> 学习最终选定的值

一个节点可以同时充当多个角色
```

## 两阶段提案：Prepare 与 Accept

这里的“两阶段”是单值 Paxos 的协商过程，**不是**事务型 2PC 的 prepare/commit。提案只有在**多数 acceptor 接受同一编号和值**后才被选定；某一个 acceptor 回复 Accepted 并不代表全局已选定。

细节以 Lamport 的 [Paxos Made Simple](https://lamport.azurewebsites.net/pubs/paxos-simple.pdf) 为准。

```
第一阶段（Prepare）：
  Proposer -> 向 Acceptors 发送 Prepare(n)    // n 是提案编号
  Acceptor -> 若 n 高于已承诺的编号，则持久承诺不再接受较小编号的提案
            -> 回报自己已接受过的最高编号及其 value（如果有）

第二阶段（Accept）：
  Proposer -> 收到多数 Acceptors 的 Promise
            -> 如果有返回已接受的值，必须选返回值中编号最高者的 value
            -> 如果没有返回 value，可以自由选择 value
            -> 向 Acceptors 发送 Accept(n, value)
  Acceptor -> 若未承诺更高编号，则接受该提案并返回 Accepted
  多数 Accepted -> 该值被选定；Learner 才能安全学习结果
```

## Paxos 活锁问题

```
Proposer P1 发 Prepare(1) -> 获得多数 Promise
Proposer P2 发 Prepare(2) -> 获得更多 Promise
P1 的 Accept(1) 被拒绝（已有更大编号 Prepare）
P1 重试 Prepare(3) -> P2 的 Accept(2) 被拒绝
P2 重试 Prepare(4) -> ...
无限循环 -> 活锁！

解决方案：选一个 Distinguished Proposer（类似于 Raft 的 Leader）
使用 Leader 选举避免竞争 -> 这就是 Multi-Paxos 的思想
```

---

# Multi-Paxos（多提议者扩展）

Basic Paxos 的优化——选出一个稳定的 Leader，简化后续流程：

```
优化前（Basic Paxos）：每个值需要两轮 RTT
  客户端 -> Proposer -> Acceptors -> 两阶段 -> 决定一个值

优化后（Multi-Paxos）：
  1. 先选出一个 Leader（一轮 Prepare，获得 Promise）
  2. 后续所有提案（不同 index）的 Prepare 阶段可省略
  3. 只需一轮 Accept 即可决定
  4. 类似 Raft 的日志复制
```

| | Basic Paxos | Multi-Paxos | Raft |
|--|------------|-------------|------|
| 选主 | 无 | 需要 | 需要 |
| 提案效率 | 2 轮 RTT | 1 轮 RTT（选主后） | 1 轮 RTT |
| 可理解性 | 极难 | 较难 | 简单 |

---

# Paxos 与 Raft

| 维度 | Paxos | Raft |
|------|-------|------|
| 设计哲学 | 最简理论模型 | 可理解性优先 |
| Leader 选举 | 未明确指定（Multi-Paxos 需要额外实现） | 心跳 + 随机超时（完整实现） |
| 日志复制 | 通过 Accept 消息 | AppendEntries RPC |
| 成员变更 | 复杂（需额外算法） | 联合共识 / 单节点变更 |
| 工业应用 | Google Chubby, Spanner | etcd, Consul, TiKV |

Raft 与 Multi-Paxos 都利用多数派和稳定 leader 降低正常路径开销，但选举限制、日志匹配、成员变更与可理解性设计不同；不把 Raft 直接等同为某个 Multi-Paxos 实现。

---

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | Paxos 两阶段 | Prepare（承诺）-> Accept（接受） |
> | Paxos 活锁 | 多个 Proposer 相互覆盖，导致无法达成一致 |
> | Multi-Paxos 优化 | 选主后省略 Prepare 阶段，一轮 Accept 决定 |
> | Raft vs Paxos | 目标相近，但安全规则和协议结构需要分别理解 |
> | Paxos 的应用 | Google Chubby（分布式锁），ZooKeeper 的 ZAB 受 Paxos 启发 |
>

> [!tip]- **工程要点**
> Paxos 理论价值极高但实现困难。实际操作中，新系统应优先选择 Raft（etcd/Consul/TiKV 已验证），而非从头实现 Paxos。了解 Paxos 有助于理解分布式一致性的本质。

>
> ---
>

> [!info]- 延伸阅读
> - 下一步：[04-Distributed Transactions (分布式事务)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/04-Distributed%20Transactions%20(分布式事务).md)
