---
study_stage: backlog
review_due:
tags: [topic/debugging, topic/reliability]
---

> [!abstract] 本篇要点
> 可靠性来自可演练的失败处理和恢复能力；混沌实验应验证假设，而不是在生产中随机破坏。

# 事故响应

角色至少包括指挥、操作、沟通和记录；小团队可兼任，但责任必须清楚。先回答影响范围、开始时间、最近变更和关键依赖，再选择回滚、降级、限流或故障转移。

~~~text
detect -> declare -> coordinate -> mitigate
       -> recover -> verify -> communicate -> review
~~~

操作记录时间、命令、结果和决策依据。频繁无效尝试会破坏证据并扩大故障。

# 恢复设计

备份只有经过恢复演练才有价值。定义 RPO 表示可接受的数据丢失窗口，RTO 表示恢复时间目标。恢复流程要覆盖密钥、配置、依赖顺序和数据一致性，而不仅是复制文件。

跨区或多副本不能自动解决误删除和错误写入，因为错误也会被复制。

# 混沌实验

从具体假设开始，例如“单个实例终止不会影响 SLO”或“Redis 不可用时回源限流能保护数据库”。先在测试环境或小范围执行，设置观察指标、中止条件和负责人。

实验前确认系统处于健康状态，并避免同时进行高风险发布。实验结束后验证资源清理和长期修复。

# 分级、指挥与变更控制

事故分级基于用户影响、数据/安全风险和持续时间，不基于团队情绪。宣布事故后建立唯一沟通频道、指挥者和时间线；非必要发布暂停，所有高风险操作先说明预期、回退方式和观察信号。

止损优先于根因：回滚最近变更、隔离故障租户、关闭非关键功能、限流或切换流量。每次动作后等待足够观测窗口，避免多人同时修改导致无法判断哪一步有效。

# 恢复验证

恢复不是图表回绿：检查关键用户旅程、数据不变量、积压、缓存/副本一致性和安全控制。队列和重试可能在依赖恢复后形成第二波流量，需要控制 drain 速率。

RPO/RTO 必须通过恢复计时验证。恢复演练从新环境获取备份、密钥、配置和依赖，校验数据完整性，再切换流量；只执行备份命令不算验证。

# 混沌实验模板

```text
hypothesis: Redis failure will not overload MySQL
steady state: success rate, P99, DB connections and queue depth
injection: block Redis traffic for one canary instance
abort: error rate or DB saturation exceeds limit
expected: bounded fallback, alert, recovery after removal
evidence: dashboard, trace, logs and timeline
```

从进程终止、网络延迟、依赖 timeout、磁盘满、凭据过期等具体故障开始。爆炸半径从本地/测试/canary 逐步扩大；实验工具也要有权限控制、自动停止和审计。

# 复盘转化为系统改进

时间线区分事实与推断。根因不是“某人执行错误命令”，而要继续问为何权限、评审、默认值、验证和恢复没有阻止或限制影响。行动项优先修复检测、隔离、自动化和恢复能力，并附负责人、期限与验收实验。

官方参考：[Google SRE Incident Response](https://sre.google/sre-book/managing-incidents/)、[Google SRE Workbook: Postmortem Culture](https://sre.google/workbook/postmortem-culture/)。

复盘不等于无人负责，而是不以惩罚个人替代系统改进。记录影响、时间线、触发事件、促成条件、有效和无效响应，并给行动项设负责人、期限和验证标准。

> [!info]- 关联笔记
> 事故证据链与发布停止条件分别见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](<../../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/05-Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪).md>) 和 [Release Verification and Quality Gates (发布验证与质量门禁)](<../../02-Engineering Fundamentals (工程基础)/03-Verification and Diagnostics (验证与诊断)/07-Release Verification and Quality Gates (发布验证与质量门禁).md>)；本篇聚焦云原生环境的混沌、恢复和无责复盘。

# 考察题目

## 1. 动手题

在测试环境断开数据库连接 2 分钟，记录检测、降级、恢复和数据校验全过程。

> [!note]- 验收要点
> 有停止条件与回滚预案，恢复后验证请求和数据正确性，不只记录服务重启。

## 2. 机制追问

故障演练怎样证明恢复能力而不是只制造中断？

> [!note]- 参考答案
> 先定义假设、影响范围、停止条件和回滚步骤，再注入受控故障；记录发现、遏制、恢复时间和数据正确性，演练后补缺口。
