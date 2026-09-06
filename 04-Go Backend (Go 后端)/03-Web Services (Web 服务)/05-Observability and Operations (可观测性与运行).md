---
status: stable
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 05-Observability and Operations (可观测性与运行)

> [!abstract] 一句话结论：服务是否健康不能只看“进程还在”；必须能从指标、日志和追踪判断请求是否成功、慢在哪里、失败由谁造成。

## 最小信号

- Metrics：请求量、错误率、延迟分位数、饱和度、goroutine 与连接池使用量。
- Logs：结构化字段、request ID、错误链；避免记录密钥和完整隐私数据。
- Traces：跨 HTTP、数据库、消息调用关联一次请求。
- Operations：readiness/liveness、优雅关闭、配置校验、容量与回滚预案。

## 自测

只有 P50 正常、P99 变差时，为什么用户仍会明显感到服务变慢？
