---
study_stage: backlog
review_due:
tags: [comparison, observability, performance]
---

> [!abstract] 本篇要点
> 结论：优化必须以可观察指标为起点；吞吐、平均延迟、p99、CPU、内存和错误率不能相互替代。
>
> 指标描述总体趋势，日志解释离散事件，追踪连接跨组件路径；三者要围绕同一请求和 SLO 组织。优化必须先用这些证据定位瓶颈，再验证收益与副作用。

# 最小观测面

- **logs**：带 request ID、错误上下文，不泄漏敏感信息。
- **metrics**：请求数、错误率、latency 分位数、队列长度、资源使用。
- **traces**：跨服务定位一次慢请求的路径。

# 工具分工

| 目标 | C++ | Go | Python |
| --- | --- | --- | --- |
| 性能定位 | perf、Sanitizer、heap profiler | pprof、trace、race detector | profiler、脚本化统计 |
| 结果处理 | 原始数据/服务侧 | 服务侧指标 | CSV/日志分析、报告生成 |

**测量模板**：负载是什么？环境是什么？基线是什么？指标变化多少？瓶颈证据是什么？改动带来什么副作用？

# 最小实验记录

```text
目标：降低哪个指标，为什么它重要？
环境：硬件、OS、版本、配置、数据规模。
负载：并发数、请求分布、持续时间、预热策略。
结果：baseline 与 change 的 p50/p95/p99、吞吐、错误率、资源占用。
结论：证据支持什么；哪些条件下不成立；下一步验证什么。
```

> [!warning] 常见误区
> 只报平均延迟、只截一张 profiler 图、或在本机单次运行后宣称“快了 N 倍”，都不是可复现的性能结论。

> [!question]- 决策复盘
> 性能优化先定义用户可感知的目标，再记录环境与负载，以 baseline 对照分析分位延迟、错误率和资源。没有证据的优化只是猜测。
>
> - 自测：吞吐上升但 p99 变差，你会再看哪些队列、GC、锁或依赖指标？
> - 自测：为什么火焰图中的热点不必然就是第一个该优化的点？

> [!info]- 关联笔记
> 技术判断基线见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/05-Observability%20Logs%20Metrics%20and%20Tracing%20%28%E5%8F%AF%E8%A7%82%E6%B5%8B%E6%80%A7%E3%80%81%E6%97%A5%E5%BF%97%E3%80%81%E6%8C%87%E6%A0%87%E4%B8%8E%E8%BF%BD%E8%B8%AA%29.md) 与 [Performance Benchmarking and Profiling (性能基准与剖析)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/04-Performance%20Benchmarking%20and%20Profiling%20%28%E6%80%A7%E8%83%BD%E5%9F%BA%E5%87%86%E4%B8%8E%E5%89%96%E6%9E%90%29.md)；本篇聚焦如何把监控与性能证据表达成工程决策。

# 考察题目

## 1. 动手题

给项目建立请求延迟分位、错误率和依赖耗时面板，做一次前后性能比较。

> [!note]- 验收要点
> 同负载同环境比较，改善 p50 不掩盖 p99 或错误率恶化。

## 2. 机制追问

服务平均延迟下降，为什么仍需检查 p99 和错误率？

> [!note]- 参考答案
> 优化可能让少数请求更慢或失败；同时比较负载、分位延迟、吞吐和资源成本，并把观测指标与用户路径联系起来。
