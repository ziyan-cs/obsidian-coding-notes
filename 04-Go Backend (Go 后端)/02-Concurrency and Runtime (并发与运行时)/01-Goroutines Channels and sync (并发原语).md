---
tags: [language/go, go/concurrency]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

> [!abstract] 一句话结论：goroutine 很轻，但泄漏、竞态和死锁仍真实存在；先定义所有权、退出条件和背压，再选 channel 或 mutex。

> [!warning] 并发代码先写“退出设计”
> 在启动 goroutine 前，能明确回答“谁取消、谁关闭、谁等待、谁消费结果”，才开始写业务逻辑。否则功能即使暂时可用，也容易在超时或错误路径泄漏。

# 有取消的 worker 示例

```go
func worker(ctx context.Context, jobs <-chan int, out chan<- int) {
    for {
        select {
        case <-ctx.Done():
            return
        case n, ok := <-jobs:
            if !ok { return }
            select {
            case out <- n * n:
            case <-ctx.Done(): return
            }
        }
    }
}
```

# 选择工具

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 任务交接、流水线、需要背压 | channel | 数据流和阻塞语义清楚 |
| 保护共享 map/状态 | `sync.Mutex` | 比绕一圈 channel 更直接 |
| 一次性初始化 | `sync.Once` | 明确表达意图 |
| 等待一批 goroutine | `sync.WaitGroup` | 只负责等待，不传结果 |

# 必问三题

1. 谁关闭 channel？通常是发送方；接收方不应猜测关闭时机。
2. goroutine 如何退出？必须有输入关闭、context 取消或明确完成条件。
3. 接收方变慢时会怎样？无缓冲 channel 会阻塞发送方；缓冲不是无限队列。

# 常见坑

- “用 channel 就线程安全”：共享变量仍可能 race；运行 `go test -race ./...`。
- 多个发送者都 `close(ch)`：会 panic。
- 启动 goroutine 却从未等待、取消或消费其输出：泄漏。
- 用 `time.Sleep` 同步测试：改用 channel、WaitGroup 或 context。

# 30 秒回答

goroutine 是 Go 调度器管理的轻量执行单元；channel 适合表达任务交接、顺序与背压，`Mutex` 适合直接保护共享状态。二者不是互斥的架构阵营，关键是明确数据所有权和退出路径。每个 goroutine 都要能因 `context` 取消、输入关闭或任务完成而退出。

# 自测

1. 为什么通常只能由发送方关闭 channel？多个发送方如何安全地收口？
2. 给一个 worker pool 设计取消路径：阻塞在收任务和发结果时各如何响应 `ctx.Done()`？
3. 一个缓存 map 同时读写，为什么“改成 channel”未必比 `Mutex` 更好？

# C++ 对照

Go channel 类似“带同步语义的消息通道”，不是 `std::queue`；mutex 与 C++ mutex 同样需要保护不变量。Go 不让你免于理解并发，只减少线程创建和调度细节。

# 从零建立模型

本页主题是 **01-Goroutines Channels and sync (并发原语)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Goroutines Channels and sync (并发原语)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
