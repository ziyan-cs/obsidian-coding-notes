---
study_stage: backlog
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

# 一个会收口的 worker 批处理

输入由 producer 唯一关闭，worker 只读 `jobs`；所有 worker 退出后，由单一 closer 关闭 `results`。调用者一直消费结果直到通道关闭，所以慢消费端形成背压，而不是让任务悄悄堆满内存。

```go
package lesson

import (
    "context"
    "errors"
    "sync"
)

type squareResult struct {
    index int
    value int
}

func squareBatch(ctx context.Context, input []int, workers int) ([]int, error) {
    if workers < 1 {
        return nil, errors.New("workers must be positive")
    }
    jobs := make(chan int)
    results := make(chan squareResult)
    var wg sync.WaitGroup

    for range workers {
        wg.Add(1)
        go func() {
            defer wg.Done()
            for {
                select {
                case <-ctx.Done():
                    return
                case index, ok := <-jobs:
                    if !ok {
                        return
                    }
                    item := squareResult{index, input[index] * input[index]}
                    select {
                    case results <- item:
                    case <-ctx.Done():
                        return
                    }
                }
            }
        }()
    }
    go func() {
        defer close(jobs)
        for index := range input {
            select {
            case jobs <- index:
            case <-ctx.Done():
                return
            }
        }
    }()
    go func() {
        wg.Wait()
        close(results)
    }()

    output := make([]int, len(input))
    for item := range results {
        output[item.index] = item.value
    }
    if err := ctx.Err(); err != nil {
        return nil, err
    }
    return output, nil
}
```

这段示例以 Go 1.22+ 的整数 `range` 语法为基线，内存仍随输入和输出大小增长；它只限定同时工作的 goroutine 数与通道缓冲。真正的无限输入需要有界队列与流式结果消费。注意整数平方可能溢出，业务代码必须按数值范围另行校验。

# 选择工具

| 场景 | 首选 | 原因 |
| --- | --- | --- |
| 任务交接、流水线、需要背压 | channel | 数据流和阻塞语义清楚 |
| 保护共享 map/状态 | `sync.Mutex` | 比绕一圈 channel 更直接 |
| 一次性初始化 | `sync.Once` | 明确表达意图 |
| 等待一批 goroutine | `sync.WaitGroup` | 只负责等待，不传结果 |

# 动手验证

测试四条路径：正常输入按原顺序得到平方；`workers=0` 返回错误；开始处理后取消 context 能返回且所有 goroutine 退出；消费者变慢时 producer 被阻塞而不是无限创建任务。用 `go test -race` 检查实际跑到的路径，并用显式完成信号验证退出，不靠 `time.Sleep` 猜测。

## C++ 对照

Go channel 类似“带同步语义的消息通道”，不是 `std::queue`；mutex 与 C++ mutex 同样需要保护不变量。Go 不让你免于理解并发，只减少线程创建和调度细节。
