---
study_stage: backlog
tags: [ai/observability, ai/security, ai/cost]
---

> [!note] 方法论坐标
> 通用遥测设计和安全检测证据见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md) 与 [Security Detection and Incident Evidence (安全检测与事件证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/06-Security%20Detection%20and%20Incident%20Evidence%20(安全检测与事件证据).md)；本篇聚焦 token、模型版本、质量漂移、成本和提示注入等 AI 特有信号。

> [!abstract] 学习定位
> AI 系统必须同时观察传统服务指标、模型质量、数据链路、工具行为和费用；只监控 HTTP 200 会漏掉最重要的失败。

> [!summary] 核心摘要
>
> 一次 trace 串起检索、模型、工具和副作用，记录版本、延迟、token、费用和评估结果。敏感内容按最小化原则采集，安全控制独立于模型判断。

# 统一 Trace

~~~text
request
  -> retrieval and rerank
  -> model call
  -> tool call
  -> business side effect
  -> response and evaluation
~~~

span 记录稳定 ID、版本、耗时、错误分类和用量，不默认保存完整 prompt。在线质量无法只靠系统指标表达，需要抽样评估、用户反馈和业务结果。

# 质量与漂移

监控无引用回答、拒答率、工具失败、人工接管、检索空结果和安全拦截。模型、prompt、索引和工具任一变化都可能导致回归；发布时使用固定评估集和灰度对比。

反馈数据可能有偏差和隐私风险。进入训练或评估前需要授权、脱敏、质量筛选和版本记录。

# 成本归因

费用按租户、功能、模型、token、检索和工具调用归因。重试、过长上下文、无效检索和 Agent 循环会隐藏成本。预算控制可限制请求、切换模型或要求审批，但必须向调用方明确降级。

用“每成功任务成本”比“每次模型调用成本”更接近业务：

~~~text
unit cost = (model tokens + retrieval + tools + retries + allocated infrastructure) / successful tasks
~~~

若廉价模型需要多次重试和人工接管，总成本可能更高。分母必须定义清楚：被拒答但符合策略的任务是否算成功，应按业务目标决定；报告成本时同时报告任务成功率和安全护栏。

# 版本维度与质量告警

每条 trace 记录模型、prompt/template、索引、策略与工具 schema 版本，才能比较发布前后行为。高基数 request/user 不进入指标标签，保留在受控日志或 trace；原始内容默认不采集。

质量告警需要基线和人工校准：检索空结果、引用缺失、拒答、工具失败、越权阻断和人工接管突变都可能表示回归。离线评估负责已知样本，canary/shadow 负责真实分布，但 shadow 数据仍要遵守授权与保留要求。

成本按成功任务归因，并拆出重试、无效上下文、循环步骤和缓存未命中。预算耗尽时定义拒绝、降级模型或转异步，不能静默降低安全检查。遥测后端失败不得阻塞业务，同时监控导出失败和丢弃量。

# 安全边界

防止 prompt injection、敏感信息泄漏、不安全输出、供应链风险和过度授权。模型不可直接决定身份与权限；工具执行器、数据层和网络层实施确定性控制。

日志与 trace 本身可能包含隐私和商业秘密，需要访问控制、保留期限和删除流程。

> [!question]- 理解检查
> 一个 AI 接口成功率 99.99%，为什么仍可能完全不可用？列出质量、权限和成本层的失败例子。

> [!info]- 官方参考
> - [OWASP Top 10 for LLM Applications](https://genai.owasp.org/llm-top-10/)
> - [NIST AI Risk Management Framework](https://www.nist.gov/itl/ai-risk-management-framework)
