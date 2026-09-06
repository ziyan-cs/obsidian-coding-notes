---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 一句话结论：`context.Context` 用来沿调用链传递取消、截止时间与请求范围元数据；它不是任意参数包，也不是可选的全局变量。

# 30 秒回答

`context.Context` 让请求的取消和 deadline 从入口一致地传到数据库、HTTP、RPC、worker 与 goroutine。它解决的是工作何时应停止，不承担业务参数传递；每个派生 goroutine 都必须有退出路径，并在阻塞点检查 `Done()` 或使用支持 context 的 API。

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

# 从零建立模型

本页主题是 **02-Context Cancellation and Errors (取消传播与错误)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

# 常见误区

- 用 `context.Background()` 在请求链中重新开始，丢失取消信号。
- 把数据库连接、logger 或可选业务字段塞入 context。
- goroutine 启动后没有等待、取消或关闭协议，形成泄漏。

# 使用规则

1. 请求入口创建或接收 context，向下游调用继续传递。
2. I/O 调用设置 deadline，并在循环、worker 和 select 中响应取消。
3. 返回错误时保留语义与上下文；调用方据此决定重试、降级或返回状态码。
4. 不把业务可选参数塞进 context；它们应显式出现在函数签名或配置结构中。

# 自测

客户端断开后，一个正在访问数据库与消息队列的请求为何还会继续消耗资源？怎样让它停止？

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Context Cancellation and Errors (取消传播与错误)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Concurrency and Runtime Map (并发与运行时导航)](/04-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/00-Concurrency%20and%20Runtime%20Map%20(并发与运行时导航).md)
- 下一步：[03-Scheduler GC and Memory (调度 GC 与内存)](/04-Go%20Backend%20(Go%20后端)/02-Concurrency%20and%20Runtime%20(并发与运行时)/03-Scheduler%20GC%20and%20Memory%20(调度%20GC%20与内存).md)
