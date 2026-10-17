---
study_stage: reference
tags: [vault/sources, learning/verification]
---

> [!abstract] 核验原则
> AI 生成内容只作初稿。稳定原理要能解释；版本、API、安全建议和性能结论要能追到一手来源或本机实验。

# 来源优先级

1. 语言/协议标准、RFC、官方手册、源码与 release notes。
2. 原始论文、维护者设计文档、官方工程手册。
3. 高质量书籍和社区文章可辅助理解，不能单独支撑高风险断言。

岗位信息只代表采样时点；不能由少量招聘描述推导行业普遍结论。

# 需要标版本或实测的内容

- 编译器、语言运行时、数据库、Kubernetes、SDK 与云产品行为。
- 弃用或实验性 API、安全建议、默认参数和配额。
- 性能、延迟、容量及硬件相关数字。

在结论旁记录版本、日期和直接来源；未核实写 `VERSION_CHECK`，需本机测量写 `MEASURE_LOCALLY`。性能记录至少包含软硬件、配置、输入规模、运行命令和结果边界；没有条件的数据只作线索。

# 何时复查

版本或规范更新、实际行为与笔记冲突、链接失效、发现安全通告，或无法解释结论成立的前提时复查。修正文中结论与边界即可，不维护逐日更新流水账。

常用一手入口：[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) · [Python Docs](https://docs.python.org/3/) · [Go Docs](https://go.dev/doc/) · [Linux man-pages](https://man7.org/linux/man-pages/) · [RFC Editor](https://www.rfc-editor.org/) · [MySQL Manual](https://dev.mysql.com/doc/refman/8.4/en/) · [Kubernetes Docs](https://kubernetes.io/docs/) · [OWASP](https://owasp.org/) · [NIST AI RMF](https://www.nist.gov/itl/ai-risk-management-framework)
