> **核心考点**：Topic/Partition/Consumer Group、分区机制、消息有序性、消费者 Rebalance

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
# 同 group.id 的消费者组成一个消费组
# 一条消息只能被同一个组内的一个消费者消费
# 不同组的消费者独立消费同一条消息

# 分区数 = 消费者数 -> 一对一
# 分区数 > 消费者数 -> 部分消费者消费多个分区
# 分区数 < 消费者数 -> 部分消费者空闲
# 最佳实践：消费者数 = 分区数
```

---

## 分区与消息路由

```java
// 生产者决定消息写入哪个分区
// 1. 指定分区 -> 直接写入
ProducerRecord("topic", 0, "key", "value");

// 2. 有 key -> hash(key) % 分区数 （相同key保证同一分区）
ProducerRecord("topic", "user123", "value");

// 3. 无 key -> 轮询（round-robin）
ProducerRecord("topic", "value");
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
# 生产者
acks=all                    # 等待所有副本确认
retries=3                   # 重试次数
linger.ms=5                 # 批量发送等待
batch.size=16384            # 批次 16KB
compression.type=snappy     # 压缩

# 消费者
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

> **工程要点**：性能与分区数相关——分区太多增加选举和句柄开销。建议分区数不超过 1000/集群。手动提交 offset，处理成功后再提交。
