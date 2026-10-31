---
study_stage: backlog
review_due:
tags: [ai/observability, ai/security, ai/cost]
---

> [!abstract] 本篇要点
> AI 系统必须同时观察传统服务指标、模型质量、数据链路、工具行为和费用；只监控 HTTP 200 会漏掉最重要的失败。

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

**参考资料：** [OWASP Top 10 for LLM Applications 2026](https://genai.owasp.org/resource/owasp-genai-llm-top-10-2026/) · [OWASP Top 10 for Agentic Applications 2026](https://genai.owasp.org/resource/owasp-top-10-for-agentic-applications-for-2026/) · [NIST AI Risk Management Framework](https://www.nist.gov/itl/ai-risk-management-framework)

> [!info]- 关联笔记
> 通用遥测设计和安全检测证据见 [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/05-Observability%20Logs%20Metrics%20and%20Tracing%20%28%E5%8F%AF%E8%A7%82%E6%B5%8B%E6%80%A7%E3%80%81%E6%97%A5%E5%BF%97%E3%80%81%E6%8C%87%E6%A0%87%E4%B8%8E%E8%BF%BD%E8%B8%AA%29.md) 与 [Security Detection and Incident Evidence (安全检测与事件证据)](../../02-Engineering%20Fundamentals%20%28%E5%B7%A5%E7%A8%8B%E5%9F%BA%E7%A1%80%29/03-Verification%20and%20Diagnostics%20%28%E9%AA%8C%E8%AF%81%E4%B8%8E%E8%AF%8A%E6%96%AD%29/06-Security%20Detection%20and%20Incident%20Evidence%20%28%E5%AE%89%E5%85%A8%E6%A3%80%E6%B5%8B%E4%B8%8E%E4%BA%8B%E4%BB%B6%E8%AF%81%E6%8D%AE%29.md)；本篇聚焦 token、模型版本、质量漂移、成本和提示注入等 AI 特有信号。

# 考察题目

## 1. 动手题

给一次模型请求串联检索、工具和模型调用 trace，统计 token/费用、延迟和答案质量；注入工具错误。

> [!note]- 验收要点
> 能区分质量、成本与故障来源；日志与 trace 不记录原始秘密。

## 2. 机制追问

AI 请求成功返回，为何仍可能是产品失败？

> [!note]- 参考答案
> 还要衡量答案质量、依据、安全事件、延迟和费用；用统一请求/工具 trace 关联模型与业务结果，避免记录敏感提示和凭据。
