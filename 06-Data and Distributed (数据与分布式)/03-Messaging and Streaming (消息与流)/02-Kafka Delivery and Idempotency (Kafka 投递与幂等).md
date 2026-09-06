---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Kafka Delivery and Idempotency (Kafka 投递与幂等)

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

## Kafka Core Concepts (Kafka核心概念)

> [!note] 本节重点心考点：> Topic/Partition/Consumer Group、分区机制、消息有序性、消费者 Rebalance

## Kafka 核心概念

Producer -> Topic -> Consumer Group

Topic 包含多个 Partition，每个 Partition 是有序的、不可变的日志序列：

```
Partition 0:
[msg0] [msg1] [msg2] [msg3] [msg4] [msg5] -> ...
offset: 0     1     2     3     4     5
```

- **分区内有序**：消息按写入顺序追加，offset 递增
- **全局无序**：不同分区之间不保证顺序
- **分区数决定并行度**：一个分区同时只能被一个消费者消费

### Consumer Group

```properties

```

---

## 分区与消息路由

```cpp
// 生产者决定消息写入哪个分区
// 1. 指定分区 -> 直接写入（librdkafka API）
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_PARTITION(0),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);

// 2. 有 key -> hash(key) % 分区数 （相同 key 保证同一分区）
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_KEY("user123", 7),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);

// 3. 无 key -> 轮询（round-robin，librdkafka 自动处理）
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);
```

---

## 消费者 Rebalance

当消费者加入/退出或分区数变更时触发 Rebalance：

```
Topic-A (3 partitions)
  P0 -- Consumer-1
  P1 -- Consumer-2
  P2 -- Consumer-3

Consumer-3 宕机 -> Rebalance:
  P0 -- Consumer-1
  P1 -- Consumer-2
  P2 -- Consumer-2  接管 P2
```

**减少影响：**
- 合理设置 session.timeout.ms
- Cooperative Rebalancing (Kafka 2.4+)
- 固定分区数

---

## 关键配置

```properties
acks=all                    # 等待所有副本确认
retries=3                   # 重试次数
linger.ms=5                 # 批量发送等待
batch.size=16384            # 批次 16KB
compression.type=snappy     # 压缩

enable.auto.commit=false    # 手动提交
auto.offset.reset=earliest  # 最早开始
max.poll.records=500        # 每次拉取条数
```

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| 分区的作用 | 提高并行度、水平扩展 |
| 消息顺序保证 | 同 key 进同分区（分区内有序） |
| Consumer Group 作用 | 组内竞争消费，组间独立 |
| Rebalance 影响 | 期间消费暂停，应避免频繁触发 |
| 分区数建议 | 通常 = 消费者数 = CPU 核数 |

> [!tip]- **工程要点**
> 性能与分区数相关——分区太多增加选举和句柄开销。建议分区数不超过 1000/集群。手动提交 offset，处理成功后再提交。

---

消息可靠性保证详解见 → [03b2-Message Delivery Guarantees (消息可靠性)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/03-Message%20Queues%20(消息队列)/03b-Kafka%20Architecture%20Overview%20(Kafka架构概览)/03b2-Message%20Delivery%20Guarantees%20(消息可靠性).md)

---

## Message Delivery and Idempotency (消息可靠性与幂等)

> [!note] 本节重点心考点：> 消息可靠性三语义、ACK 机制、幂等生产者、事务、三端保证

> [!warning] “Exactly Once” 必须说明边界
> Kafka 的幂等与事务能约束 Kafka 内部的写入/消费链路；把消息处理结果写进 MySQL 等外部系统时，不能仅凭一段本地 SQL 就宣称端到端 exactly-once。通常要使用幂等写入、去重键、outbox/inbox 或可恢复的状态机。

## Kafka 消息可靠性语义

