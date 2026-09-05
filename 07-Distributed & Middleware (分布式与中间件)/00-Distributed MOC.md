---
tags: [vault/navigation, distributed/middleware]
status: seed
verified: 2026-09-05
---

# Distributed MOC — 分布式与中间件导航

> 缓存、消息队列与一致性协议，是从「单机后端」走向「分布式后端」的桥。

## 主题分组
- Redis：[[07-Distributed & Middleware (分布式与中间件)/01-Redis (缓存与数据结构)/01a-Data Structures Internals (底层数据结构实现) ⭐/01a1-SDS：Simple Dynamic String (简单动态字符串)|底层结构]] · [[07-Distributed & Middleware (分布式与中间件)/01-Redis (缓存与数据结构)/01b-Persistence：RDB & AOF (持久化机制) ⭐/01b1-RDB：Snapshot & BGSAVE (快照原理)|持久化]] · [[07-Distributed & Middleware (分布式与中间件)/01-Redis (缓存与数据结构)/01d-Cache Patterns & Problems (缓存三大问题) ⭐/01d1-Cache Penetration (缓存穿透：布隆过滤器)|缓存三问题]] · [[07-Distributed & Middleware (分布式与中间件)/01-Redis (缓存与数据结构)/01e-Redis Single Thread Model (单线程模型为何高性能) ⭐|单线程模型]]
- Nginx：[[07-Distributed & Middleware (分布式与中间件)/02-Nginx (反向代理与负载均衡)/02a-Nginx Architecture：Master & Worker Process (架构模型)|架构]] · [[07-Distributed & Middleware (分布式与中间件)/02-Nginx (反向代理与负载均衡)/02b-Reverse Proxy & Load Balancing Config (反向代理配置)|反向代理配置]]
- 消息队列：[[07-Distributed & Middleware (分布式与中间件)/03-Message Queues (消息队列)/03a-Why MQ：Decoupling, Peak Shaving, Async (消息队列三大作用)|MQ 三作用]] · [[07-Distributed & Middleware (分布式与中间件)/03-Message Queues (消息队列)/03b-Kafka Architecture Overview (Kafka架构概览)/03b1-Topic, Partition, Consumer Group (核心概念)|Kafka]] · [[07-Distributed & Middleware (分布式与中间件)/03-Message Queues (消息队列)/03c-RabbitMQ vs Kafka：Comparison (两者对比)|RabbitMQ vs Kafka]]
- 分布式协议：[[07-Distributed & Middleware (分布式与中间件)/04-Distributed Protocols (分布式协议)/04a-CAP Theorem & BASE Theory (CAP理论) ⭐|CAP]] · [[07-Distributed & Middleware (分布式与中间件)/04-Distributed Protocols (分布式协议)/04c-Raft Consensus Algorithm (Raft共识算法) ⭐/04c1-Leader Election (领导者选举) ⭐|Raft]] · [[07-Distributed & Middleware (分布式与中间件)/04-Distributed Protocols (分布式协议)/04e-Distributed Transaction：2PC & Saga (分布式事务)|分布式事务]]

## 学习顺序
1. Redis 数据结构 → 2. 持久化 → 3. 缓存三问题 → 4. Nginx → 5. MQ → 6. CAP/一致性 → 7. Raft。

## 与 C++ / 项目入口
- 缓存落点：[[06-Database (数据库)/04-High Availability & Architecture (高可用与架构)/12-MySQL vs Redis：Caching Strategy (缓存策略对比)|MySQL vs Redis]]
- 项目：[[12-Backend Projects (后端项目)/04-Flash Sale|Flash Sale]] · [[12-Backend Projects (后端项目)/05-Config Center|Config Center]]
