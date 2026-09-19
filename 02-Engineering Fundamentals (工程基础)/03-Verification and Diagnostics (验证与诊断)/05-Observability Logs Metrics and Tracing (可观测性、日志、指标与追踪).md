---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 可观测性（Observability）不是“接入监控平台”，而是让未知故障能通过系统外部信号被解释。本篇统一日志、指标、追踪、告警和 SLO 的设计方法；语言 SDK 与平台配置留在各专题。

# 从问题开始设计信号

系统至少要能回答四类问题：

1. 用户是否成功完成请求？
2. 哪个组件、版本或依赖正在失败？
3. 失败影响了谁、多久、多少流量？
4. 资源饱和、代码路径还是外部依赖导致了问题？

日志、指标和追踪不是三套独立作业：指标发现异常，trace 缩小请求路径，日志补充离散事件和业务上下文，profile 解释持续的资源消耗。

## 统一上下文模型

跨信号关联应使用一致的服务与请求上下文，但**不能把所有关联字段直接放进指标标签**。区分适合聚合的低基数维度与仅适合逐请求事件的高基数标识：

```text
指标维度示例：service.name   environment   region/zone
              operation(路由模板)   error.type   tenant_tier(有限枚举)
日志/trace 关联：trace_id   span_id   request_id   build_id
              instance_id(按后端及查询方式决定是否聚合)
```

请求 ID、trace ID、用户 ID、URL 原文和异常全文等高基数值不能作为普通指标标签；前两者适合关联日志与 trace，用户标识与异常详情仍需遵守数据最小化、脱敏和访问控制。即使 `operation` 是指标标签，也应使用路由模板而非带动态 ID 的实际 URL。

# 结构化日志

日志是一条有时间、有上下文的事件记录。推荐结构化 JSON，并将“发生了什么”与“为什么”分开：

```json
{
  "timestamp": "2026-09-18T10:20:30.123Z",
  "level": "ERROR",
  "service": "order-api",
  "operation": "CreateOrder",
  "trace_id": "...",
  "error_type": "dependency_timeout",
  "dependency": "inventory",
  "duration_ms": 803,
  "retryable": true
}
```

日志级别应表达操作意图：`DEBUG` 用于临时诊断，`INFO` 记录有价值的生命周期事件，`WARN` 表示已降级但仍可服务，`ERROR` 表示当前操作失败。不要在每层重复打印同一异常；在拥有处理决策和完整上下文的边界记录一次。

敏感信息不得直接写入日志：密码、令牌、Cookie、密钥、完整身份证号和支付数据需要禁止或脱敏。日志还应有保留周期、访问审计和删除策略。

# 指标、追踪与服务目标

常用指标类型：Counter 只增不减；Gauge 表示瞬时状态；Histogram 记录分布并支持分位数估算。服务侧优先覆盖：

- Traffic：请求、消息或任务吞吐；
- Errors：失败率，按可行动的错误类别拆分；
- Latency：成功与失败请求分开统计分布；
- Saturation：CPU、内存、连接池、队列、线程/协程和磁盘容量。

百分位延迟不能由多个实例的局部 percentile 直接求平均；应汇总 histogram bucket 或使用支持可合并分布的数据结构。指标命名要包含单位，例如 `_seconds`、`_bytes_total`。

## 分布式追踪

一个 trace 描述一次端到端操作，span 描述其中一个阶段。span 至少包括名称、开始与结束时间、状态、父子关系和必要属性。

```text
client
  +-- HTTP server span
        +-- auth span
        +-- SQL span
        +-- RPC inventory span
```

入口生成或验证 trace context，跨 HTTP、RPC 和消息边界传播。异步消费要明确 producer/consumer 关系；重试应创建新 span，并用属性记录 attempt，避免把一次逻辑操作误画成无穷调用链。

采样是成本与诊断能力的权衡。Head sampling 在请求开始时决定，便宜但可能丢失罕见错误；Tail sampling 根据完整结果决定，更有信息但需要集中缓冲。错误和高延迟请求可以提高保留概率，但不能用采样后的数量直接推导未经校正的业务总量。

## SLI、SLO 与错误预算

SLI（Service Level Indicator）是测量值，SLO（Objective）是目标。可用性应从用户可感知结果定义：

```text
availability = good_events / valid_events
```

“good”需写清状态码、业务结果和延迟阈值；健康检查成功不等于用户请求成功。错误预算把可靠性目标转成可消费额度：预算消耗过快时暂停高风险发布，优先修复可靠性问题。

# 告警与信号可靠性

好告警意味着需要人采取动作。告警说明应包含：影响、当前值与阈值、持续时间、可能相关变更、dashboard/trace/runbook 链接。避免对每个底层指标单独报警，优先使用面向用户症状的多窗口 burn-rate 或持续失败条件。

```text
告警 -> 确认用户影响 -> 检查近期变更
     -> 按服务/版本/区域切分指标
     -> 用 trace 找慢段或失败边
     -> 用日志与 profile 验证假设
```

Dashboard 用于理解状态，不应变成所有指标的陈列柜。首页保留流量、成功率、延迟、饱和度和发布标记，再从异常维度下钻。

## 可观测性本身也会失败

- collector 阻塞不能拖垮业务请求；使用有界队列、批量、超时与丢弃策略。
- 本地时间漂移会破坏跨服务排序；统一时钟同步并记录单调时钟持续时间。
- 标签失控会造成存储和查询成本爆炸；发布前审核 cardinality。
- telemetry 后端不可用时，业务应降级而非同步等待。
- 对日志、指标和 trace 做容量预算，监控丢弃率、导出失败和采样配置。

# 实践与资料

- [ ] 为一个 API 定义 success、latency、traffic、saturation 四组指标。
- [ ] 让入口日志、错误日志和 trace 通过 `trace_id` 互相定位。
- [ ] 模拟依赖超时，确认告警、dashboard、trace 和日志能形成同一证据链。
- [ ] 删除一个高基数指标标签，并比较时间序列数量变化。
- [ ] 写一条包含触发条件、影响判断、止损和验证步骤的 runbook。

## 关联专题

- [Performance Benchmarking and Profiling (性能基准与剖析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md)：从运行信号进入资源热点分析。
- [Incident Response Chaos and Recovery (事故响应、混沌与恢复)](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/03-Reliability%20Engineering%20(可靠性工程)/03-Incident%20Response%20Chaos%20and%20Recovery%20(事故响应、混沌与恢复).md)：用信号驱动事故处置与恢复验证。

## 参考资料

- [OpenTelemetry Observability Primer](https://opentelemetry.io/docs/concepts/observability-primer/)
- [Google SRE Books](https://sre.google/books/)
- [Kubernetes Observability](https://kubernetes.io/docs/concepts/cluster-administration/observability/)
