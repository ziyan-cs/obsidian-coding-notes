---
tags: [language/go, go/concurrency]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Goroutines, Channels & sync — 并发不等于自动正确

> **一句话结论**：goroutine 很轻，但泄漏、竞态和死锁仍真实存在；先定义所有权、退出条件和背压，再选 channel 或 mutex。

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

## C++ 对照

Go channel 类似“带同步语义的消息通道”，不是 `std::queue`；mutex 与 C++ mutex 同样需要保护不变量。Go 不让你免于理解并发，只减少线程创建和调度细节。
