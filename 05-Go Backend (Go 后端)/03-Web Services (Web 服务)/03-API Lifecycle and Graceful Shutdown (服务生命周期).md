---
status: learning
confidence: low
content_verified: 2026-09-17
tags: [language/go, go/server]
---

> [!abstract] 学习定位
> 生产服务不是 `ListenAndServe` 后就结束；它必须响应取消、停止接收新请求、给在途请求有限时间，并关闭依赖。

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

# 启动阶段

启动按“解析配置 → 校验 → 建立依赖 → 验证必要连通性 → 启动后台任务 → 监听端口 → readiness=true”推进。每一步失败都返回错误并清理已创建资源；不要在 package `init()` 中建立网络连接。

HTTP server 显式配置读取 header、读取请求、写响应和 idle 等 timeout，并根据 streaming 场景评估。只在 handler 内设置 context timeout，无法保护服务器慢速发送 header 或无限 header 的资源风险。

```go
srv := &http.Server{
    Addr:              ":8080",
    Handler:           handler,
    ReadHeaderTimeout: 5 * time.Second,
    IdleTimeout:       60 * time.Second,
    MaxHeaderBytes:    1 << 20,
}
```

具体数值由负载和协议测量，不作为通用常数。

# 关闭状态机

推荐顺序：readiness=false 并等待负载均衡传播；停止接收新工作；取消根 context；调用 `Server.Shutdown` 等待在途请求；停止 producer；关闭任务队列并等待 worker；flush 有界缓冲；最后关闭数据库等依赖。

顺序取决于依赖图：仍在使用 DB 的 worker 退出前不能先关 DB。所有等待都有总 deadline，超时后记录未完成数量和 goroutine/任务证据，再决定强制退出。

# 后台任务与错误汇聚

每个 goroutine 都必须属于一个生命周期：由根 context 取消，有错误出口，并在主函数等待。启动后丢弃返回值的 goroutine 会让服务“HTTP 正常但后台早已死亡”。可用 `errgroup` 或显式 channel 汇聚首个致命错误并取消同组任务。

定时任务用 ticker 时 `defer ticker.Stop()`；每轮避免重叠执行或明确允许并发。队列 consumer 停止拉取后再排空已接收任务，确保 ack、事务和进程退出的关系清晰。

# Kubernetes/进程管理边界

容器环境的 SIGTERM、termination grace period、readiness 探针和 preStop 必须与应用 shutdown timeout 协调。若平台只给 30 秒而程序内部最多等 60 秒，最终仍会被强杀。PID 1、信号转发和 shell wrapper 也要在目标镜像验证。

# 故障演练

在请求处理中发送 SIGTERM，确认新请求被摘除、在途请求在期限内完成；让一个 handler 永不返回，确认总 deadline 后进程能退出并留下证据；让后台 worker 先失败，确认主进程不会继续假装健康。
# 检查清单

- [ ] readiness / health endpoint 语义不同：能否接流量 vs 进程是否活着。
- [ ] 所有请求路径能接收 `context` 取消。
- [ ] shutdown 有超时；超时后记录必要日志。
- [ ] DB、Redis、worker 等依赖有明确关闭顺序。
- [ ] 启动、监听失败与正常关闭分别处理，不能全部 `Fatal`。

> [!summary] 核心摘要
>
> 优雅关闭的顺序是：收到取消信号，停止接收新请求，让 readiness 反映不可接流量；在有限 timeout 内等待在途请求，并按依赖关系关闭 worker、连接池等资源。`context` 应从入口向下传递，监听失败、正常关闭和关闭超时也必须区分处理。

> [!question]- 自测：先回答再展开
> 1. health 与 readiness 为什么不能总用同一个 endpoint？
> 2. 为什么 `Shutdown` 的 timeout 不应复用已经被取消的根 `context`？
> 3. 有后台 worker、HTTP server 和 DB pool 时，关闭顺序如何确定？

# C++ 对照

与 C++ server 的“优雅关闭”目标相同：停止接入、排空在途任务、释放资源。差异在于 Go 通常以 `context` 从入口向下传播取消信号。

