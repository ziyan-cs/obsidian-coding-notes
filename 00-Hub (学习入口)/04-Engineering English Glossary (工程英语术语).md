---
study_stage: reference
tags: [english/engineering, learning/glossary]
---

> [!abstract] 用法
> 先保留英文术语，再结合上下文理解中文义。这里只收跨专题高频词；新词优先记在对应知识笔记。

# 资源、并发与性能

| Term | 常用义 | 阅读时注意 |
| --- | --- | --- |
| lifetime | 生命周期 | 不等同于词法作用域 |
| ownership | 所有权 | 谁负责资源释放，不只是访问权限 |
| allocation | 分配 | 不预设一定发生在堆上 |
| blocking / non-blocking | 阻塞 / 非阻塞 | 非阻塞不等于异步，也不等于一定更快 |
| concurrency / parallelism | 并发 / 并行 | 重叠推进不等于同时执行 |
| contention | 竞争 | 多个执行者争用共享资源 |
| backpressure | 背压 | 下游容量限制向上游传播 |
| throughput / tail latency | 吞吐量 / 尾延迟 | 吞吐高不代表尾延迟低 |

# 网络与数据

| Term | 常用义 | 阅读时注意 |
| --- | --- | --- |
| payload / framing | 载荷 / 分帧 | 消息内容与字节流边界是两回事 |
| handshake | 握手 | 建立身份、能力或参数共识 |
| timeout / deadline | 超时 / 截止时刻 | 相对时长与绝对时刻不同 |
| retry / idempotency | 重试 / 幂等 | 重试需考虑副作用、上限与退避 |
| consistency | 一致性 | 说明对象、观察者和时间范围 |
| durability | 持久性 | 取决于承诺的故障模型 |
| replication / sharding | 复制 / 分片 | 复制保留副本；分片按规则拆分数据 |
| network partition | 网络分区 | 节点间无法正常通信，不是数据分片 |
| authentication / authorization | 认证 / 授权 | 你是谁，与允许你做什么 |

# 交付与运行

| Term | 常用义 | 阅读时注意 |
| --- | --- | --- |
| artifact / dependency | 构建产物 / 依赖 | 关注版本、来源和可复现性 |
| regression | 回归、退化 | 明确功能或性能指标及基线 |
| observability | 可观测性 | 从日志、指标、追踪推断内部状态 |
| rollout / rollback | 渐进发布 / 回滚 | 关注健康判据与恢复边界 |
| incident / mitigation | 事故 / 缓解 | 先控制影响，再确认根因并修复 |
| deprecation | 弃用 | 通常仍可用；移除时间以维护者公告为准 |
| inference / evaluation | 推理 / 评估 | 生成结果，与按标准检查结果质量 |

遇到生词先回到原句或代码理解，再用自己的话解释。只把反复遇到、容易混淆的词补进本页，不单独建立背词任务。
