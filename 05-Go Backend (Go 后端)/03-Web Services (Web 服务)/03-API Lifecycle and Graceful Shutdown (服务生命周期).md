---
study_stage: backlog
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

推荐顺序：readiness=false 并按部署条件等待流量摘除；停止接收新工作；调用 `Server.Shutdown` 排空普通 HTTP 请求；再按依赖关系停止后台 producer、关闭任务队列并等待 worker；flush 有界缓冲；最后关闭数据库等依赖。不要在排空前无差别取消所有在途请求的父 context。

顺序取决于依赖图：仍在使用 DB 的 worker 退出前不能先关 DB。所有等待都有总 deadline，超时后记录未完成数量和 goroutine/任务证据，再决定强制退出。

# 后台任务与错误汇聚

每个 goroutine 都必须属于一个生命周期：由根 context 取消，有错误出口，并在主函数等待。启动后丢弃返回值的 goroutine 会让服务“HTTP 正常但后台早已死亡”。可用 `errgroup` 或显式 channel 汇聚首个致命错误并取消同组任务。

定时任务用 ticker 时 `defer ticker.Stop()`；每轮避免重叠执行或明确允许并发。队列 consumer 停止拉取后再排空已接收任务，确保 ack、事务和进程退出的关系清晰。

# Kubernetes/进程管理边界

容器环境的 SIGTERM、termination grace period、readiness 探针和 preStop 必须与应用 shutdown timeout 协调。若平台只给 30 秒而程序内部最多等 60 秒，最终仍会被强杀。PID 1、信号转发和 shell wrapper 也要在目标镜像验证。

# 故障演练

在请求处理中发送 SIGTERM，确认新请求被摘除、在途请求在期限内完成；让一个 handler 永不返回，确认总 deadline 后进程能退出并留下证据；让后台 worker 先失败，确认主进程不会继续假装健康。
# Shutdown 的边界与演练

`Server.Shutdown(ctx)` 会关闭 listener、拒绝新连接，并等待当前普通 HTTP 请求结束；传入的 `ctx` 是**关闭操作的预算**，应从未取消的独立 context 派生。若它超时返回，尚未结束的请求可能仍在运行，进程必须有明确的强制退出策略及未完成工作记录。

`Shutdown` 不自动等待 hijacked 连接（如 WebSocket）或任意后台 goroutine；这些资源需要应用自己发停止信号、等待并设置上限。演练时分别制造慢请求、永不返回的 handler 和长连接，观察 readiness、监听返回、关闭耗时与已完成请求数。

官方依据：[net/http Server.Shutdown](https://pkg.go.dev/net/http#Server.Shutdown)。
