---
study_stage: backlog
tags: [language/go, language/cpp, comparison]
---

> [!abstract] 学习定位
> C++ 训练你控制资源、成本和底层细节；Go 训练你用更小的语言表面积交付并维护服务。二者是互补，不是替代关系。

| 主题 | C++ 思维 | Go 思维 | 迁移时最易错 |
| --- | --- | --- | --- |
| 资源清理 | RAII，析构确定性 | `defer` + 显式 `Close` | 误以为 `defer` 与析构等价 |
| 错误 | exception / `expected` | `error` 返回值 + wrapping | 忽略 error 或到处 panic |
| 抽象 | template / virtual | 小 interface + composition | 设计过多 interface 层 |
| 并发 | thread、lock、atomic | goroutine、channel、mutex、context | 以为 channel 自动消除 race |
| 性能 | 手动布局/分配/缓存 | profiling 后优化，理解 GC | 用 C++ 微优化替代测量 |
| 构建 | CMake / package manager | `go.mod` + go command | 试图复制复杂构建层 |

# 什么时候选哪一个

- 更偏底层网络、极致延迟、已有 C++ 基础设施：C++ 仍合理。
- 典型业务 API、团队协作、快速稳定地交付服务：Go 通常更合适。
- 数据清洗、测试工具、运维自动化：Python 通常更省力。

选择必须看团队、已有系统、延迟/吞吐目标、运维能力和依赖生态；不要只凭语言喜好下结论。

实际判断可以从一条请求路径出发：测清 P95/P99 延迟、CPU、内存、GC 暂停、依赖等待和部署复杂度。若瓶颈是慢 SQL 或下游超时，换语言通常解决不了；若热点是频繁分配，则先在 Go 中用 pprof 找到分配点，再决定是否需要改变数据结构或抽出 C++ 组件。最终比较应在同一负载、相同正确性和相同运维约束下进行。

# 两个迁移实例

在 C++ 中，锁保护的任务队列常和线程生命周期一起封装进类。迁到 Go 时，不要机械地把每个请求放进一个新 goroutine：先限定并发数和排队长度，再让请求 `context` 决定能否继续等待。`channel` 适合交接任务，`mutex` 仍适合保护共享状态；两者都不能替你决定过载时拒绝还是丢弃。

C++ 的 RAII 对象通常在作用域结束时析构；Go 的 `defer` 在**当前函数返回**时执行，而不是离开任意代码块时执行。循环内打开资源若直接 `defer Close()`，会一直积累到外层函数返回；把单次处理放进独立函数，或在迭代结束处显式关闭。若关闭结果影响写入是否完整，还要处理 `Close` 返回的错误，不能只 `defer` 后忽略。

> [!warning] 翻译语法不是迁移设计
> 把 C++ 的类层次、手写线程管理或“零成本抽象”原样搬进 Go，通常会失去 Go 的可读性。迁移前先写清服务边界、错误策略、取消传播和性能预算。

> [!summary] 核心摘要
>
> C++ 与 Go 的共同点是都要求理解资源、并发和成本；差别是控制方式。C++ 适合细粒度资源与性能控制，Go 适合以标准工具链和小 interface 交付服务。迁移时不逐行翻译，而是重新选择包边界、错误返回、`context` 取消和测量方法。

> [!question]- 自测：先回答再展开
> 1. 为什么 `defer` 不能替代 C++ 析构函数的全部语义？
> 2. 一个接口只有一个调用者时，什么时候不该急着抽 Go interface？
> 3. 你会拿什么 profile 或压测证据支持一次语言/架构调整？

> [!info]- 延伸阅读
> - 下一步：[03-First Service Delivery (首个服务交付)](/05-Go%20Backend%20(Go%20后端)/05-Go%20Practice%20(Go%20实践)/03-First%20Service%20Delivery%20(首个服务交付).md)

