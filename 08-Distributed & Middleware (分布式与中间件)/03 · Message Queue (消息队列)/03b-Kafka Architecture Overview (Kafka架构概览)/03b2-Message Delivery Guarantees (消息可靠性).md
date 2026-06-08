---
tags:
  - distributed/mq
status: 🌱
---

> **核心考点**：消息可靠性三语义、ACK 机制、幂等生产者、事务、三端保证

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
# 原理：每个请求带 (ProducerID + Sequence Number)
# Broker 去重：同一 (PID, Seq) 的消息只写一次
# 解决：网络重试导致的重复消息
```

### 事务性写入

```properties
transactional.id=my-txn-id
# 支持跨分区/跨 Topic 的原子写入
# 全部成功或全部回滚
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

### 端到端 Exactly Once

```
Kafka -> MySQL 的原子写入：
BEGIN TRANSACTION;
  INSERT INTO result VALUES (processed_data);
  UPDATE consumer_offsets SET offset=X;
COMMIT;
两者要么同时成功，要么同时失败
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

## 经典题型速查

| 题型 | 要点 |
|------|------|
| Exactly Once 三层 | 生产者幂等、broker 副本、消费者事务 |
| 幂等与事务区别 | 幂等防重试重复，事务跨分区原子 |
| 重复消费原因 | Rebalance、消费超时、手动提交失败 |
| 消费者幂等实现 | UPSERT、去重表、状态机 |

> **工程要点**：生产推荐 acks=all + enable.idempotence=true + 手动 commit。追求极致吞吐可降为 acks=1，但需接受极端情况可能丢消息。

---

Kafka 核心概念详解见 → [03b1-Topic, Partition, Consumer Group (核心概念)](/08-Distributed%20&%20Middleware%20(分布式与中间件)/03%20·%20Message%20Queue%20(消息队列)/03b-Kafka%20Architecture%20Overview%20(Kafka架构概览)/03b1-Topic,%20Partition,%20Consumer%20Group%20(核心概念).md)
