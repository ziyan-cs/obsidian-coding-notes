---
study_stage: backlog
review_due:
tags: [topic/project-delivery, topic/resource-management]
---

> [!abstract] 本篇要点
> 结论：GC 只处理内存可达性；文件、socket、锁、事务等资源仍须有确定的关闭协议。
>
> GC 只回收不可达内存，不替代文件、socket、锁和事务的关闭协议。C++ 用 RAII，Go 用 defer，Python 用 context manager，本质都是把资源生命周期绑定到明确作用域。

# 工程落点

- C++：用对象边界表达 ownership；连接、锁、文件句柄都应由 RAII 包装。
- Go：`defer cancel()`、`defer resp.Body.Close()` 应紧邻成功获得资源的位置；循环内的 `defer` 要先判断生命周期是否过长。
- Python：文件与事务优先写成 `with`；HTTP response、线程池等仍应按库文档显式关闭。

# 统一的资源责任模型

```text
acquire -> one clear owner -> use -> release
                 |
                 +-> ownership transfer must be explicit
```

无论语言是否有 GC，外部资源都需要“谁拥有、何时转移、谁关闭”的答案。C++ 的 RAII 在作用域退出时执行析构；Go 的 `defer` 在函数返回时执行；Python 的 `with` 调用 context manager 的退出逻辑。它们都不能替代资源协议本身：连接池里的连接通常应归还池而非真正关闭。

# 面试表达模板

“我先区分内存和外部资源。GC 解决的是不可达内存，不能替我决定事务提交、锁释放或 socket 关闭。工程上我把资源责任绑定到最小清晰作用域，并为所有权转移和异常路径写测试或日志证据。”

> [!warning] 常见误区
> `defer`、`with` 和析构都不是“自动解决资源管理”。它们只在对应作用域、协议和异常路径被正确设计时生效。

> [!question]- 决策复盘
> 内存生命周期和外部资源生命周期不是一回事。GC 可以回收不可达内存，却不能替你决定何时提交事务、解锁或关闭 socket；因此资源责任必须在获得处明确。
>
> - 自测：一个函数创建 DB 连接、打开文件并加锁，三种语言各应把释放写在哪里？
> - 自测：为什么不能用“对象最终会被 GC”解释文件描述符不会泄漏？

**参考资料：** 下一步：[02-Concurrency Models (并发模型)](<./02-Concurrency Models (并发模型).md>)

# 考察题目

## 1. 动手题

从一条真实请求列出连接、任务、缓冲和事务的创建者与释放者；注入每个步骤失败。

> [!note]- 验收要点
> 每个资源释放一次，取消和关闭路径无泄漏，借用不越过所有者寿命。

## 2. 机制追问

一次请求跨网络、线程和数据库时，如何画出资源所有权？

> [!note]- 参考答案
> 对连接、缓冲、任务和事务分别指定创建者、持有者、释放时机与失败路径；借用关系不能跨越被借用对象生命周期。
