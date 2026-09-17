---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习定位：从数据真相、业务不变量和故障窗口出发，理解事务、缓存、消息与分布式协调的边界。

> [!summary]- 复述检查：学完后再展开
>
> **回答**：消息队列通过异步缓冲解耦生产者和消费者，但引入重复、乱序、积压和最终一致性。可靠设计依赖确认、重试、死信、幂等消费和可观测积压，而不是假设 exactly-once。

# Why Message Queue (消息队列价值)

> [!note] 本节重点： 消息队列三大作用、同步 vs 异步对比、MQ 引入的问题

# 消息队列三大作用

## 1. 解耦

```
不使用 MQ：强耦合
订单服务 -> 库存服务 / 积分服务 / 通知服务
新增服务时需修改订单服务代码

使用 MQ：松耦合
订单服务 -> [MQ] -> 库存/积分/通知服务
新增服务只需订阅 MQ，无需改订单代码
```

## 2. 削峰填谷

```
不使用 MQ：瞬时 10 倍峰值 -> MySQL 被打垮
使用 MQ：请求 -> [MQ] -> 消费者按自身速率消费 -> DB 平稳

削峰的实质：变瞬时高峰为持续平峰，系统按均量设计
```

## 3. 异步处理

```
synchronous path
  request -> order -> inventory -> points -> notification -> response
  user latency includes every synchronous dependency

asynchronous path
  request -> order -> message broker -> response
  broker -> inventory / points / notification consumers
  user latency excludes later work, but delivery and consistency must be designed
```

---

# 引入 MQ 的问题

| 问题 | 说明 | 解决方案 |
|------|------|---------|
| 系统可用性降低 | MQ 宕机则流程中断 | 集群、持久化 |
| 复杂度增加 | 重复消费、丢消息 | 幂等设计、重试 |
| 一致性难保证 | 异步导致分布式事务 | 最终一致性+补偿 |
| 延迟增加 | 引入网络和排队 | 选低延迟 MQ |

---

# MQ 选型对比

| 特性 | Kafka | RabbitMQ | RocketMQ |
|------|-------|----------|----------|
| 吞吐量 | 极高(百万/秒) | 中(万/秒) | 高(十万/秒) |
| 延迟 | 毫秒级 | 微秒级 | 毫秒级 |
| 消息可靠性 | 高(副本复制) | 高(confirm) | 高(同步刷盘) |
| 适用场景 | 日志/流处理 | 业务消息 | 金融/电商 |

---

> [!example]- 题型索引
> | 题型 | 要点 |
> |------|------|
> | MQ 三大作用 | 解耦、削峰、异步 |
> | 引入 MQ 的代价 | 可用性降低、复杂度增加、一致性问题 |
> | 何时用 MQ | 需要异步+解耦的场景 |
> | 最大坑 | 重复消费（一定要幂等）和消息堆积（监控消费延迟） |
>
> > [!tip]- **工程要点**
> > 核心判断标准——是否需要异步+解耦。简单同步调用强行引入 MQ 是过度设计。
>
> ---
>
> 消息队列选型对比详解见 → RabbitMQ vs Kafka：Comparison (两者对比)
