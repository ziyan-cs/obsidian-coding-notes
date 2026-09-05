---
tags: [language/go, go/server]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# API Lifecycle & Graceful Shutdown — 服务如何正确停止

> **一句话结论**：生产服务不是 `ListenAndServe` 后就结束；它必须响应取消、停止接收新请求、给在途请求有限时间，并关闭依赖。

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

## C++ 对照

与 C++ server 的“优雅关闭”目标相同：停止接入、排空在途任务、释放资源。差异在于 Go 通常以 `context` 从入口向下传播取消信号。
