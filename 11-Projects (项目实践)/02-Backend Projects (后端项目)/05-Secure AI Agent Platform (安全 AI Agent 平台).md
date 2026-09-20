---
study_stage: backlog
tags: [project/ai, project/security, project/cloud-native]
---

> [!abstract] 项目定位
> 构建一个面向内部知识库的 Agent 平台：回答必须带引用，工具执行受权限和审批控制，系统能够评估、观测、限流并部署到 Kubernetes。

# 问题与非目标

用户上传或同步文档后，可以按权限提问；系统检索证据、生成带引用回答，并在授权后调用只读查询或工单工具。第一版不训练基础模型、不追求通用自主 Agent，也不执行任意用户代码。

为避免把多种高难度系统一次做完，最小版本只用一组经授权的 Markdown 文档、一个租户、一个只读查询工具和一个模型接口。先让“导入 → 检索 → 引用回答 → 无答案拒答”在本地可运行；再增加权限、工具和部署能力。模型服务可以是受控 API 或本地实例，但须固定版本和调用预算。

成功标准：

- 越权文档不会进入检索上下文。
- 无证据问题明确拒答。
- 工具参数、权限、幂等和 timeout 可验证。
- 一次请求能从 API 追踪到检索、模型与工具。
- 能测量质量、P95/P99、token、费用和失败分类。

# 建议架构

~~~text
client -> Go API and auth -> task or synchronous workflow
             |                    |
             v                    v
       metadata and ACL      Python evaluation/data pipeline
             |
       retrieval -> model gateway -> policy -> tool executor
             |                       |
          vector index          sandbox or isolated service
~~~

Go 负责在线 API、身份、状态、队列和工具执行；Python 负责数据处理、离线评估与回归集。C++ 只在真实 profile 证明解析、向量计算或推理组件成为瓶颈后引入。

这个分工是项目建议而非语言定律。Go API 不直接持有宽权限数据库凭据去执行模型任意提议；工具执行器是独立服务边界。Python 离线流水线产出带 checksum 的索引候选版本，在线服务在验收后切换别名。第一版可以用进程内抽象模拟队列与索引，不能把“计划使用 Kubernetes”写成已实现成果。

# 数据、权限与工作流

文档处理链记录 source、tenant、ACL、版本、解析器版本、chunk 位置和 checksum。新增、修改、删除和权限变化都必须传播到索引；否则旧 chunk 会继续被召回。索引构建到新版本后先离线验证，再原子切换查询别名，失败可回到上一版本。

```text
source -> parse -> normalize -> chunk -> attach ACL/version
       -> embed -> candidate index -> evaluation -> activate alias
```

权限过滤在检索层执行，并在返回上下文前再次核验；不能先召回其他租户数据再提示模型“不要使用”。缓存 key 包含租户、权限范围、模型和索引版本，防止跨用户复用越权结果。

文档 ACL 变化是独立测试路径：用户 A 原本可读文档 X，管理员撤销权限后，A 的下一次查询不能从旧索引、缓存或历史会话里拿到 X 的原文。定义权限变更的生效时限，并让检索过滤依据当前服务端权限，而不是仅信任导入时写入的旧标签；历史回答与审计记录的保留又应遵循单独的数据治理规则。

## 请求工作流与可恢复状态

一次请求包含稳定 task ID 和状态机：`accepted -> retrieving -> generating -> tool_pending -> completed/failed/canceled`。同步请求受总 deadline 限制；长任务进入队列，客户端按 task ID 查询状态。

模型调用可能 timeout、限流或返回无法解析内容。重试只对明确瞬态错误，并把剩余 deadline、token 预算和已产生副作用纳入判断。结构化输出通过 schema 校验；解析失败不能直接驱动工具。

回答保存引用到 chunk ID、源文档版本和位置。文档后来更新时，历史回答仍能说明当时使用了哪个版本。无足够证据时返回拒答，而不是让模型用参数知识补全并伪造引用。

# 工具执行是安全边界

模型只提出结构化 tool proposal，确定性策略层重新验证调用者身份、工具白名单、参数 schema、资源范围和风险等级。高风险动作要求人工审批；执行器使用最小权限的独立身份、网络范围和 timeout。

