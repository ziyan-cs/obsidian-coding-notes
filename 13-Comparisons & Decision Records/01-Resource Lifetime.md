---
tags: [comparison, resource/lifetime]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# Resource Lifetime — RAII、defer、GC 的边界

> **结论**：GC 只处理内存可达性；文件、socket、锁、事务等资源仍须有确定的关闭协议。

| 维度 | C++ | Go | Python |
| --- | --- | --- | --- |
| 主机制 | RAII / 析构 | 显式 `Close` + `defer` | `with` + 显式 close |
| 内存 | 确定性释放或智能指针 | GC | 引用计数/GC 实现细节不可作资源协议 |
| 失败路径 | 析构自动执行 | `defer` 在函数返回执行 | context manager 的 `__exit__` |
| 常见坑 | 悬垂引用、循环引用 | 循环内 defer、漏 Close | 忘记 `with`、依赖析构时机 |

**选型规则**：资源的获得点旁边就写清释放策略；让所有权或关闭责任只有一个明确主体。
