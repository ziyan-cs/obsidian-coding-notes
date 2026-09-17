---
status: stable
confidence: medium
verified: 2026-09-17
---

> [!abstract] 一句话结论：服务是否健康不能只看“进程还在”；必须能从指标、日志和追踪判断请求是否成功、慢在哪里、失败由谁造成。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：metrics 看总体趋势与告警，structured log 解释单次事件，trace 串起跨服务链路；三者使用同一请求标识，才能从症状追到原因。

# 最小信号

- Metrics：请求量、错误率、延迟分位数、饱和度、goroutine 与连接池使用量。
- Logs：结构化字段、request ID、错误链；避免记录密钥和完整隐私数据。
- Traces：跨 HTTP、数据库、消息调用关联一次请求。
- Operations：readiness/liveness、优雅关闭、配置校验、容量与回滚预案。

# 先建立一条可追踪请求

```text
request_id
    -> access log: method, route, status, latency
    -> metric: request_total and duration histogram
    -> trace: downstream database or HTTP span
```

同一个 `request_id` 贯穿入口与下游日志，才能把“用户说慢”还原为一次请求。指标看趋势和告警，日志看离散事件，trace 看一次调用的依赖关系；三者互补，不能相互替代。

# 上线前最小验收

1. readiness 失败时不接新流量；liveness 只判断进程是否需要重启，不承担依赖健康判断。
2. 配置缺失或格式错误应在启动时失败，而不是收到第一个请求才暴露。
3. 仪表盘至少有请求量、错误率、P50/P95/P99、并发或队列饱和度；阈值来自 SLO 和基线测量。

> [!question]- 自测：先回答再展开
> 只有 P50 正常、P99 变差时，为什么用户仍会明显感到服务变慢？
