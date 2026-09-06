---
tags:
  - distributed/mq
status: 🌱
---

# RabbitMQ vs Kafka：Comparison — 两者对比

> [!important] **核心考点**
> RabbitMQ 与 Kafka 的设计差异、适用场景、技术选型

## 设计哲学对比

| 维度 | RabbitMQ | Kafka |
|------|----------|-------|
| 定位 | 消息代理（Message Broker） | 分布式流处理平台 |
| 模型 | Exchange -> Queue -> Consumer | Topic -> Partition -> Consumer Group |
| 消息存储 | 消费即删除（默认） | 持久化日志，按时间/大小清理 |
| 消费模式 | Push（推） | Pull（拉） |
| 顺序保证 | 单队列有序 | 分区内有序 |
| 路由能力 | 灵活（direct/topic/fanout/headers） | 基于 key hash |

---

## 性能对比

```
RabbitMQ 吞吐量：万级/秒（典型 2-5 万/s）
Kafka 吞吐量：   百万级/秒（典型 50-100 万/s）

Kafka 快的原因：
1. 顺序磁盘写入（比随机写快 1000 倍）
2. 零拷贝（sendfile）
3. 批量压缩发送
4. 分区并行

延迟：
RabbitMQ: 微秒级（< 100us，实时推送）
Kafka:    毫秒级（2-5ms，批量设计）
```

---

## 选型指南

### 选 RabbitMQ

| 场景 | 原因 |
|------|------|
| 复杂路由 | Exchange 类型丰富 |
| 低延迟（< 1ms） | Push 模式，消息即到即推 |
| 业务系统（订单/支付） | 消息确认机制成熟 |
| 灵活的死信队列 | DLX + TTL 支持 |
| 小规模（< 1万/秒） | 部署运维简单 |

### 选 Kafka

| 场景 | 原因 |
|------|------|
| 高吞吐日志/埋点 | 百万级/秒 |
| 流处理/实时计算 | Kafka Streams / Flink |
| 消息重播/回溯 | 持久化日志，可重置 offset |
| 数据管道（ETL） | 批量消费、顺序写 |
| 大数据生态 | Hadoop / Spark 天然对接 |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| Kafka 为什么快 | 顺序写 + 零拷贝 + 批量 + 分区 |
| RabbitMQ 消息模型 | Exchange 绑定 Queue，消费即删 |
| RabbitMQ 死信队列 | 消息过期/拒绝 -> DLX |
| 如何选型 | 低延迟灵活路由选 RabbitMQ；高吞吐流处理选 Kafka |

> [!tip]- **工程要点**
> Kafka 不适合传统任务队列（延迟/定时任务），RabbitMQ 不适合海量日志。微服务内部通信选 RabbitMQ，大数据管道选 Kafka。很多公司两者都用。

---

消息队列基础概念详解见 → [Why MQ：Decoupling, Peak Shaving, Async (消息队列三大作用)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/03-Message%20Queues%20(消息队列)/03a-Why%20MQ：Decoupling,%20Peak%20Shaving,%20Async%20(消息队列三大作用).md)
