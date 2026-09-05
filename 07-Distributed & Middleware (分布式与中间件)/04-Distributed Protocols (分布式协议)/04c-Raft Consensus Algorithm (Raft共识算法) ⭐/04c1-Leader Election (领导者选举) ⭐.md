---
tags:
  - distributed/protocol
status: 🌱
---

> [!important] **核心考点**
> Raft 角色（Leader/Candidate/Follower）、任期、选举流程、随机超时时间

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
    
    int votes = 1;
    for (auto& peer : peers_) {
        go func() {
            if requestVote(peer, n.currentTerm) {
                votes += 1
                if votes > len(peers)/2 {
                    n.becomeLeader()  // 过半数 -> 成为 Leader
                }
            }
        }()
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
| 选举超时时间 | 150-300ms 随机（防 split vote） |
| 成为 Leader 条件 | 获得半数以上投票 |
| 任期作用 | 逻辑时钟，防止过期 Leader 发出指令 |
| 日志完整性限制 | 只有日志最新的节点才能当选 Leader |
| 新 Leader 第一件事 | 发送心跳（空 AppendEntries）确立权威 |

> [!tip]- **工程要点**
> Raft 选举超时的设置需权衡——太短容易频繁选举，太长导致 Leader 故障后恢复慢。etcd 默认 election timeout 为 1000ms。网络不稳定时可通过调整心跳间隔（heartbeat interval = 1/10 election timeout）减少不必要选举。

---

Raft 日志复制与安全性详解见 → [04c2-Log Replication (日志复制)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c2-Log%20Replication%20(日志复制)%20⭐.md) · [04c3-Safety & Membership Change (安全性保证)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c3-Safety%20&%20Membership%20Change%20(安全性保证).md)
