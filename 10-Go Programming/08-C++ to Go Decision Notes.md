---
tags: [language/go, language/cpp, comparison]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# C++ to Go Decision Notes — 不要把 C++ 逐行翻译成 Go

> **一句话结论**：C++ 训练你控制资源、成本和底层细节；Go 训练你用更小的语言表面积交付并维护服务。二者是互补，不是替代关系。

| 主题 | C++ 思维 | Go 思维 | 迁移时最易错 |
| --- | --- | --- | --- |
| 资源清理 | RAII，析构确定性 | `defer` + 显式 `Close` | 误以为 `defer` 与析构等价 |
| 错误 | exception / `expected` | `error` 返回值 + wrapping | 忽略 error 或到处 panic |
| 抽象 | template / virtual | 小 interface + composition | 设计过多 interface 层 |
| 并发 | thread、lock、atomic | goroutine、channel、mutex、context | 以为 channel 自动消除 race |
| 性能 | 手动布局/分配/缓存 | profiling 后优化，理解 GC | 用 C++ 微优化替代测量 |
| 构建 | CMake / package manager | `go.mod` + go command | 试图复制复杂构建层 |

## 什么时候选哪一个

- 更偏底层网络、极致延迟、已有 C++ 基础设施：C++ 仍合理。
- 典型业务 API、团队协作、快速稳定地交付服务：Go 通常更合适。
- 数据清洗、测试工具、运维自动化：Python 通常更省力。

选择必须看团队、已有系统、延迟/吞吐目标、运维能力和依赖生态；不要只凭语言喜好下结论。
