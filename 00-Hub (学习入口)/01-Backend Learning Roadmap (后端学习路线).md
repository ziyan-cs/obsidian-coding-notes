---
study_stage: reference
tags: [career/roadmap, career/backend]
---

> [!abstract] 学习定位
> 本页只描述能力依赖、升级条件和最终产物，不安排具体日期、周次或每日任务。实际执行统一进入 Study Cycle。

> [!summary] 核心摘要
>
> C++ 建立资源与性能直觉，Python承担自动化和 AI 数据工具，Go形成服务交付能力；系统、数据、云原生、安全和 AI 最终在项目中汇合。是否升级由证据决定，不由日历决定。

# 能力主线

```text
CS / algorithms / engineering basics
        |
        +-> C++ resource model -> Linux / network / concurrency
        |
        +-> Python automation and engineering assistance
        |
        +-> Go service delivery
        v
data systems -> distributed systems -> cloud reliability / security
        v
backend projects -> AI systems extension -> interview evidence
```

Python 在 Go 之前进入主线，用于建立脚本、测试、数据处理和工具能力；Go 在已经理解服务、网络、数据库和失败语义后学习，重点是交付与维护服务，而不是重新背一遍语法。

# 阶段与升级条件

| 阶段 | 核心能力 | 升级证据 |
| --- | --- | --- |
| 基础与 C++ 入门 | 数据结构、对象、内存、Git、构建、测试 | 一个可复现构建并通过测试的 C++ 小项目 |
| Python 工程 | CLI、文件、HTTP、pytest、日志与自动化 | 可安装、可测试、失败安全的工具 |
| C++ 系统主线 | 并发、Linux、网络、调试与性能 | 网络或并发组件及其 sanitizer/profile 证据 |
| Go 服务 | context、HTTP、测试、数据边界和优雅关闭 | 可观测、可停止、有故障测试的 API |
| 数据与分布式 | MySQL、Redis、消息、一致性与恢复 | 含事务、缓存、异步任务和故障演练的服务 |
| 云原生、安全与可靠性 | 容器、Kubernetes、SLO、IAM、事件响应 | 可回滚部署、安全验证和恢复记录 |
| AI 系统工程 | RAG、Agent、评估、推理容量与隔离 | 有数据集、权限、成本和质量证据的 AI 后端 |
| 项目与求职 | 系统设计、工程证据和表达 | 两个可演示主项目及可复现材料 |

# 统一掌握标准

一个主题达到以下条件才算进入可用状态：

1. 不看正文能讲清问题、机制和边界。
2. 能写最小例子、实验或故障复现。
3. 能解释失败发生在哪一层，以及如何取得证据。
4. 能在项目中说明为什么选它，以及什么情况下不选。

# 防止主线漂移

- AI 不替代算法、系统和后端基础。
- Kubernetes 不替代 Linux、网络与服务生命周期。
- 安全不是孤立专题，要进入身份、数据、交付和运行。
- 新框架先说明它解决的问题、引入的成本和退出方案。
- 连续多个学习周期没有可运行产物时，停止扩展主题，回到当前最小交付。

# 执行入口

- 唯一任务面板：[Study Cycle (学习周期)](/00-Hub%20(学习入口)/09-Study%20Cycle%20(学习周期).md)
- 到期复习：[Spaced Review System (间隔复习系统)](/00-Hub%20(学习入口)/02-Spaced%20Review%20System%20(间隔复习系统).md)
- 全库入口：[Vault Map (知识库地图)](/00-Hub%20(学习入口)/00-Vault%20Map%20(知识库地图).md)
