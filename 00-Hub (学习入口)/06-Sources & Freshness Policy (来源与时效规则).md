---
study_stage: reference
tags: [vault/sources, learning/verification]
---

> [!abstract] 核验边界
> AI 生成内容需要核验。原理应能解释，版本行为应有依据，性能数字应交代条件；不因排版完整就认为结论可靠。

# 依据怎么选

| 要确认的内容 | 优先依据 |
| --- | --- |
| 语言与协议规定 | 标准、规范、RFC |
| 某个版本怎样实现 | 对应版本的官方手册、源码与发布说明 |
| 算法或研究结论 | 原始论文与明确假设的推导 |
| 当前环境的实际表现 | 可重复的最小实验与测量记录 |

书籍与社区文章用于帮助理解，关键断言再追到直接依据。区分“规范保证”“实现细节”和“本次实验观察”，三者不能互相替代。

# 在正文里怎么写

紧挨结论写清适用条件，再附直接来源。涉及易变行为时补版本与核验日期；这些是技术依据，不是学习时间属性。

- **版本待核验**：`VERSION_CHECK` 后说明具体疑点，例如某 API 在目标版本是否仍适用。
- **性能待实测**：`MEASURE_LOCALLY` 后说明比较对象和指标，不保留无条件的固定倍数。
- **已有测量**：记录环境、配置、输入规模、命令和结果范围，保留复现信息。

标记是待办线索，不能替代修正。行为与笔记冲突、依赖升级、来源失效或出现安全通告时复查；影响当前学习的问题记入[审计与迁移队列](/00-Hub%20(学习入口)/07-Vault%20Audit%20&%20Migration%20Queue%20(审计与迁移队列).md)。

# 常用来源

| 方向 | 入口 |
| --- | --- |
| C++、Python、Go | [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) · [Python 文档](https://docs.python.org/3/) · [Go 文档](https://go.dev/doc/) |
| Linux、网络与数据库 | [Linux man-pages](https://man7.org/linux/man-pages/) · [RFC Editor](https://www.rfc-editor.org/) · [MySQL 8.4 手册](https://dev.mysql.com/doc/refman/8.4/en/) |
| 云原生、安全与 AI 风险 | [Kubernetes 文档](https://kubernetes.io/docs/) · [OWASP](https://owasp.org/) · [NIST AI RMF](https://www.nist.gov/itl/ai-risk-management-framework) |

入口用于找资料，引用时尽量链接具体章节，并匹配实际环境版本。工程指南和风险框架各有用途，不等同于语言标准或具体 API 规范。
