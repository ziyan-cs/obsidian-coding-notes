---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Raft Consensus (Raft 共识)

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

## 30 秒回答

**核心结论**：学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。


## Raft Leader Election (Raft领导者选举)

> [!note] 本节重点：核心考点：> Raft 角色（Leader/Candidate/Follower）、任期、选举流程、随机超时时间

## Raft 角色与任期

### 三种角色

```
Leader（领导者）     → 处理客户端请求、管理日志复制
Candidate（候选者）  → 参与 Leader 选举
Follower（跟随者）   → 被动接受 Leader 的日志
```

**正常状态流转：**

```
Follower ->（选举超时，发起选举）-> Candidate ->（获得多数票）-> Leader
                                              ->（选举超时）-> 回到 Follower
Leader ->（收到更高任期请求）-> Follower
```

### 任期（Term）

```
Term 1        Term 2        Term 3          Term 4
[Leader A]    [无 Leader]   [Leader B]      [Leader C]
    |            |             |               |
    |----选举----X---选举------|---正常操作------|----
    |            |   (split vote)  |           |
```

- Raft 时间被划分为不同 Term，每个 Term 以选举开始
- 每个 Term 最多有一个 Leader
- 如果选举失败（split vote），该 Term 无 Leader，立即进入下一 Term

---

## 选举流程

```
1. Follower 在 election timeout 内未收到 Leader 心跳
   -> 转为 Candidate
   -> 任期 +1
   -> 给自己投票
   -> 向其他节点发送 RequestVote RPC

2. 收到 RequestVote 的节点：
   - 如果 Candidate 的任期 >= 自己，且未在当前 Term 投票
   - 则投票给该 Candidate

3. 获得超过半数投票的 Candidate 成为 Leader
   -> 立即发送心跳（AppendEntries RPC）确立权威
   -> 重置其他节点的选举超时
```

```cpp
// 选举超时逻辑（C++ + condition_variable）
void Node::runElectionTimer() {
    auto timeout = randomDuration(150ms, 300ms);  // 随机超时
    unique_lock lock(mtx_);
    if (cv_.wait_for(lock, timeout) == cv_status::timeout) {
        startElection();  // 超时 -> 发起选举
    }
    // 收到 Leader 心跳 -> cv_ 被 notify -> 重置定时器
}

void Node::startElection() {
    currentTerm_++;
    role_ = CANDIDATE;
    votedFor_ = id_;  // 投自己
    
    std::atomic<int> votes{1};
    for (auto& peer : peers_) {
        std::thread([this, peer, term = currentTerm_, &votes] {
            if (requestVote(peer, term) && votes.fetch_add(1) + 1 > peers_.size() / 2)
                becomeLeaderIfCurrentTerm(term);
        }).detach(); // 教学示意：生产实现需托管线程、取消与 RPC 超时
    }
}
```

---

## 随机超时时间

**为什么要随机？** 避免 split vote（多个节点同时成为 Candidate，各得一半票）：

```
如果没有随机超时：
  所有 Follower 同时超时 -> 同时成为 Candidate
  -> 互相不投票 -> 选举失败 -> 新 Term 再次同时超时
  -> 活锁（选举永不成功）

有随机超时（150-300ms）：
  大概率某个 Follower 先超时 -> 在其他节点超时前
  发送 RequestVote -> 获得多数票 -> 发送心跳
  -> 其他节点收到心跳，重置定时器
```

---

## 选举限制

**只有拥有全部已提交日志的节点才能当选 Leader：**

```
Candidate 的 RequestVote 中带上自己最后一条日志的 (term, index)
Follower 投票前比较：
  - Candidate 的最后一条日志 term > 自己的 => 投票
  - 同 term 时 Candidate 的日志 index >= 自己的 => 投票
  - 否则不投票

这个限制保证 Leader 一定包含所有已提交的日志
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 选举超时时间 | 随机区间以降低 split vote；具体数值由网络与实现配置决定 |
| 成为 Leader 条件 | 获得半数以上投票 |
| 任期作用 | 逻辑时钟，防止过期 Leader 发出指令 |
| 日志完整性限制 | 只有日志最新的节点才能当选 Leader |
| 新 Leader 第一件事 | 发送心跳（空 AppendEntries）确立权威 |

> [!tip]- **工程要点**
> 选举 timeout 应显著大于稳定网络中的心跳与调度抖动，同时满足故障恢复目标；具体比例和 etcd 默认值都应以当前版本文档/部署测量验证。实现中还要防止过期 RPC、并发状态迁移和没有取消的后台线程。

---

Raft 日志复制与安全性详解见 → [04c2-Log Replication (日志复制)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c2-Log%20Replication%20(日志复制)%20⭐.md) · [04c3-Safety & Membership Change (安全性保证)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c3-Safety%20&%20Membership%20Change%20(安全性保证).md)

---

## Raft Log Replication (Raft日志复制)

> [!note] 本节重点：核心考点：> 日志结构、日志复制流程、日志匹配特性、Leader 崩溃处理

## Raft 日志结构

每条日志条目包含：**状态机命令 + 任期号 + 索引号**

```
Leader 视角的日志：

  Term:  1     1     1     2     2     3     3
  Index: 1     2     3     4     5     6     7
        ┌────┬────┬────┬────┬────┬────┬────┐
        │x=3 │y=1 │y=9 │x=2 │x=5 │z=3 │z=7 │  -> 应用到状态机
        └────┴────┴────┴────┴────┴────┴────┘
         已提交并应用              已提交   未提交（未同步多数）
