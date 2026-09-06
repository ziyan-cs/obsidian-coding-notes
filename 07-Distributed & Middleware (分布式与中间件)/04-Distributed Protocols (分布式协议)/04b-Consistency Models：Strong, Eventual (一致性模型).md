---
tags:
  - distributed/protocol
status: 🌱
---

# Consistency Models：Strong, Eventual — 一致性模型

> [!important] **核心考点**
> 一致性模型分级（强/弱/最终）、Quorum 机制、读写模型、Dynamo 风格

## 一致性模型分级

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

## Quorum 机制

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

## 读写模型

### 强一致写

```cpp
// 强一致写：W 个副本确认才返回成功（Quorum 机制）
bool write(const string& key, const string& value) {
    int acks = 0;
    for (auto& replica : replicas)
        if (replica.write(key, value)) acks++;
    return acks >= W_;
}
```

### 最终一致读

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

### Vector Clock（向量时钟）

```
追踪更新因果关系的机制，用于解决冲突：

D1 ([A:1])         <- A 写入 v1
D2 ([A:2])         <- A 更新 v2
D3 ([A:2, B:1])    <- B 读取 D2 后更新（有 A 的因果关系）
D4 ([A:3])         <- A 更新 v3（与 D3 冲突！需要合并）
```

---

## 实现对比

| 系统 | 一致性模型 | 实现方式 |
|------|-----------|---------|
| etcd / ZooKeeper | 线性一致 | Raft + 多数派写入 |
| Cassandra | 可调一致 | N=3, W/R 可配置 |
| DynamoDB | 最终一致/强一致可选 | Vector Clock + Quorum |
| Redis Cluster | 最终一致 | 异步主从复制 |
| MySQL Group Replication | 强一致 | Paxos-like |

---

## 经典题型速查

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

一致性模型与分布式理论基础详解见 → [CAP Theorem & BASE Theory (CAP理论)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) · [Paxos Overview (Paxos概念了解)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04d-Paxos%20Overview%20(Paxos概念了解).md)
