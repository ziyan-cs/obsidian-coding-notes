---
tags: [language/go, go/concurrency]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-Goroutines Channels and sync (并发原语)

> [!abstract] 一句话结论：goroutine 很轻，但泄漏、竞态和死锁仍真实存在；先定义所有权、退出条件和背压，再选 channel 或 mutex。

> [!warning] 并发代码先写“退出设计”
> 在启动 goroutine 前，能明确回答“谁取消、谁关闭、谁等待、谁消费结果”，才开始写业务逻辑。否则功能即使暂时可用，也容易在超时或错误路径泄漏。

## 有取消的 worker 示例

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

## 选择工具

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 任务交接、流水线、需要背压 | channel | 数据流和阻塞语义清楚 |
| 保护共享 map/状态 | `sync.Mutex` | 比绕一圈 channel 更直接 |
| 一次性初始化 | `sync.Once` | 明确表达意图 |
| 等待一批 goroutine | `sync.WaitGroup` | 只负责等待，不传结果 |

## 必问三题

1. 谁关闭 channel？通常是发送方；接收方不应猜测关闭时机。
2. goroutine 如何退出？必须有输入关闭、context 取消或明确完成条件。
3. 接收方变慢时会怎样？无缓冲 channel 会阻塞发送方；缓冲不是无限队列。

## 常见坑

- “用 channel 就线程安全”：共享变量仍可能 race；运行 `go test -race ./...`。
- 多个发送者都 `close(ch)`：会 panic。
- 启动 goroutine 却从未等待、取消或消费其输出：泄漏。
- 用 `time.Sleep` 同步测试：改用 channel、WaitGroup 或 context。

## 30 秒回答

goroutine 是 Go 调度器管理的轻量执行单元；channel 适合表达任务交接、顺序与背压，`Mutex` 适合直接保护共享状态。二者不是互斥的架构阵营，关键是明确数据所有权和退出路径。每个 goroutine 都要能因 `context` 取消、输入关闭或任务完成而退出。

## 自测

1. 为什么通常只能由发送方关闭 channel？多个发送方如何安全地收口？
2. 给一个 worker pool 设计取消路径：阻塞在收任务和发结果时各如何响应 `ctx.Done()`？
3. 一个缓存 map 同时读写，为什么“改成 channel”未必比 `Mutex` 更好？

## C++ 对照

Go channel 类似“带同步语义的消息通道”，不是 `std::queue`；mutex 与 C++ mutex 同样需要保护不变量。Go 不让你免于理解并发，只减少线程创建和调度细节。

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 01-Goroutines Channels and sync (并发原语) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
