---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-Data and Distributed MOC (数据与分布式导航)

> [!abstract] 一句话结论：先保证单机数据正确，再解决缓存与异步带来的不一致，最后用分布式协议管理跨节点故障与协调。

## 学习顺序

1. **MySQL and SQL（MySQL 与 SQL）**：查询、索引、事务、锁与执行计划。
2. **Cache and Proxy（缓存与代理）**：Redis 数据结构、缓存一致性、Nginx 反向代理。
3. **Messaging and Streaming（消息与流）**：异步解耦、投递语义、Kafka 与消息可靠性。
4. **Distributed Fundamentals（分布式基础）**：CAP、一致性模型、Raft 与分布式事务。

## 判断顺序

面对一个架构选择，依次问：

1. 数据的唯一事实来源是什么？
2. 哪些读可以缓存，失效后怎样回源？
3. 失败与重试会不会造成重复、乱序或不一致？
4. 多节点故障时，系统选择可用性还是强一致性？

## 本模块出口

- 能为一个后端项目解释数据写入、缓存更新、异步通知与故障恢复的完整链路。
- 能在面试或设计评审中明确说出每个一致性取舍的代价。