```text
model proposal
 -> schema validation
 -> authorization / policy
 -> optional approval
 -> idempotency record
 -> isolated executor
 -> sanitized result
 -> audit event
```

工具调用携带 idempotency key。超时后结果不确定时先查询执行状态，不能盲目重试。工具输出也属于不可信输入，返回模型前限制大小、类型和敏感字段。

# 验证、部署与运行

评估集保存问题、允许访问的文档、期望证据、不可回答样本和期望工具行为。指标分层：retrieval recall/precision、引用正确性、回答忠实度、拒答质量、工具参数正确率、越权率、延迟和费用。一个总分无法定位问题来自检索、模型还是策略。

安全用例覆盖提示注入文档、跨租户问题、编码混淆、工具参数越权、敏感输出和重复执行。模型拒绝不是安全边界；真正的通过条件是检索过滤、策略和执行器即使模型被诱导也不能越权。

| 验收输入 | 预期结果 | 必须留下的证据 |
| --- | --- | --- |
| 授权问题且存在证据 | 回答引用正确版本的授权 chunk | 检索候选、引用 ID、人工核对 |
| 文档中没有答案 | 明确拒答或转人工 | 空结果/低支持度原因 |
| 恶意文档要求调用导出工具 | 工具策略拒绝，而非仅靠模型拒绝 | 授权日志与工具未执行记录 |
| 工具执行成功但响应丢失 | 用任务/幂等 ID 找回原结果 | 单一副作用记录与重试轨迹 |
| 文档权限被撤销 | 新查询与缓存都不返回文档 | 撤权时间、索引/缓存版本与测试结果 |

评估集先从十余条人工可核对样本起步，再扩充到足以覆盖语言、权限、过期文档、无答案与工具失败的回归集。报告每类样本的结果，不把小样本的“100% 通过”写成线上安全保证。

## 部署与运行证据

在线 API、worker、索引任务和执行器分成独立 workload，分别设置资源上限、身份和扩缩指标。GPU/模型网关限流按并发、token 与费用预算；队列积压和 deadline 决定是否继续接受任务。

trace 串联认证、检索、重排、模型与工具 span；日志记录 task、模型/索引版本和稳定错误类别；指标覆盖成功率、各阶段延迟、token、费用、拒答、工具审批、越权阻断和队列饱和。原始 prompt、文档和模型输出可能含敏感数据，采样、脱敏和保留期必须单独设计。

验收时演示三条路径：有证据的正常回答；无证据拒答；恶意文档诱导工具越权但被确定性策略阻断。再注入模型 timeout、向量库故障和重复工具请求，展示降级、幂等与审计证据。

# 里程碑

1. 单租户问答：文档导入、检索、回答引用和基础测试。
2. 多租户权限：索引 ACL、缓存隔离和越权测试。
3. 工具调用：只读工具、schema、timeout、审计和幂等。
4. 评估：固定问题集，区分召回、忠实度和拒答质量。
5. 生产化：容器、Kubernetes、SLO、限流、灰度和回滚。
6. 安全化：prompt injection 样本、秘密保护、最小权限与人工审批。

# 故障分析与表达

主动注入模型 timeout、Redis/向量库故障、检索空结果、工具重复调用和恶意文档。保存请求 trace、指标、测试和复盘，说明系统如何降级以及哪些风险仍未解决。

## 面试表达边界

不要宣称“实现了通用 Agent”或“完全防御提示注入”。说明你通过确定性权限、数据过滤、工具隔离和评估降低了哪些具体风险，并用测试与日志证明。

风险边界参考：[OWASP LLM01 Prompt Injection](https://genai.owasp.org/llmrisk/llm01-prompt-injection/) 与 [NIST Generative AI Profile](https://www.nist.gov/publications/artificial-intelligence-risk-management-framework-generative-artificial-intelligence)。这两份资料用于确定风险与验证方向，不替代本项目的实际权限测试。

> [!question]- 验收
> 如果恶意文档要求模型读取其他租户数据并调用工具外传，系统的哪几层会阻止它？
