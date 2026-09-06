---
status: stable
confidence: medium
verified: 2026-09-06
---

> [!abstract] 一句话结论：服务是否健康不能只看“进程还在”；必须能从指标、日志和追踪判断请求是否成功、慢在哪里、失败由谁造成。

# 30 秒回答

**核心结论**：一句话结论：服务是否健康不能只看“进程还在”；必须能从指标、日志和追踪判断请求是否成功、慢在哪里、失败由谁造成。

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

# 自测

只有 P50 正常、P99 变差时，为什么用户仍会明显感到服务变慢？

# 从零建立模型

本页主题是 **05-Observability and Operations (可观测性与运行)**。Go 的入门主线是“值怎样流动、错误怎样返回、goroutine 怎样结束”。先用普通函数写清业务规则；再把 HTTP、数据库和并发放在边界层。每新建一个 goroutine，都要回答谁取消它、谁等待它、它失败后谁知道。

# 最小实践

写一个十到三十行的最小程序或测试：覆盖正常输入、边界输入和取消/错误路径之一。运行 `go test`；涉及并发时再运行 `go test -race`，把工具输出作为结论证据。

# 工程检查点

channel、context 与 goroutine 都不是性能装饰。没有 deadline、背压和退出协议的并发，会把一次下游慢请求放大成资源泄漏。

# 渐进练习

1. **第一步 · 理解**：读：标出本页代码中错误向上返回、资源释放和 goroutine 退出的位置。
2. **第二步 · 实现**：写：为一个纯业务函数补 table-driven test；若有并发，写一个取消或关闭案例。
3. **第三步 · 验证**：测：运行 `go test`，并在适用时运行 `go test -race` 或 benchmark，记录结论与环境。

# 常见误区

- 把语法或并发原语当万能解法，忽略取消、资源归属、背压和下游失败。
- 不以测试、race detector、profile 或一次可复现请求来验证服务行为。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **05-Observability and Operations (可观测性与运行)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Web Services Map (Web 服务导航)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/00-Web%20Services%20Map%20(Web%20服务导航).md)
- 下一步：[04-Testing and Quality (测试与质量)](/04-Go%20Backend%20(Go%20后端)/03-Web%20Services%20(Web%20服务)/04-Testing%20and%20Quality%20(测试与质量).md)
