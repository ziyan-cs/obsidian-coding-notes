---
study_stage: backlog
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

# Kafka Core Concepts (Kafka核心概念)

> [!note] 本节重点： Topic/Partition/Consumer Group、分区机制、消息有序性、消费者 Rebalance

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
- **组内并行度受分区数限制**：同一消费组中，一个分区在稳定分配期间只分配给一个消费者；跨消费组互不影响，分区数也不是吞吐的唯一决定因素

### Consumer Group

同一组内的成员分摊 topic 分区；不同组各自维护消费进度。`offset` 是分区内位置，不是全局消息 ID。消费成功和提交 offset 是两个动作：如果副作用已落库但 offset 尚未提交就崩溃，重启后会再次读到该消息。

---

## 分区与消息路由

```cpp
// 生产者决定消息写入哪个分区
// 1. 指定分区 -> 直接写入（librdkafka API）
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_PARTITION(0),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);

// 2. 有 key -> 由客户端分区器按 key 选分区；具体算法随客户端/配置变化
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_KEY("user123", 7),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);

// 3. 无 key -> 使用客户端默认分区器；不要假定总是轮询
rd_kafka_producev(rk, RD_KAFKA_V_TOPIC("topic"),
    RD_KAFKA_V_VALUE("value", 5), RD_KAFKA_V_END);
```

同一 key 在**分区数与分区算法不变**时通常落到同一分区；扩容分区或更换分区器后可能改变映射。需要严格按业务键保序时，应显式规定路由规则并设计迁移。

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
- 避免把可用性与顺序性假设绑定在固定分区数上；变更时先评估 key 路由

---

## 关键配置

```properties
acks=all                    # 按当前 ISR 与 min.insync.replicas 约束确认，不等于所有配置副本
enable.idempotence=true     # 生产者重试去重；需核对依赖客户端版本的限制
compression.type=snappy     # 示例值；按真实负载比较压缩率、CPU 与延迟

enable.auto.commit=false    # 消费者关闭自动提交；还须显式实现成功后的提交
auto.offset.reset=earliest  # 仅在消费组没有有效已提交位点等情况下生效
```

---

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | 分区的作用 | 提高并行度、水平扩展 |
> | 消息顺序保证 | 同 key 进同分区（分区内有序） |
> | Consumer Group 作用 | 组内竞争消费，组间独立 |
> | Rebalance 影响 | 期间消费暂停，应避免频繁触发 |
> | 分区数规划 | 以目标吞吐、单分区负载、组内并行度、扩容与顺序范围综合决定 |
>

> [!tip]- **工程要点**
> 分区过多增加元数据、复制与文件句柄等开销，但没有通用的“集群上限 1000”规则。手动提交 offset 也不能自动保证外部数据库副作用恰好一次，必须明确失败和重放路径。

>
> ---
>
> 接下来把生产确认、broker 副本和消费位点连成同一条故障路径。

# Message Delivery and Idempotency (消息可靠性与幂等)

> [!note] 本节重点： 消息可靠性三语义、ACK 机制、幂等生产者、事务、三端保证

> [!warning] “Exactly Once” 必须说明边界
> Kafka 的幂等与事务能约束 Kafka 内部的写入/消费链路；把消息处理结果写进 MySQL 等外部系统时，不能仅凭一段本地 SQL 就宣称端到端 exactly-once。通常要使用幂等写入、去重键、outbox/inbox 或可恢复的状态机。

## Kafka 消息可靠性语义

| 语义 | 说明 | 设置方式 |
|------|------|---------|
| At most once | 尽量不重复，允许遗漏 | 例如先确认位点再执行副作用；失败可能漏处理 |
| At least once | 可重试直至成功，允许重复 | 成功处理后再确认位点；重放时业务需幂等 |
| Exactly once（限定范围） | 某个定义好的结果只生效一次 | Kafka 内部可组合事务、幂等与 `read_committed`；外部系统需另设计原子边界 |

---

## 生产者端

### ACK 机制

```properties
acks=0      # 不等待 broker 响应；可能丢失，也不能据此断言吞吐必然最高
acks=1      # Leader 写入成功即返回（不等待 Follower）
acks=all    # 等待当前 ISR 按配置确认；需结合 min.insync.replicas 和副本数
```

### 幂等生产者

```properties
enable.idempotence=true   # 对生产者重试引入的重复进行去重；非跨系统幂等
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
// 流程伪代码：不是某个客户端可直接编译的 API
for each record in poll():
    if processing_succeeds(record):
        commit_next_offset(record.partition, record.offset + 1)
    else:
        retry_or_pause_partition_and_alert()
```

真实客户端还需区分“本地保存位点”和“提交到 broker”、同步/异步提交失败、分区 rebalance 时的在途任务及毒消息处置。`offset + 1` 表示下一条要消费的位置，不能跳过同分区中尚未处理的更早记录。

### 外部系统的处理一致性

```
Kafka -> MySQL 的常见目标是“至少一次投递 + 幂等落库”：
BEGIN TRANSACTION;
  INSERT INTO processed_messages(consumer, message_id) ...;  -- 唯一约束做去重闸门
  执行业务更新；记录该分区已连续完成的下一 offset
COMMIT;
重启时从数据库里的业务位点恢复或重复读取并被去重表拦截；不能同时把 broker offset 和数据库位点当成互不协调的唯一真相。
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

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | Exactly once 边界 | Kafka 内部事务可覆盖 Kafka 主题之间的读-处理-写；外部副作用要另设计 |
> | 幂等与事务区别 | 幂等防重试重复，事务跨分区原子 |
> | 重复消费原因 | Rebalance、消费超时、手动提交失败 |
> | 消费者幂等实现 | UPSERT、去重表、状态机 |
>

> [!tip]- **工程要点**
> `acks=all`、幂等生产者、合理副本和消费成功后确认是可靠性起点，不是“零丢失”证明。还要实测故障注入、备份恢复、积压、毒消息以及外部副作用的幂等键。

>

> [!summary] 核心摘要
>
> Kafka 的投递语义来自生产、复制与消费确认的组合：`acks=all` 和幂等生产者降低写入丢失/重复风险，消费者在业务成功后提交 offset 得到 at-least-once。若副作用进入数据库或第三方服务，必须额外设计幂等键与恢复流程；“exactly-once”永远要先问它覆盖到哪里。

> [!question]- 自测：先回答再展开
> 1. 为什么手动提交 offset 仍可能造成重复消费？
> 2. Kafka transaction 能否让 Kafka 与 MySQL 自动成为同一个原子事务？
> 3. 业务去重键应选择消息 ID、订单 ID 还是两者组合？为什么？
>
> ---
>
> 参考 [Apache Kafka 官方文档](https://kafka.apache.org/documentation/)；具体参数默认值与客户端分区器行为以部署版本为准。

> [!info]- 延伸阅读
> - 下一步：[03-RabbitMQ and Kafka Selection (RabbitMQ 与 Kafka 选型)](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/03-Messaging%20and%20Streaming%20(消息与流)/03-RabbitMQ%20and%20Kafka%20Selection%20(RabbitMQ%20与%20Kafka%20选型).md)
