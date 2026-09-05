---
tags: [english/engineering, learning/glossary]
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Engineering English Glossary — 高频技术英语

> 不背脱离语境的单词；每次在代码、报错、文档中遇到术语时，补一个自己能用的句子。

| Term | 常用中文 | 一句话理解 | 例句 / 搭配 |
| --- | --- | --- | --- |
| ownership | 所有权 | 谁负责对象或资源最终释放 | `unique ownership` |
| lifetime | 生命周期 | 一个对象从有效到失效的时间范围 | `object lifetime` |
| concurrency | 并发 | 多个任务交错推进，不等于一定并行 | `concurrency control` |
| parallelism | 并行 | 多个任务同时占用多个计算资源 | `data parallelism` |
| race condition | 竞态条件 | 结果取决于不可控的执行先后顺序 | `data race` |
| deadlock | 死锁 | 多方相互等待，系统无法继续 | `avoid deadlock` |
| throughput | 吞吐量 | 单位时间完成的工作量 | `increase throughput` |
| latency | 延迟 | 单个请求从开始到结束的时间 | `p99 latency` |
| idempotent | 幂等的 | 重复执行与执行一次效果相同 | `idempotent request` |
| rollback | 回滚 | 撤销未完成或失败操作的影响 | `transaction rollback` |
| middleware | 中间件 | 请求处理链中通用的横切逻辑 | `HTTP middleware` |
| observability | 可观测性 | 从外部信号理解系统内部状态的能力 | `logs, metrics, traces` |
| regression | 回归问题 | 修改后重新出现的旧问题或新破坏 | `regression test` |
| trade-off | 权衡 | 得到一项收益同时付出代价 | `performance trade-off` |

## 维护规则

- 新词优先记录在对应主题笔记，只有跨主题高频词才加入这里。
- 每个术语至少写一个来自代码、日志或文档的真实搭配。
- 每周复习 5 个，不追求一次背完。
