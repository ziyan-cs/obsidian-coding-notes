---
status: learning
confidence: low
verified: 2026-09-17
tags: [comparison, concurrency]
---

> [!abstract] 学习定位：结论：并发模型改变表达方式，不会消除共享状态、取消、背压和错误传播问题。

> [!summary]- 复述检查：学完后再展开
> **回答**：线程、协程、goroutine 和事件循环改变调度与表达方式，却都必须处理共享状态、取消、背压和错误传播。选型依据是工作负载与所有权模型，不是抽象看起来更轻。

# 先画四条边界

在选择 `thread`、goroutine 或 coroutine 前，先写清：

1. 谁创建任务，谁负责等待其结束；
2. 取消如何传播，超时由谁定义；
3. 生产速度超过消费速度时如何 backpressure；
4. 错误如何回到请求边界并被记录。

# 选型不是语言偏好

| 工作类型 | 常见起点 | 仍需明确的边界 |
| --- | --- | --- |
| 少量阻塞任务 | thread / worker pool | 队列上限、关闭与等待 |
| 大量 I/O 等待 | goroutine / async | timeout、取消、连接上限 |
| CPU 密集计算 | bounded worker / process | 核数、抢占、结果汇总 |
| 严格顺序状态 | 单 owner / actor-like loop | mailbox 容量、失败恢复 |

先从最简单可验证的模型开始。当任务量增长，只有在 profile、排队长度或尾延迟显示瓶颈时才增加复杂并发；并发度提升吞吐，也可能放大下游故障。

# 面试表达模板

“我不先按线程、协程或 channel 选方案，而是先定义任务所有者、取消、背压和错误回收。模型只改变调度与通信方式，共享状态和资源上限仍要显式设计。”

> [!warning] 常见误区
> channel 不是 mutex 的替代品，async 也不是并行。一个共享不变量仍需要明确的同步策略；CPU 密集型 Python coroutine 仍会阻塞 event loop。

> [!summary]- 复述检查：学完后再展开
> **回答**：并发模型只改变任务调度和通信表达，不能消除状态一致性问题。先定义任务的所有者、退出条件、背压和错误路径，再选线程、channel 或 async。
>
> - 自测：一个消费者停住时，生产者是阻塞、丢弃、限流还是持久化？为什么？
> - 自测：Go service 关闭时，怎样证明没有 goroutine leak？

> [!info]- 延伸阅读
> - 下一步：[03-Error Handling (错误处理)](/09-Career%20(求职与成长)/04-Decision%20Records%20(决策记录)/03-Error%20Handling%20(错误处理).md)
