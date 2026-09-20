---
study_stage: reference
tags: [vault/navigation, backend-learning]
---

> [!abstract] 使用方式
> 本页只负责导航与职责分流，不保存日程、完成率或临时任务。学习安排统一进入 Study Cycle，复习到期项由脚本读取各笔记的 `review_due`。

# 四个核心入口

| 需要解决的问题     | 唯一入口                                                                                                               | 不在这里做什么     |
| ----------- | ------------------------------------------------------------------------------------------------------------------ | ----------- |
| 今天和本周做什么    | [Study Cycle (学习周期)](/00-Hub%20(学习入口)/09-Study%20Cycle%20(学习周期).md)                                                | 不把任务散落进知识模块 |
| 长期先学什么、何时升级 | [Backend Learning Roadmap (后端学习路线)](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md)                | 不绑定具体日期     |
| 哪些笔记需要复习    | `List-ReviewDue.ps1` 与 [Spaced Review System (间隔复习系统)](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md) | 不预先为未学习内容排期 |
| 一个模块学到什么程度  | 各模块 `00-... Standard`                                                                                              | 不记录每日进度     |

# 知识库维护

- [Note Standard (笔记规范)](/00-Hub%20(学习入口)/03-Note%20Standard%20(笔记规范).md)：正文结构和写作约束。
- [Engineering English Glossary (工程英语术语)](/00-Hub%20(学习入口)/04-Engineering%20English%20Glossary%20(工程英语术语).md)：跨模块术语。
- [Mistake Log (错误记录)](/00-Hub%20(学习入口)/05-Mistake%20Log%20(错误记录).md)：跨主题错误模式。
- [Sources and Freshness Policy (来源与时效规则)](/00-Hub%20(学习入口)/06-Sources%20&%20Freshness%20Policy%20(来源与时效规则).md)：事实证据与更新策略。
- [Vault Audit and Migration Queue (审计与迁移队列)](/00-Hub%20(学习入口)/07-Vault%20Audit%20&%20Migration%20Queue%20(审计与迁移队列).md)：只记录知识库维护事项。
- [Vault Design System (知识库设计系统)](/00-Hub%20(学习入口)/08-Vault%20Design%20System%20(知识库设计系统).md)：视觉和信息架构。

# 模块能力标准

1. [基础能力](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md)
2. [工程基础](/02-Engineering%20Fundamentals%20(工程基础)/00-Engineering%20Fundamentals%20Standard%20(工程基础标准与自测).md)
3. [C++ 后端](/03-C++%20Backend%20(C++%20后端)/00-C++%20Backend%20Standard%20(C++%20后端标准与自测).md)
4. [Python 工程](/04-Python%20Engineering%20(Python%20工程)/00-Python%20Engineering%20Standard%20(Python%20工程标准与自测).md)
5. [Go 后端](/05-Go%20Backend%20(Go%20后端)/00-Go%20Backend%20Standard%20(Go%20后端标准与自测).md)
6. [系统与网络](/06-Systems%20and%20Networking%20(系统与网络)/00-Systems%20and%20Networking%20Standard%20(系统与网络标准与自测).md)
7. [数据系统与分布式](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/00-Data%20Systems%20and%20Distributed%20Computing%20Standard%20(数据系统与分布式标准与自测).md)
8. [云原生与可靠性](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/00-Cloud%20Native%20and%20Reliability%20Standard%20(云原生与可靠性标准与自测).md)
9. [安全工程](/09-Security%20Engineering%20(安全工程)/00-Security%20Engineering%20Standard%20(安全工程标准与自测).md)
10. [AI 系统工程](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/00-AI%20Systems%20Engineering%20Standard%20(AI%20系统工程标准与自测).md)
11. [项目实践](/11-Projects%20(项目实践)/00-Project%20Standard%20(项目标准与自测).md)
12. [求职与成长](/12-Career%20(求职与成长)/00-Career%20Standard%20(求职能力标准与自测).md)

# 默认使用流程

1. 从 Study Cycle 只选择一个当前主线和一个可验证产物。
2. 学习前查看模块标准，学习后完成最小实验或代码。
3. 需要长期记忆时才写 `review_due`，由脚本统一汇总。
4. 错误进入 Mistake Log，知识库结构问题进入 Audit Queue；两者都不混入学习日程。
