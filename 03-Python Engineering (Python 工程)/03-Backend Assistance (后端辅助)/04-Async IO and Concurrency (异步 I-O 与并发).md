---
status: stable
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 04-Async IO and Concurrency (异步 I-O 与并发)

> [!abstract] 一句话结论：`asyncio` 适合大量可等待的 I/O；CPU 密集计算仍需进程、原生扩展或交给专门服务，不要期待协程消除计算瓶颈。

## 选择模型

| 任务 | 优先选择 | 原因 |
| --- | --- | --- |
| 少量顺序脚本 | 同步代码 | 最易读、最易调试 |
| 大量 HTTP/数据库等待 | async I/O | 等待时可切换其他任务 |
| CPU 密集转换 | 多进程或下沉到 C++ | 避免单解释器线程成为瓶颈 |
| 需要排队与限流 | worker + queue | 控制并发和失败重试 |

## 常见误区

- 在协程中调用阻塞 I/O 或长 CPU 循环，等于阻塞整个事件循环。
- 无限制 `gather` 请求，导致连接、内存或下游被压垮。
- 只设并发数，不设 timeout、取消和重试策略。
