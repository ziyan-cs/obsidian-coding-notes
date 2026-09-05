---
tags: [comparison, observability, performance]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Observability & Performance — 先定义问题，再测量

> **结论**：优化必须以可观察指标为起点；吞吐、平均延迟、p99、CPU、内存和错误率不能相互替代。

## 最小观测面

- **logs**：带 request ID、错误上下文，不泄漏敏感信息。
- **metrics**：请求数、错误率、latency 分位数、队列长度、资源使用。
- **traces**：跨服务定位一次慢请求的路径。

## 工具分工

| 目标 | C++ | Go | Python |
| --- | --- | --- | --- |
| 性能定位 | perf、Sanitizer、heap profiler | pprof、trace、race detector | profiler、脚本化统计 |
| 结果处理 | 原始数据/服务侧 | 服务侧指标 | CSV/日志分析、报告生成 |

**测量模板**：负载是什么？环境是什么？基线是什么？指标变化多少？瓶颈证据是什么？改动带来什么副作用？
