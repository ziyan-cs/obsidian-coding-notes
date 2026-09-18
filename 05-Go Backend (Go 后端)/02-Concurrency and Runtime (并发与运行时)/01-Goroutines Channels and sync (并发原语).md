---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [language/go, go/concurrency]
---

> [!abstract] 学习定位
> goroutine 很轻，但泄漏、竞态和死锁仍真实存在；先定义所有权、退出条件和背压，再选 channel 或 mutex。

> [!warning] 并发代码先写“退出设计”
> 在启动 goroutine 前，能明确回答“谁取消、谁关闭、谁等待、谁消费结果”，才开始写业务逻辑。否则功能即使暂时可用，也容易在超时或错误路径泄漏。

# Happens-before 与竞态

两个 goroutine 并发访问同一变量，至少一个是写且没有同步，就是 data race。结果不仅是“值可能旧”，而是程序不再具备可靠推理基础。mutex unlock/lock、channel send/receive、close 与接收等操作可建立 Go memory model 定义的 happens-before 关系。

`go test -race` 通过运行时插桩检测实际执行路径中的竞态；没报告不代表不存在，需要让测试覆盖真实并发交错。修复目标是保护完整不变量，而不是只给单个字段加锁。

```go
type Counter struct {
    mu sync.Mutex
    n  int64
}

func (c *Counter) Add(delta int64) {
    c.mu.Lock()
    defer c.mu.Unlock()
    c.n += delta
}
```

# Channel 所有权与关闭

close 表示不会再发送新值，不是销毁 channel，也不是广播任意业务事件。向已关闭 channel 发送会 panic；重复关闭会 panic；从已关闭且排空的 channel 接收立即返回零值和 `ok=false`。

通常由唯一发送者关闭；多个发送者由协调 goroutine、WaitGroup 后的单一 closer 或 context 收口。接收者无法安全判断“是否还有发送者”，因此通常不关闭输入 channel。

nil channel 的发送和接收永久阻塞，可在 select 中通过把 channel 设为 nil 动态禁用 case，但必须清楚退出条件。

## 缓冲、背压与有界队列

无缓冲 channel 要求发送与接收会合；缓冲 channel 允许有限解耦。容量是允许的在途工作量，不是性能越大越好。缓冲过大推迟过载暴露、增加内存并让任务在超时后仍排队。

worker pool 的入口应有界：队列满时阻塞、拒绝、降级或按业务丢弃，而不是继续启动 goroutine。每项工作携带 deadline，worker 在开始前也检查任务是否已经过期。

# Mutex、RWMutex 与 Atomic

mutex 适合保护一个状态不变量。临界区内避免网络、磁盘和未知回调；否则持锁时间由外部系统决定。复制已使用的 mutex 是错误，包含它的 struct 使用指针传递。

RWMutex 只有在读临界区足够长、读多写少且测量证明有益时才使用；它更复杂且不保证自然更快。atomic 适合独立计数器、标志或经过严格设计的数据结构，不适合维护多个字段的一致性。

# 生命周期与退出

为 worker pool 编写测试：输入关闭时全部退出；context 取消时阻塞在收任务和发结果的 worker 都能退出；队列满时行为符合契约。测试前后比较 goroutine 数只能作为线索，最好让每个 goroutine 都有显式 done 信号并在测试中等待。
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

# 选择工具

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 任务交接、流水线、需要背压 | channel | 数据流和阻塞语义清楚 |
| 保护共享 map/状态 | `sync.Mutex` | 比绕一圈 channel 更直接 |
| 一次性初始化 | `sync.Once` | 明确表达意图 |
| 等待一批 goroutine | `sync.WaitGroup` | 只负责等待，不传结果 |

## 核心检查

1. 谁关闭 channel？通常是发送方；接收方不应猜测关闭时机。
2. goroutine 如何退出？必须有输入关闭、context 取消或明确完成条件。
3. 接收方变慢时会怎样？无缓冲 channel 会阻塞发送方；缓冲不是无限队列。

> [!warning]- 易错点
> - “用 channel 就线程安全”：共享变量仍可能 race；运行 `go test -race ./...`。
> - 多个发送者都 `close(ch)`：会 panic。
> - 启动 goroutine 却从未等待、取消或消费其输出：泄漏。
> - 用 `time.Sleep` 同步测试：改用 channel、WaitGroup 或 context。
>

> [!summary] 核心摘要
>
> goroutine 是 Go 调度器管理的轻量执行单元；channel 适合表达任务交接、顺序与背压，`Mutex` 适合直接保护共享状态。二者不是互斥的架构阵营，关键是明确数据所有权和退出路径。每个 goroutine 都要能因 `context` 取消、输入关闭或任务完成而退出。

> [!question]- 自测：先回答再展开
> 1. 为什么通常只能由发送方关闭 channel？多个发送方如何安全地收口？
> 2. 给一个 worker pool 设计取消路径：阻塞在收任务和发结果时各如何响应 `ctx.Done()`？
> 3. 一个缓存 map 同时读写，为什么“改成 channel”未必比 `Mutex` 更好？

## C++ 对照

Go channel 类似“带同步语义的消息通道”，不是 `std::queue`；mutex 与 C++ mutex 同样需要保护不变量。Go 不让你免于理解并发，只减少线程创建和调度细节。

