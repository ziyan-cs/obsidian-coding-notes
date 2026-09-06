---
tags: [comparison, resource/lifetime]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-Resource Lifetime (资源生命周期)

> [!abstract] 阅读定位
>
> 结论：GC 只处理内存可达性；文件、socket、锁、事务等资源仍须有确定的关闭协议。

| 维度 | C++ | Go | Python |
| --- | --- | --- | --- |
| 主机制 | RAII / 析构 | 显式 `Close` + `defer` | `with` + 显式 close |
| 内存 | 确定性释放或智能指针 | GC | 引用计数/GC 实现细节不可作资源协议 |
| 失败路径 | 析构自动执行 | `defer` 在函数返回执行 | context manager 的 `__exit__` |
| 常见坑 | 悬垂引用、循环引用 | 循环内 defer、漏 Close | 忘记 `with`、依赖析构时机 |

**选型规则**：资源的获得点旁边就写清释放策略；让所有权或关闭责任只有一个明确主体。

## 工程落点

- C++：用对象边界表达 ownership；连接、锁、文件句柄都应由 RAII 包装。
- Go：`defer cancel()`、`defer resp.Body.Close()` 应紧邻成功获得资源的位置；循环内的 `defer` 要先判断生命周期是否过长。
- Python：文件与事务优先写成 `with`；HTTP response、线程池等仍应按库文档显式关闭。

> [!warning] 常见误区
> `defer`、`with` 和析构都不是“自动解决资源管理”。它们只在对应作用域、协议和异常路径被正确设计时生效。

## 30 秒回答与自测

**回答**：内存生命周期和外部资源生命周期不是一回事。GC 可以回收不可达内存，却不能替你决定何时提交事务、解锁或关闭 socket；因此资源责任必须在获得处明确。

- 自测：一个函数创建 DB 连接、打开文件并加锁，三种语言各应把释放写在哪里？
- 自测：为什么不能用“对象最终会被 GC”解释文件描述符不会泄漏？
