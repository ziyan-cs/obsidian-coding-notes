---
status: learning
confidence: medium
content_verified: 2026-09-18
tags: [ai/evaluation, ai/data, engineering/experimentation]
---

> [!abstract] 学习目标
> 把“感觉模型变好了”变成可复现的评估：定义任务、样本、评分器、基线、切片和发布阈值，同时识别数据泄漏、评分偏差和线上分布漂移。

> [!summary] 核心摘要
>
> AI 系统评估必须覆盖最终任务以及检索、模型、工具等中间环节。数据集要代表真实分布和高风险边界，指标要与业务损失对应；自动评分器需要人工校准，线上反馈也不能直接当作无偏真值。

# 从产品目标到评估任务

先写清输入、允许输出、成功条件和失败代价。例如“客服回答准确”过于含糊，应拆成：答案是否被给定证据支持、是否完整回答问题、引用是否指向正确片段、无权限内容是否拒答、延迟和成本是否在预算内。

```text
business outcome
  -> task definition
  -> representative dataset + risk slices
  -> scorer and human rubric
  -> baseline and release threshold
  -> online monitoring and feedback loop
```

端到端成功率最接近用户价值，但难以定位根因；组件指标用于诊断：

- Retrieval：recall@k、MRR/nDCG、命中证据、权限过滤正确率。
- Generation：事实支持、完整性、格式遵循、拒答与引用正确率。
- Tool use：工具选择、参数、调用顺序、任务完成与副作用。
- System：P50/P95/P99、token、费用、缓存命中、超时与降级。
- Safety：提示注入抵抗、敏感信息泄漏、越权动作和不安全输出。

# 构建可信数据集

样本来源包括真实任务脱敏、专家编写、历史故障、合成边界和对抗样本。随机抽样只能表示平均流量，还需按语言、租户、文档类型、问题难度、长上下文、冷门实体和高风险动作建立 slice。

划分训练、开发和测试集时防止同一文档、模板或近重复问题跨集合泄漏。每条样本保存来源、版本、授权、预期行为、评分规则和敏感级别；生产数据进入评估集前需要用途授权、最小化和保留策略。

测试集不是一次性资产。线上新失败经人工确认后进入回归集，但要避免只收集“被用户投诉的失败”造成选择偏差。模型、prompt、索引、文档和工具 schema 变化都应能追溯到对应评估版本。

# 评分器与不确定性

精确匹配适合 JSON、分类和确定答案；规则评分适合 schema、引用和权限；专家标注适合开放任务。LLM-as-judge 可扩大覆盖，但可能受提示、位置、长度、模型偏好和同源偏差影响。

使用自动评分器时：

1. 定义清晰 rubric 和可观察证据。
2. 用人工双标样本校准一致性。
3. 随机交换候选顺序，避免位置偏差。
4. 保存评分理由但不把理由当成真值。
5. 对接近阈值和高风险失败进行人工复核。

比较两个方案时报告样本量、各 slice 结果和置信区间，不只比较总平均。多次采样可估计非确定性，但生产参数固定后仍需按真实调用方式评估。

# 离线结果到线上发布

离线通过不代表线上安全。发布流程应固定模型与依赖版本，先 replay 或 shadow，再小流量 canary；同时观察任务质量、错误、延迟、成本、安全事件和下游业务影响。出现回归时要能回退模型、prompt、检索索引或工具配置。

线上 A/B 实验要预先定义主指标、护栏指标、分流单位和停止条件。用户点击、采纳或重试受 UI、曝光和人群影响，不等价于答案事实正确。严重安全问题不应等待统计显著性才停止实验。

# 最小评估项目

为一个带引用的 RAG API 建立不少于 50 条的小型黄金集，包含正常问题、无答案、过期文档、跨权限、提示注入和长文档。保存检索结果、最终回答、引用、token、延迟和版本；先以简单关键词或单模型方案作基线，再证明新方案在哪些 slice 改善、在哪些 slice 退化。

# 参考资料

- [NIST AI Risk Management Framework](https://www.nist.gov/itl/ai-risk-management-framework)
- [NIST AI Resource Center](https://airc.nist.gov/)
- [OWASP Top 10 for LLM and GenAI](https://genai.owasp.org/initiative/owasp-top-10-for-llm-and-genai/)

