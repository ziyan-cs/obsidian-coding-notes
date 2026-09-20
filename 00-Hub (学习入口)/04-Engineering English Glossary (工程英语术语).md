---
study_stage: reference
tags: [english/engineering, learning/glossary]
---

> [!abstract] 学习定位：不背脱离语境的单词；每次在代码、报错、文档中遇到术语时，补一个自己能用的句子。

> [!summary] 核心摘要
>
> 工程英语要和真实语境绑定：记录术语的中文含义、代码或报错上下文，以及自己能复述的一句话。目标是读懂文档和沟通设计，而不是孤立背单词。

# 阅读术语的方法

技术词很少只有一个固定译法。先判断它在句子里扮演的角色：对象、动作、约束还是结果。例如 `allocate` 是“分配”这个动作，`allocation` 是一次分配或分配行为，`allocator` 才是执行分配的组件。阅读文档时优先保留英文概念，再用中文解释当前语境。

| 阅读步骤 | 要问的问题 | 示例 |
| --- | --- | --- |
| 找主体 | 谁在做事 | runtime schedules goroutines |
| 找动作 | 发生了什么 | retries the request |
| 找条件 | 何时成立 | when the timeout expires |
| 找边界 | 不保证什么 | ordering is not guaranteed |

# 运行时与并发

| Term | 常用中文 | 工程语境 | 容易误解 |
| --- | --- | --- | --- |
| lifetime | 生命周期 | object lifetime ends here | 不等于变量作用域一定结束 |
| ownership | 所有权 | transfer ownership to the caller | 指资源释放责任，不只是“能访问” |
| allocation | 内存分配 | reduce heap allocations | 不一定发生在堆上，要看上下文 |
| blocking | 阻塞 | the call blocks until data arrives | 阻塞线程与等待协程不是一回事 |
| concurrency | 并发 | handle tasks in overlapping time | 不保证同一时刻并行 |
| parallelism | 并行 | run work on multiple cores | 不等于程序一定更快 |
| contention | 竞争 | lock contention increases latency | 多个执行者争用同一资源 |
| backpressure | 背压 | apply backpressure when the queue is full | 向上游传播容量限制 |
| throughput | 吞吐量 | requests processed per second | 高吞吐不能推出低延迟 |
| tail latency | 尾延迟 | P99 tail latency regressed | 描述最慢一小部分请求 |

# 网络与数据

| Term | 常用中文 | 工程语境 | 关注点 |
| --- | --- | --- | --- |
| payload | 载荷 | decode the response payload | 通常不含协议头 |
| framing | 分帧 | length-prefixed framing | 字节流怎样划分为完整消息 |
| handshake | 握手 | complete the TLS handshake | 建立能力、身份或参数共识 |
| timeout | 超时 | set a timeout for the operation | timeout 是时长，deadline 是绝对截止时刻 |
| retry | 重试 | retry transient failures with backoff | 要考虑幂等与重试上限 |
| idempotency | 幂等性 | repeated requests have the same effect | 不是“只执行一次” |
| consistency | 一致性 | accept eventual consistency | 要说明对象、观察者和时间边界 |
| durability | 持久性 | committed data survives a crash | 仍取决于约定的故障模型 |
| replication | 复制 | replicate data across nodes | 增加可用性，也引入一致性问题 |
| partition | 分区／网络分区 | partition data by key | 根据上下文判断数据切分或网络隔离 |

# 工程交付与运行

| Term | 常用中文 | 工程语境 |
| --- | --- | --- |
| artifact | 构建产物 | publish a versioned artifact |
| dependency | 依赖 | pin direct dependencies |
| regression | 回归／退化 | detect a performance regression |
| observability | 可观测性 | infer internal state from emitted signals |
| rollout | 灰度展开 | gradually roll out the new version |
| rollback | 回滚 | roll back after error rate rises |
| incident | 线上事故 | declare and mitigate an incident |
| root cause | 根因 | distinguish root cause from trigger |
| mitigation | 缓解措施 | restore service before the final fix |
| deprecation | 弃用 | the API is deprecated but still available |

# 维护与复习

- 新词优先记录在对应主题笔记，只有跨主题高频词才加入这里。
- 每个术语至少写一个来自代码、日志或官方文档的真实搭配，不只抄中文释义。
- 复习时遮住“常用中文”，尝试解释它在当前句子中的约束与反例。
- 已经能在设计说明中自然使用的词不再机械背诵。

> [!info]- 关联
> - 导航：[00-Vault Map (知识库地图)](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md)
> - 复盘：[05-Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)