| 语义 | 说明 | 设置方式 |
|------|------|---------|
| At Most Once | 最多一次（可能丢） | acks=0 / 自动 commit |
| At Least Once | 至少一次（可能重复） | acks=all + 手动 commit |
| Exactly Once | 恰好一次 | 幂等生产者 + 事务 |

---

## 生产者端

### ACK 机制

```properties
acks=0      # 发完即走，不管是否写入（吞吐最高，可能丢）
acks=1      # Leader 写入成功即返回（不等待 Follower）
acks=all    # Leader + 所有 ISR 副本写入成功（最可靠）
```

### 幂等生产者

```properties
enable.idempotence=true   # Kafka 0.11+
```

### 事务性写入

```properties
transactional.id=my-txn-id
```

---

## Broker 端

```properties
replication.factor=3          # 3 副本
min.insync.replicas=2         # 最少同步副本数
```

Leader 崩溃 -> 从 ISR（In-Sync Replicas）中选举新 Leader

---

## 消费者端

```cpp
// 手动提交 offset（处理完再提交，librdkafka 回调模式）
class ConsumerCb : public RdKafka::ConsumeCb {
    void consume_cb(RdKafka::Message& msg, void*) override {
        if (msg.err()) return;                  // 出错跳过
        process(msg.payload(), msg.len());       // 先处理
        // librdkafka 内部自动管理 offset，或手动存储
    }
};
```

### 外部系统的处理一致性

```
Kafka -> MySQL 的常见目标是“至少一次投递 + 幂等落库”：
BEGIN TRANSACTION;
  INSERT ... ON DUPLICATE KEY UPDATE ...;  -- 用业务唯一键去重
  UPDATE consumer_offsets SET offset=X;
COMMIT;
仍需设计崩溃恢复：offset 与业务状态的存储边界、重试和补偿不能靠假设自动原子化。
```

---

## 消息不丢失总结

```
Producer -> Broker -> Consumer
  ①          ②         ③

① Producer：acks=all + 重试 + 幂等
② Broker：replication.factor=3 + min.insync.replicas=2
③ Consumer：手动提交 + 处理成功再 commit
```

---

## 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| Exactly Once 三层 | 生产者幂等、broker 副本、消费者事务 |
| 幂等与事务区别 | 幂等防重试重复，事务跨分区原子 |
| 重复消费原因 | Rebalance、消费超时、手动提交失败 |
| 消费者幂等实现 | UPSERT、去重表、状态机 |

> [!tip]- **工程要点**
> 生产推荐 acks=all + enable.idempotence=true + 手动 commit。追求极致吞吐可降为 acks=1，但需接受极端情况可能丢消息。

## 30 秒回答

Kafka 的投递语义来自生产、复制与消费确认的组合：`acks=all` 和幂等生产者降低写入丢失/重复风险，消费者在业务成功后提交 offset 得到 at-least-once。若副作用进入数据库或第三方服务，必须额外设计幂等键与恢复流程；“exactly-once”永远要先问它覆盖到哪里。

## 自测

1. 为什么手动提交 offset 仍可能造成重复消费？
2. Kafka transaction 能否让 Kafka 与 MySQL 自动成为同一个原子事务？
3. 业务去重键应选择消息 ID、订单 ID 还是两者组合？为什么？

---

Kafka 核心概念详解见 → [03b1-Topic, Partition, Consumer Group (核心概念)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/03-Message%20Queues%20(消息队列)/03b-Kafka%20Architecture%20Overview%20(Kafka架构概览)/03b1-Topic,%20Partition,%20Consumer%20Group%20(核心概念).md)



## 零基础阅读路径

先写出业务不变量和“数据真相在哪里”；再读本地事务或缓存流程；最后处理副本、消息、故障和一致性。若没有失败场景，分布式结论没有意义。

## 常见误区

- 把存储或分布式结论脱离一致性、失败窗口和数据规模来背，容易在工程中套错。
- 没有通过事务、并发读写、故障注入或指标观察验证关键假设。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Kafka Delivery and Idempotency (Kafka 投递与幂等)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
