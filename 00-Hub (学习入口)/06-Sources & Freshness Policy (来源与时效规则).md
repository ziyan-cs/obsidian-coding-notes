---
status: stable
confidence: high
verified: 2026-09-18
tags: [vault/sources, learning/verification]
---

> [!abstract] 学习定位
> 稳定原理追求正确、可解释；版本、API、性能和市场信息必须能追溯到当前一手来源。来源用于约束结论，不用于装饰文末。

> [!summary] 核心摘要
>
> 课程结构参考权威能力框架，技术事实优先查规范、官方手册、RFC、源码与维护者资料；实测结论保存环境和方法。无法确认的内容明确写出验证入口和适用边界。

# 证据层级

1. 语言标准、协议 RFC、官方文档、源码与 release notes。
2. 官方工程手册、原始论文、维护者设计文档和大厂公开工程资料。
3. 高质量书籍、课程和社区文章，只用于帮助理解或提供线索。
4. AI 生成内容只能作为初稿，不能独立证明事实。

稳定基础不要求每篇重复引用同一首页；易变 API、工具行为、安全建议和性能数字必须在相关笔记附近给出具体来源。

# 全库能力框架

| 目标 | 主要依据 | 在库中承担的作用 |
| --- | --- | --- |
| 计算机科学基础 | [ACM/IEEE-CS/AAAI CS2023](https://csed.acm.org/) | 校验算法、系统、网络、数据、安全、AI 与专业能力是否缺项 |
| 生产工程 | [Google SRE Books](https://sre.google/books/) | SLO、告警、容量、事故响应、变更与恢复 |
| 云架构评审 | [AWS Well-Architected](https://docs.aws.amazon.com/wellarchitected/latest/framework/welcome.html) | 运行、安全、可靠性、性能、成本与可持续性 |
| 开发者能力 | [Microsoft Developer Career Path](https://learn.microsoft.com/en-us/training/career-paths/developer) | 端到端交付、分布式系统、运行责任与持续改进 |
| 行业方向 | 字节 Seed、阿里 AI Infra 等官方招聘与团队页面 | 只用于判断能力交集，不作为稳定技术事实 |

岗位描述是时点样本，不能推导整个市场占比；只把多家公司反复出现的能力纳入主线。

# 模块一手来源

| 模块 | 首选入口 |
| --- | --- |
| C++ | [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)、ISO 草案与编译器文档 |
| Python | [Python Documentation](https://docs.python.org/3/)、[PyPA Packaging Guide](https://packaging.python.org/) |
| Go | [Go Documentation](https://go.dev/doc/)、标准库与 release notes |
| Linux | [Linux man-pages](https://man7.org/linux/man-pages/)、[Kernel Documentation](https://docs.kernel.org/) |
| 网络协议 | [RFC Editor](https://www.rfc-editor.org/)、IETF 工作组文档 |
| MySQL | [MySQL Reference Manual](https://dev.mysql.com/doc/refman/8.4/en/) |
| Redis | [Redis Documentation](https://redis.io/docs/latest/) |
| Kafka | [Apache Kafka Documentation](https://kafka.apache.org/documentation/) |
| Kubernetes | [Kubernetes Documentation](https://kubernetes.io/docs/) 与 CNCF 报告 |
| 安全 | [NIST SSDF](https://csrc.nist.gov/projects/ssdf)、[OWASP Top 10](https://top10.owasp.org/) 与 CISA |
| AI 系统 | [NIST AI RMF](https://www.nist.gov/itl/ai-risk-management-framework)、[OWASP GenAI Security](https://genai.owasp.org/) 与 [NVIDIA Triton 文档](https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/) |

# 何时更新 verified

更新 verified 的条件是实际审阅了该笔记的结构和关键结论。它不表示所有外部资料当天重新发布或全部重新实验。

下列内容需要在正文中写明版本或验证入口：

- 编译器、解释器、数据库、Kubernetes 和 SDK 行为；
- 已废弃或实验性 API；
- 云产品限制、价格、配额和默认设置；
- 性能、延迟、容量与硬件数字；
- 招聘趋势、行业采用率和安全风险清单。

使用 VERSION_CHECK 标识随版本变化的行为，使用 MEASURE_LOCALLY 标识必须在目标环境测量的性能结论。

# 实验记录

性能和兼容性结论至少保存：

- 软件与硬件版本；
- 操作系统、编译选项和关键配置；
- 输入规模、并发、预热与持续时间；
- 命令、原始数据和统计方法；
- 观察到的结果与不能推广的边界。

没有这些条件的数字只能作为待验证线索，不能写成背诵结论。

# 更新触发器

- 新版本、release note 或规范修订改变了行为。
- 实际项目现象与笔记冲突。
- 面试或 review 中无法解释断言成立的前提。
- 链接失效、API 弃用或工具命令不再可复现。
- 安全通告影响当前依赖或部署。

更新后优先修改结论与边界；只有确有复盘价值时才保留简短变更记录，避免笔记变成流水账。