```

---

## 日志复制流程

```text
Client               Leader              Follower 1          Follower 2
  │                     │                     │                   │
  ├── Proposal:         │                     │                   │
  │   SET x=3 ─────────→│                     │                   │
  │                     ├── Append log        │                   │
  │                     │   locally           │                   │
  │                     │   (uncommitted)     │                   │
  │                     ├── AppendEntries ────→│                   │
  │                     │   RPC               │                   │
  │                     ├── AppendEntries ──────────────────────→│
  │                     │   RPC               │                   │
  │                     │◄── Success ─────────┤                   │
  │                     │◄── Success ────────────────────────────┤
  │                     │                     │                   │
  │    Majority acknowledged → commit          │                   │
  │                     │                     │                   │
  │                     ├── Mark log as       │                   │
  │                     │   committed         │                   │
  │◄──── Return success ┤                     │                   │
  │                     │                     │                   │
  │                     ├── Next AppendEntries│                   │
  │                     │   (with commitIndex)───────→            │
  │                     ├── Next AppendEntries│                   │
  │                     │   (with commitIndex)───────────────────→│
  │                     │                     │                   │
  │                     │  Follwers see commitIndex                │
  │                     │  → apply to state machine               │
```

**关键点：**
1. Leader 收到客户端请求，先将日志追加到自己的日志
2. 并行向所有 Follower 发送 AppendEntries RPC
3. 日志写入多数节点（半数以上）后即视为已提交（committed）
4. Leader 应用到状态机后回复客户端
5. 后续的心跳/AppendEntries 会携带最新 commitIndex 通知 Follower 提交

---

## 日志匹配特性

Raft 的**核心一致性保证**——两条关键性质：

```
1. 如果不同节点上有两条日志条目具有相同的 index 和 term
   则它们存储的命令相同（内容一致）

2. 如果不同节点上有两条日志条目具有相同的 index 和 term
   则它们之前的所有日志也一致

实现方式：
AppendEntries RPC 携带 prevLogIndex 和 prevLogTerm
- Follower 检查自己的日志在 prevLogIndex 位置是否为 prevLogTerm
- 如果不匹配 -> 拒绝该 AppendEntries
- Leader 收到拒绝 -> 回退 prevLogIndex 重试（最终找到一致点）
```

---

## Leader 崩溃恢复

```
正常：    S1(L) 1 1 1 2 2 3
          S2    1 1 1 2 2 3
          S3    1 1 1 2

Leader S1 崩溃后：
S3 可能被选为 Leader（日志 1 1 1 2 —— 可能覆盖 S2 未提交的条目）

Raft 规则：Leader 强制覆写 Follower 中与自己不一致的日志条目
被覆写的都是未提交的日志（已提交的日志因多数派性质一定不会丢失）
```

---

## 日志压缩与快照

```
日志会无限增长 -> 需要快照压缩

快照包含：
- 当前状态机状态（如 KV 数据库的全部键值）
- 最后一条已提交日志的 index 和 term

     ┌───────────────────────────────┐
     │          Snapshot             │
     │  lastIncludedIndex = 5        │
     │  lastIncludedTerm = 2         │
     │  state machine data: {...}    │
     └───────────────────────────────┘
     ↑ 可以丢弃 index <= 5 的日志 
```

---

## 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| 日志何时算提交 | 写入多数节点（过半）即提交 |
| 日志匹配特性 | 相同 index/term 则内容和之前日志完全一致 |
| Leader 覆写 | Leader 可以强制覆写 Follower 的不一致日志（未提交的） |
| 什么是快照 | 状态机状态的"照片"，用于截断日志 |
| 客户端请求何时返回 | 日志提交+应用到状态机后 |

> [!tip]- **工程要点**
> Raft 写性能受限于多数派中最慢的节点。生产环境通常 3-5 节点，3 节点允许 1 故障，5 节点允许 2 故障。磁盘 I/O 是 Raft 的主要瓶颈——建议 SSD。批量写入可大幅提升吞吐（如 etcd 的 txn 合并多个操作）。

---

Raft 完整流程详解见 → [04c1-Leader Election (领导者选举)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md) · [04c3-Safety & Membership Change (安全性保证)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c3-Safety%20&%20Membership%20Change%20(安全性保证).md)

---

## Raft Safety and Membership (Raft安全性与成员变更)

> [!note] 本节重点：核心考点：> Raft 安全性保证（Election Safety / Leader Completeness / State Machine Safety）、成员变更、联合共识

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

## 经典题型速查 · 延伸要点 3
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



## 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

## 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Raft Consensus (Raft 共识)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
