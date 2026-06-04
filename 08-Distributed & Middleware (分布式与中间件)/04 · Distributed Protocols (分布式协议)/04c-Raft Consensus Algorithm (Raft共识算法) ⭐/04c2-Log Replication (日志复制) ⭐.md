---
tags:
  - distributed
  - distributed-protocol
---

> **核心考点**：日志结构、日志复制流程、日志匹配特性、Leader 崩溃处理

## Raft 日志结构

每条日志条目包含：**状态机命令 + 任期号 + 索引号**

```
Leader 视角的日志：

  Term:  1     1     1     2     2     3     3
  Index: 1     2     3     4     5     6     7
        ┌────┬────┬────┬────┬────┬────┬────┐
        │x=3 │y=1 │y=9 │x=2 │x=5 │z=3 │z=7 │  -> 应用到state机
        └────┴────┴────┴────┴────┴────┴────┘
         已提交并应用              已提交   未提交（未同步多数）
```

---

## 日志复制流程

```
Client         Leader               Follower
  │              │                     │
  ├── Propose ──→│                     │
  │              │ Append to local log │
  │              │                     │
  │              │ AppendEntries RPC ──┤  ← 并行发送给所有 Follower
  │              │  (entries, commitIndex)
  │              │◄────────────────────┤  ← Follower 追加到本地日志后回复
  │              │  (majority acked)          │
  │              │ Apply to state machine
  │              │                     │
  │◄── Response ─┤                     │
  │              │ 下一条 AppendEntries │
  │              │  (with updated commitIndex) ──→ Follower Commit
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

     ┌──────────────────────────────┐
     │          Snapshot             │
     │  lastIncludedIndex = 5        │
     │  lastIncludedTerm = 2         │
     │  state machine data: {...}    │
     └──────────────────────────────┘
     ↑ 可以丢弃 index <= 5 的日志 
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 日志何时算提交 | 写入多数节点（过半）即提交 |
| 日志匹配特性 | 相同 index/term 则内容和之前日志完全一致 |
| Leader 覆写 | Leader 可以强制覆写 Follower 的不一致日志（未提交的） |
| 什么是快照 | 状态机状态的"照片"，用于截断日志 |
| 客户端请求何时返回 | 日志提交+应用到状态机后 |

> **工程要点**：Raft 写性能受限于多数派中最慢的节点。生产环境通常 3-5 节点，3 节点允许 1 故障，5 节点允许 2 故障。磁盘 I/O 是 Raft 的主要瓶颈——建议 SSD。批量写入可大幅提升吞吐（如 etcd 的 txn 合并多个操作）。

---

## 关联笔记

- [04c1-Leader Election (领导者选举)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md)
- [04c3-Safety & Membership Change (安全性保证)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c3-Safety%20&%20Membership%20Change%20(安全性保证).md)
- [01a1-SDS：Simple Dynamic String (简单动态字符串)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md)
- [01a2-ziplist & listpack (压缩列表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a2-ziplist%20&%20listpack%20(压缩列表).md)
- [01a3-skiplist：Sorted Set Internals (跳表)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
