---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# 30 秒回答

**核心结论**：学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# Paxos Overview (Paxos概览)

> [!note] 本节重点：核心考点：> Paxos 核心思想、Basic Paxos、Multi-Paxos、与 Raft 对比

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

## 两阶段提交

```
第一阶段（Prepare）：
  Proposer -> 向 Acceptors 发送 Prepare(n)    // n 是提案编号
  Acceptor -> 如果 n > 之前见过的最大编号
            -> 承诺不再接受编号 < n 的 Prepare
            -> 如果已有 accepted 值，返回 (n, value)

第二阶段（Accept）：
  Proposer -> 收到多数 Acceptors 的 Promise
            -> 如果有返回 value，用最大编号的 value
            -> 如果没有返回 value，可以自由选择 value
            -> 向 Acceptors 发送 Accept(n, value)
  Acceptor -> 如果未收到更大编号的 Prepare
            -> 接受该提案（Accepted）
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

# Multi-Paxos

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

# Paxos vs Raft

| 维度 | Paxos | Raft |
|------|-------|------|
| 设计哲学 | 最简理论模型 | 可理解性优先 |
| Leader 选举 | 未明确指定（Multi-Paxos 需要额外实现） | 心跳 + 随机超时（完整实现） |
| 日志复制 | 通过 Accept 消息 | AppendEntries RPC |
| 成员变更 | 复杂（需额外算法） | 联合共识 / 单节点变更 |
| 工业应用 | Google Chubby, Spanner | etcd, Consul, TiKV |

**Raft 本质上是 Multi-Paxos 的一种特化实现**——把 Paxos 中的"为每个日志位置运行一次 Paxos"简化为"通过 Leader 连续复制日志"。

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| Paxos 两阶段 | Prepare（承诺）-> Accept（接受） |
| Paxos 活锁 | 多个 Proposer 相互覆盖，导致无法达成一致 |
| Multi-Paxos 优化 | 选主后省略 Prepare 阶段，一轮 Accept 决定 |
| Raft vs Paxos | Raft 是可理解的 Paxos 实现 |
| Paxos 的应用 | Google Chubby（分布式锁），ZooKeeper 的 ZAB 受 Paxos 启发 |

> [!tip]- **工程要点**
> Paxos 理论价值极高但实现困难。实际操作中，新系统应优先选择 Raft（etcd/Consul/TiKV 已验证），而非从头实现 Paxos。了解 Paxos 有助于理解分布式一致性的本质。

---

Paxos 与分布式理论基础详解见 → CAP Theorem & BASE Theory (CAP理论) · Consistency Models：Strong, Eventual (一致性模型)

# 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

# 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Paxos (Paxos)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Distributed Fundamentals Map (分布式基础导航)](/06-Data%20and%20Distributed%20(数据与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/00-Distributed%20Fundamentals%20Map%20(分布式基础导航).md)
- 下一步：[04-Distributed Transactions (分布式事务)](/06-Data%20and%20Distributed%20(数据与分布式)/04-Distributed%20Fundamentals%20(分布式基础)/04-Distributed%20Transactions%20(分布式事务).md)
