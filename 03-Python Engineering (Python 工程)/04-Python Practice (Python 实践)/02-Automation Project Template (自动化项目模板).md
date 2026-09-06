---
status: learning
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 02-Automation Project Template (自动化项目模板)

> [!abstract] 一句话结论：Python 的第一批项目应该解决真实重复劳动，并留下输入、输出、日志、测试和失败处理的工程证据。

## 推荐题目

- API health checker：并发检查接口状态与延迟，输出 JSON/Markdown 报告。
- Log summarizer：解析服务日志，统计错误类型与时间窗口。
- Data migration verifier：比对源端和目标端的行数、主键覆盖与校验和。

## 每个项目必须有

1. 可配置的输入与输出目录。
2. dry-run 模式，先展示将执行什么。
3. 可重试的瞬态失败与明确的失败报告。
4. 最少三条测试：正常、空输入、异常输入。
