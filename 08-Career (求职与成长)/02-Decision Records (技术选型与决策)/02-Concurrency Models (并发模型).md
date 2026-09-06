---
tags: [comparison, concurrency]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Concurrency Models — 线程、goroutine、asyncio

> **结论**：并发模型改变表达方式，不会消除共享状态、取消、背压和错误传播问题。

| 维度 | C++ | Go | Python |
| --- | --- | --- | --- |
| 常用单元 | `std::thread` / thread pool | goroutine | thread / process / coroutine |
| 协调 | mutex、CV、atomic | channel、mutex、context | queue、lock、asyncio task |
| CPU 并行 | 直接线程/任务 | runtime 调度到多核 | 常用多进程；理解 GIL/实现差异 |
| 主要风险 | data race、deadlock | goroutine leak、race、deadlock | 阻塞 event loop、共享可变状态 |

**选择规则**：共享可变状态少、任务传递明显时偏消息通道；保护一个清晰不变量时 mutex 更直接。所有方案先定义取消和背压。

## 先画四条边界

在选择 `thread`、goroutine 或 coroutine 前，先写清：

1. 谁创建任务，谁负责等待其结束；
2. 取消如何传播，超时由谁定义；
3. 生产速度超过消费速度时如何 backpressure；
4. 错误如何回到请求边界并被记录。

> [!warning] 常见误区
> channel 不是 mutex 的替代品，async 也不是并行。一个共享不变量仍需要明确的同步策略；CPU 密集型 Python coroutine 仍会阻塞 event loop。

## 30 秒回答与自测

**回答**：并发模型只改变任务调度和通信表达，不能消除状态一致性问题。先定义任务的所有者、退出条件、背压和错误路径，再选线程、channel 或 async。

- 自测：一个消费者停住时，生产者是阻塞、丢弃、限流还是持久化？为什么？
- 自测：Go service 关闭时，怎样证明没有 goroutine leak？
