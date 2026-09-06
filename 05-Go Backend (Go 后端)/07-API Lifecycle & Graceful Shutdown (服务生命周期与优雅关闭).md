---
tags: [language/go, go/server]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# API Lifecycle & Graceful Shutdown — 服务如何正确停止

> **一句话结论**：生产服务不是 `ListenAndServe` 后就结束；它必须响应取消、停止接收新请求、给在途请求有限时间，并关闭依赖。

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

## 检查清单

- [ ] readiness / health endpoint 语义不同：能否接流量 vs 进程是否活着。
- [ ] 所有请求路径能接收 `context` 取消。
- [ ] shutdown 有超时；超时后记录必要日志。
- [ ] DB、Redis、worker 等依赖有明确关闭顺序。
- [ ] 启动、监听失败与正常关闭分别处理，不能全部 `Fatal`。

## 30 秒回答

优雅关闭的顺序是：收到取消信号，停止接收新请求，让 readiness 反映不可接流量；在有限 timeout 内等待在途请求，并按依赖关系关闭 worker、连接池等资源。`context` 应从入口向下传递，监听失败、正常关闭和关闭超时也必须区分处理。

## 自测

1. health 与 readiness 为什么不能总用同一个 endpoint？
2. 为什么 `Shutdown` 的 timeout 不应复用已经被取消的根 `context`？
3. 有后台 worker、HTTP server 和 DB pool 时，关闭顺序如何确定？

## C++ 对照

与 C++ server 的“优雅关闭”目标相同：停止接入、排空在途任务、释放资源。差异在于 Go 通常以 `context` 从入口向下传播取消信号。
