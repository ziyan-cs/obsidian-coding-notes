---
study_stage: backlog
---

> [!note] 方法论坐标
> 日志、指标、追踪与 SLO 的共同语义见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)；本篇聚焦 Go 服务中的 context、middleware、runtime 指标与关闭流程。

> [!abstract] 学习定位
> 可观测性不是“多打日志”，而是让请求、依赖与资源状态能被度量、关联并用于决策。

> [!summary] 核心摘要
>
> metrics 发现趋势与告警，structured log 解释离散事件，trace 串起跨服务链路；三者共享上下文，才能从用户症状追到系统原因。

# 从服务目标选择信号

面向请求的服务先看 RED：Rate、Errors、Duration；面向资源再看 USE：Utilization、Saturation、Errors。指标必须服务于 SLO 或排障问题，而不是因为“能采集”就全部保留。

| 信号 | 擅长回答 | 不适合单独回答 |
| --- | --- | --- |
| Metrics | 是否普遍变慢、错误率是否越界 | 某次请求为什么失败 |
| Logs | 当时发生了什么、错误上下文 | 全局趋势与精确分位数 |
| Traces | 时间花在哪个依赖和 span | 长期容量趋势 |
| Profiles | CPU、内存、锁热点在哪里 | 单次业务请求语义 |

延迟应使用 histogram 等可聚合分布计算分位数；不要对各实例已经算出的 P99 再做平均。标签不能放用户 ID、原始 URL 或随机 request ID 等高基数字段，否则时序数量会失控。

# 一条可关联的请求

```text
trace_id / request_id
   -> access log: method, route, status, duration
   -> metric: request_total, request_duration histogram
   -> trace: HTTP -> service -> SQL / Redis / message span
```

request ID 便于日志检索，trace ID 关联完整调用树；二者可以同时存在。错误日志保留错误链、稳定错误码和关键上下文，但不得记录 token、密码、完整请求体或个人敏感信息。

# Go 中的最小实现

标准库 `log/slog` 可以输出结构化日志。将稳定字段放在 logger 上，将请求字段放在单条事件中；错误使用稳定分类字段，完整错误链作为诊断属性。

```go
logger := slog.New(slog.NewJSONHandler(os.Stdout, nil)).With(
    "service", "todo-api",
    "version", buildVersion,
)

logger.ErrorContext(ctx, "dependency call failed",
    "operation", "CreateTodo",
    "dependency", "mysql",
    "error_type", "timeout",
    "err", err,
)
```

HTTP middleware 若要记录 status 与响应字节数，需要包装 `ResponseWriter`；包装类型还要谨慎保留 `Flusher`、`Hijacker` 等可选接口，否则 streaming 或 WebSocket 行为会改变。只记录规范化 route pattern，不把包含 ID 的原始 path 当指标标签。

Go runtime 指标至少关注 goroutine、heap、GC、CPU 与文件描述符，再结合应用连接池、队列和依赖延迟。指标注册应发生一次；不要在每个请求里动态创建 collector。

OpenTelemetry 的 span 在入口创建并通过 `context.Context` 传播到 SQL、HTTP 和 RPC。只创建 span 不等于链路完整：出站请求必须注入 trace context，接收方必须提取，异步消息还要定义 producer/consumer 关系。

# 观测系统失效时

日志写入、指标导出和 trace exporter 不能无限阻塞业务请求。采用有界队列、批量、超时和丢弃计数；遥测后端不可用时业务应按设计降级。与此同时监控 exporter 失败、队列占用和丢弃量，否则“没有告警”可能只是观测管线已经断开。

官方参考：[log/slog package](https://pkg.go.dev/log/slog)、[OpenTelemetry Go](https://opentelemetry.io/docs/languages/go/)。

# 健康检查与运行边界

- startup 检查启动前置条件，避免尚未初始化就接流量。
- readiness 表示实例当前能否接收新请求；依赖故障是否导致不就绪要谨慎，避免所有实例同时摘除。
- liveness 只判断进程是否陷入必须重启的状态，不能把短暂下游故障当作重启理由。
- 优雅关闭先停止接收新流量，再等待在途请求与后台任务在期限内收敛。

pprof、调试接口和详细指标可能暴露内部信息，应限制在管理网络、鉴权或本机，不直接公开到互联网。

# 告警与事故处理

告警应指向用户影响或即将耗尽的容量，并附带运行手册。收到告警后按顺序确认影响范围、最近变更、资源饱和与依赖状态；先止损，再定位根因。

```text
symptom -> scope -> recent change -> dependency/resource evidence
        -> mitigate -> verify recovery -> preserve timeline -> root cause
```

上线前至少验证：仪表盘能显示流量、错误、尾延迟和饱和度；一条测试请求可从日志定位到 trace；readiness 与优雅关闭行为符合预期；回滚步骤真实可执行。

> [!question]- 理解检查
> P50 正常而 P99 变差时，哪些用户会受影响？为什么只看平均值可能完全漏掉故障？

> [!info]- 官方参考
> - [Go Diagnostics](https://go.dev/doc/diagnostics)
