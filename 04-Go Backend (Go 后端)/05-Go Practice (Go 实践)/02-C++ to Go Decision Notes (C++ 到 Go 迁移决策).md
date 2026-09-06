---
tags: [language/go, language/cpp, comparison]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 02-C++ to Go Decision Notes (C++ 到 Go 迁移决策)

> [!abstract] 一句话结论：C++ 训练你控制资源、成本和底层细节；Go 训练你用更小的语言表面积交付并维护服务。二者是互补，不是替代关系。

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

> [!warning] 翻译语法不是迁移设计
> 把 C++ 的类层次、手写线程管理或“零成本抽象”原样搬进 Go，通常会失去 Go 的可读性。迁移前先写清服务边界、错误策略、取消传播和性能预算。

## 30 秒回答

C++ 与 Go 的共同点是都要求理解资源、并发和成本；差别是控制方式。C++ 适合细粒度资源与性能控制，Go 适合以标准工具链和小 interface 交付服务。迁移时不逐行翻译，而是重新选择包边界、错误返回、`context` 取消和测量方法。

## 自测

1. 为什么 `defer` 不能替代 C++ 析构函数的全部语义？
2. 一个接口只有一个调用者时，什么时候不该急着抽 Go interface？
3. 你会拿什么 profile 或压测证据支持一次语言/架构调整？
