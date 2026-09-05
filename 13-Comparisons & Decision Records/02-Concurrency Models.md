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
