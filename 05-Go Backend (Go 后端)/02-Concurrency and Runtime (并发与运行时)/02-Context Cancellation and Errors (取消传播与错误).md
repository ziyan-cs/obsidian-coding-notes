---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习定位
> `context.Context` 用来沿调用链传递取消、截止时间与请求范围元数据；它不是任意参数包，也不是可选的全局变量。

> [!summary] 核心摘要
>
> `context.Context` 让请求的取消和 deadline 从入口一致地传到数据库、HTTP、RPC、worker 与 goroutine。它解决的是工作何时应停止，不承担业务参数传递；每个派生 goroutine 都必须有退出路径，并在阻塞点检查 `Done()` 或使用支持 context 的 API。

# 取消传播检查表

1. handler 是否将 request context 传给 service？
2. service 是否将它传给数据库、HTTP client、消息操作？
3. worker 的 `select` 是否监听 `ctx.Done()`？
4. 超时后是否停止重试和后台写入，而不是仅向客户端返回错误？

# 最小取消示例

```go
func work(ctx context.Context) error {
    select {
    case <-time.After(time.Second):
        return nil
    case <-ctx.Done():
        return ctx.Err()
    }
}

ctx, cancel := context.WithTimeout(context.Background(), 50*time.Millisecond)
defer cancel()
err := work(ctx) // err is context.DeadlineExceeded
```

`cancel` 必须被调用，即使 timeout 最终会触发；这样可尽早释放派生 context 关联的资源。真实 I/O 应优先使用 `http.NewRequestWithContext`、`QueryContext` 等支持 context 的 API，而不是只在入口处检查一次。

# 谁创建，谁负责取消

context 形成一棵派生树：父节点取消会递归通知所有子节点，子节点取消不会影响父节点或兄弟节点。创建 `WithCancel`、`WithTimeout`、`WithDeadline` 的一方拥有对应的 `CancelFunc`，应在工作完成后调用它。

```text
request context
├── database timeout (80 ms)
├── inventory RPC timeout (120 ms)
└── audit task
```

请求范围 goroutine 通常继承 request context；真正需要脱离请求继续执行的工作不能简单换成 `context.Background()` 后放任不管，而应交给拥有独立生命周期、队列、重试和关闭协议的后台组件。

Go 1.20+ 的 `context.WithCancelCause` 可以保留取消原因。调用方仍用 `errors.Is(err, context.Canceled)` 判断取消类别，用 `context.Cause(ctx)` 获取更具体的诊断原因；不要让内部原因直接泄漏为 HTTP 响应。

# 并发子任务与错误收敛

多个子任务属于同一个请求时，需要“一个失败就取消其余任务，并等待全部退出”。可以使用 `errgroup.WithContext`，也可以显式组合 `WaitGroup`、错误通道与 cancel；关键不是库名，而是不能提前返回后留下 goroutine。

```go
g, ctx := errgroup.WithContext(ctx)
g.Go(func() error { return loadUser(ctx) })
g.Go(func() error { return loadOrders(ctx) })
if err := g.Wait(); err != nil {
    return fmt.Errorf("load dashboard: %w", err)
}
```

通道发送也可能永久阻塞。生产者应同时监听取消：

```go
select {
case out <- value:
    return nil
case <-ctx.Done():
    return ctx.Err()
}
```

# 错误语义与测试

`context.Canceled` 表示主动取消，`context.DeadlineExceeded` 表示截止时间已过；底层库可能包装它们，因此用 `errors.Is`，不要比较错误字符串。服务端还要区分客户端取消、自己设置的依赖超时和内部故障，它们对应不同日志级别、指标与 HTTP/RPC 映射。

测试不要依赖长时间 `Sleep`。使用短但有余量的 timeout，或通过通道确认 worker 已启动，再执行 cancel 并等待退出：

```go
ctx, cancel := context.WithCancel(context.Background())
done := make(chan struct{})
go func() { defer close(done); _ = work(ctx) }()
cancel()
select {
case <-done:
case <-time.After(time.Second):
    t.Fatal("worker did not stop after cancellation")
}
```

官方参考：[Go blog: Context](https://go.dev/blog/context)、[context package](https://pkg.go.dev/context)、[errgroup package](https://pkg.go.dev/golang.org/x/sync/errgroup)。

> [!warning]- 易错点
> - 用 `context.Background()` 在请求链中重新开始，丢失取消信号。
> - 把数据库连接、logger 或可选业务字段塞入 context。
> - goroutine 启动后没有等待、取消或关闭协议，形成泄漏。

# 使用规则

1. 请求入口创建或接收 context，向下游调用继续传递。
2. I/O 调用设置 deadline，并在循环、worker 和 select 中响应取消。
3. 返回错误时保留语义与上下文；调用方据此决定重试、降级或返回状态码。
4. 不把业务可选参数塞进 context；它们应显式出现在函数签名或配置结构中。

> [!question]- 自测：先回答再展开
> 客户端断开后，一个正在访问数据库与消息队列的请求为何还会继续消耗资源？怎样让它停止？

> [!info]- 延伸阅读
> - 下一步：[03-Scheduler GC and Memory (调度 GC 与内存)](/05-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/03-Scheduler%20GC%20and%20Memory%20(调度%20GC%20与内存).md)

