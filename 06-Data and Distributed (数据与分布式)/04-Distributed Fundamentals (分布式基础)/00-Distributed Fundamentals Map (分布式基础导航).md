---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-Distributed Fundamentals Map (分布式基础导航)

> [!abstract] 一句话结论：分布式的难点不是“机器多”，而是节点会独立失败、网络会延迟或分区、请求会重复；设计必须明确在这些条件下仍保持什么性质。

## 推荐顺序

1. `01-CAP BASE and Consistency`：理解网络分区下的取舍语言。
2. `02-Consistency Models`：区分线性一致、顺序一致、最终一致等目标。
3. `03–05 Raft`：依次学习选主、日志复制、安全性与成员变更。
4. `06-Paxos Overview`：建立共识算法的历史与概念对照。
5. `07-Distributed Transactions and Saga`：把一致性选择落回跨服务业务流程。

## 阅读问题

- 发生网络分区时，系统拒绝请求、返回旧数据，还是允许冲突写入？
- 谁是状态的权威副本？副本落后时如何恢复？
- 同一命令被重试多次时，结果如何保持幂等？

## 不要跳过的前置

先完成 `01-MySQL and SQL` 的事务与锁、`02-Cache and Proxy` 的缓存一致性、`03-Messaging and Streaming` 的投递与幂等，再读共识与 Saga。
