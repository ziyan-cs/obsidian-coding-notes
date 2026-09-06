---
status: learning
confidence: low
verified: 2026-09-06
tags: [language/go, go/server]
---

> [!abstract] 一句话结论：生产服务不是 `ListenAndServe` 后就结束；它必须响应取消、停止接收新请求、给在途请求有限时间，并关闭依赖。

> [!warning] Shutdown 不是立即杀进程
> 进程收到终止信号后，如果立刻退出，在途请求和缓冲数据可能丢失；如果永远等待，发布和故障恢复又会卡住。因此必须同时有停止接流量、有限等待和超时后的记录策略。

```go
srv := &http.Server{Addr: ":8080", Handler: mux}
errCh := make(chan error, 1)
go func() { errCh <- srv.ListenAndServe() }()

ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
defer stop()
select {
case <-ctx.Done():
case err := <-errCh:
    if !errors.Is(err, http.ErrServerClosed) { log.Printf("server: %v", err) }
}

shutdownCtx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
defer cancel()
if err := srv.Shutdown(shutdownCtx); err != nil { log.Printf("shutdown: %v", err) }
```

# 检查清单

- [ ] readiness / health endpoint 语义不同：能否接流量 vs 进程是否活着。
- [ ] 所有请求路径能接收 `context` 取消。
- [ ] shutdown 有超时；超时后记录必要日志。
- [ ] DB、Redis、worker 等依赖有明确关闭顺序。
- [ ] 启动、监听失败与正常关闭分别处理，不能全部 `Fatal`。

# 30 秒回答

优雅关闭的顺序是：收到取消信号，停止接收新请求，让 readiness 反映不可接流量；在有限 timeout 内等待在途请求，并按依赖关系关闭 worker、连接池等资源。`context` 应从入口向下传递，监听失败、正常关闭和关闭超时也必须区分处理。

# 自测

1. health 与 readiness 为什么不能总用同一个 endpoint？
2. 为什么 `Shutdown` 的 timeout 不应复用已经被取消的根 `context`？
3. 有后台 worker、HTTP server 和 DB pool 时，关闭顺序如何确定？

# C++ 对照

与 C++ server 的“优雅关闭”目标相同：停止接入、排空在途任务、释放资源。差异在于 Go 通常以 `context` 从入口向下传播取消信号。

# 从零建立模型

本页主题是 **03-API Lifecycle and Graceful Shutdown (服务生命周期)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-API Lifecycle and Graceful Shutdown (服务生命周期)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Web Services Map (Web 服务导航)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/00-Web%20Services%20Map%20(Web%20服务导航).md)
- 下一步：[04-Testing and Quality (测试与质量)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/04-Testing%20and%20Quality%20(测试与质量).md)
