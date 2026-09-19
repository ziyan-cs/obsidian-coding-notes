# Obsidian Coding Notes

> **定位**：面向零基础系统学习的中文后端工程笔记库。C++ 打牢资源、系统与性能基础；Python 用于自动化、测试和 AI 数据工具；Go 用于服务端与云原生工程。系统、数据、安全与可靠性构成生产底座，项目用于验证知识是否真正可用。

## 从这里开始

- 总入口：[00-Vault Map (知识库地图)](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md)
- 学习路线：[01-Backend Learning Roadmap (后端学习路线)](/00-Hub%20(学习入口)/01-Backend%20Learning%20Roadmap%20(后端学习路线).md)
- 学习周期：[09-Study Cycle (学习周期)](/00-Hub%20(学习入口)/09-Study%20Cycle%20(学习周期).md)
- 验证与诊断：[01-Testing Strategy and Evidence (测试策略与证据)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/01-Testing%20Strategy%20and%20Evidence%20(测试策略与证据).md)，同目录统一收录调试、内存、性能、可观测性、安全检测与发布门禁。
- 到期复习：运行 `./List-ReviewDue.ps1`；完整队列使用 `./List-ReviewDue.ps1 -All`。

## 阅读约定

1. 每个一级模块只有一篇 `00-... Standard`，用于开始前定目标、学完后验收。
2. 文件名承担文章标题，正文一级标题只表达知识结构。
3. 核心摘要直接展示主题、机制与边界；综合自测集中在模块 `00` 标准页，正文只保留必要实验。
4. 内部链接使用根路径 Markdown 格式，保证 Obsidian 与 GitHub 都能访问。
5. `review_due` 是按实际表现调整的复习建议，不是固定死线。

## 知识模块

| 顺序  | 模块        | 完成标准                                                                                                                                                            |
| --- | --------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 01  | 基础能力      | [基础能力标准](/01-Foundations%20(基础能力)/00-Foundations%20Standard%20(基础能力标准与自测).md)                                                                                   |
| 02  | 工程基础      | [工程基础标准](/02-Engineering%20Fundamentals%20(工程基础)/00-Engineering%20Fundamentals%20Standard%20(工程基础标准与自测).md)                                                     |
| 03  | C++ 后端    | [C++ 后端](/03-C++%20Backend%20(C++%20后端)/00-C++%20Backend%20Standard%20(C++%20后端标准与自测).md)                                                                       |
| 04  | Python 工程 | [Python 工程标准](/04-Python%20Engineering%20(Python%20工程)/00-Python%20Engineering%20Standard%20(Python%20工程标准与自测).md)                                              |
| 05  | Go 后端     | [Go 后端标准](/05-Go%20Backend%20(Go%20后端)/00-Go%20Backend%20Standard%20(Go%20后端标准与自测).md)                                                                          |
| 06  | 系统与网络     | [系统与网络标准](/06-Systems%20and%20Networking%20(系统与网络)/00-Systems%20and%20Networking%20Standard%20(系统与网络标准与自测).md)                                                  |
| 07  | 数据系统与分布式  | [数据系统与分布式标准](/07-Data%20Systems%20and%20Distributed%20Computing%20(数据系统与分布式)/00-Data%20Systems%20and%20Distributed%20Computing%20Standard%20(数据系统与分布式标准与自测).md) |
| 08  | 云原生与可靠性   | [云原生与可靠性标准](/08-Cloud%20Native%20and%20Reliability%20(云原生与可靠性)/00-Cloud%20Native%20and%20Reliability%20Standard%20(云原生与可靠性标准与自测).md)                            |
| 09  | 安全工程      | [安全工程标准](/09-Security%20Engineering%20(安全工程)/00-Security%20Engineering%20Standard%20(安全工程标准与自测).md)                                                             |
| 10  | AI 系统工程   | [AI 系统工程标准](/10-AI%20Systems%20Engineering%20(AI%20系统工程)/00-AI%20Systems%20Engineering%20Standard%20(AI%20系统工程标准与自测).md)                                        |
| 11  | 项目实践      | [项目标准](/11-Projects%20(项目实践)/00-Project%20Standard%20(项目标准与自测).md)                                                                                              |
| 12  | 求职与成长     | [求职能力标准](/12-Career%20(求职与成长)/00-Career%20Standard%20(求职能力标准与自测).md)                                                                                            |

目录编号表达知识依赖，不要求学完一整块才进入下一块。项目从基础阶段就应并行推进；AI、云原生与安全都不能替代语言、系统、网络和数据基础。

## 笔记属性

~~~yaml
---
status: learning
confidence: medium
verified: 2026-09-18
review_due: 2026-09-25
tags: [topic/example]
---
~~~

`review_due` 与 `tags` 按需存在。涉及版本或 API 的结论以官方文档为准；性能结论必须保留环境、负载和测量方法。

## 内容质量门槛

- 教学笔记应说明问题、机制、边界、失败路径与可验证实践，不能只有术语定义或几行示例。
- 稳定原理与易变实现分开书写；易变内容标明版本、环境和验证日期。
- 示例代码必须服务于一个可观察结论，给出输入、预期、错误情况或测量方式。
- 关联链接只保留真正的前置、后续或对照关系，不为了“网状化”堆砌链接。

## 维护边界

- `.obsidian/`、`.git/`、`.trash/` 与 `assets/` 不属于正文整理范围。
- 文件统一使用 `序号-English (中文).md`，目录最多三级。
- 二级知识目录原则上至少包含 3 篇实质笔记，不创建空壳导航页。
- 拆分和合并以知识因果链、工程任务与检索价值为准，不追求机械行数一致。