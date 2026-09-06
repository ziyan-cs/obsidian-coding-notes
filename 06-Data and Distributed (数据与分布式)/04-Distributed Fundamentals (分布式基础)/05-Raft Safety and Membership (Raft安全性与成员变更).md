---
tags:
  - distributed/protocol
status: 🌱
---

# 04c3-Safety & Membership Change — 安全性保证

> [!abstract] 核心考点：> Raft 安全性保证（Election Safety / Leader Completeness / State Machine Safety）、成员变更、联合共识

## Raft 安全性保证

Raft 保证以下安全性：

### 1. Election Safety（选举安全）

**一个 Term 内最多只有一个 Leader**

```
同一 Term 内，一个节点最多投出一票
获得多数票的 Candidate 成为 Leader
不可能出现两个节点都获得多数票
```

### 2. Leader Completeness（Leader 完整性）

**Leader 一定包含所有已提交的日志**

```
原因：
- 已提交的日志存在于多数节点
- 候选者需要获得多数票才能成为 Leader
- 多数派之间必有交集
- 投票时检查候选者日志是否至少和自己一样新
=> 当选 Leader 一定包含全部已提交日志
```

### 3. State Machine Safety（状态机安全）

**如果某个节点将某条日志应用到状态机，其他节点不会在该位置应用不同的日志**

```
原因：日志匹配特性保证日志内容一致
只有 Leader 能提交日志，提交的日志一定是多数派一致的
```

### 4. Log Matching（日志匹配）

已在 04c2 中详述——相同 index/term 的条目内容及之前日志完全一致。

---

## 成员变更（集群扩容/缩容）

Raft 成员变更的最大挑战——**不能直接从旧配置直接切换到新配置**（可能同时出现两个 Majority）：

```
旧配置：{A, B, C}  -> 新配置：{A, B, C, D, E}
直接切换问题：
  - 旧配置的多数派：A, B（2/3）
  - 新配置的多数派：A, B, D（3/5）
  - 这两个集合可以没有交集！
  - 可能选出两个 Leader，导致脑裂
```

### 联合共识（Joint Consensus）

```
联合共识（Joint Consensus）两阶段变更：

阶段 1：进入 Cold+New
  - 选举需要 Cold 多数 AND Cnew 多数
  - 日志复制需要 Cold 多数 OR Cnew 多数

阶段 2：完全切换到 Cnew
  - Leader 提交 Cnew，所有节点使用新配置

单节点变更（etcd 使用，简化方案）：
  一次只增删一个节点 → 新旧多数派必有交集
  3→4 扩容：A,B,C → A,B,C,D
  多数派交集：{A,B} ∩ {A,B,D} = {A,B}  ✅
  逐个添加，无需联合共识
```

---

## 节点恢复

```
节点恢复流程：
1. Follower/Candidate 超时未收到 Leader 消息 → 发起选举
2. 若落后太多 → Leader 通过 InstallSnapshot RPC 发送快照
3. 重启后从磁盘恢复持久化状态（currentTerm, votedFor, log[]）
4. 从快照恢复状态机
5. 以 Follower 身份启动
```

---

## 持久化

Raft 节点需要持久化的状态：

| 字段 | 说明 |
|------|------|
| currentTerm | 当前任期（重启后避免重复投票） |
| votedFor | 当前任期投给了谁 |
| log[] | 日志条目（未提交的也要持久化） |
| lastApplied | 最后应用到状态机的日志 |
| snapshot | 最新的快照 |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 脑裂如何防止 | Leader Completeness + 多数派保证 |
| 成员变更问题 | 新旧配置多数派可能无交集 |
| 联合共识 | 两个阶段的多数派各需多数 |
| 单节点变更 | 一次只增减一个，更简单 |
| InstallSnapshot | 落后节点通过快照追赶 |

> [!tip]- **工程要点**
> etcd 使用单节点变更简化了成员变更。生产环境 Raft 集群通常为 3 或 5 节点。3 节点允许 1 故障，5 节点允许 2 故障。偶数节点（4/6）没有优势（多数派为 3/4 或 4/6，故障容限相同但多了节点）。

---

Raft 基础机制详解见 → [04c1-Leader Election (领导者选举)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md) · [04c2-Log Replication (日志复制)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c2-Log%20Replication%20(日志复制)%20⭐.md)
