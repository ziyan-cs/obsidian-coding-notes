---
tags: [language/cpp, career/backend, vault/navigation]
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 00-C++ Backend Core Map (后端能力地图)

> [!abstract] 一句话结论：C++ 后端的竞争力不在会多少语法，而在能用生命周期、并发、网络、系统和测量方法构建可解释的服务。

## 学习顺序

```text
对象生命周期 / 所有权
        ↓
值语义 / 移动 / 错误边界
        ↓
线程同步 / 原子 / 内存序
        ↓
Linux I/O / socket / epoll / buffer
        ↓
CMake / 测试 / Sanitizer / gdb / perf
        ↓
项目：连接管理、优雅关闭、可观测性、压测
```

## 现有笔记优先级

| 能力 | 先读现有主题 | 必须能做到 |
| --- | --- | --- |
| 生命周期 | RAII、智能指针、Rule of Five、move | 解释谁拥有资源、何时释放 |
| 正确性 | exception、mutex、condition variable、atomic | 写出无泄漏、无 race 的最小例子 |
| 网络 | socket、epoll、Reactor、buffer | 画出一个请求/连接生命周期 |
| 工程 | compilation/linking、CMake、testing、Sanitizer | 独立编译、测试、定位问题 |
| 性能 | cache、perf、valgrind、profiling | 先测量，后优化，能解释指标 |

## 与 Go/Python 的关系

- 先用 C++ 理解资源和并发成本，再学 Go 的 `context`、goroutine 和 GC 边界。
- Python 不替代 C++；它用于把压测日志、接口回归和数据生成自动化。
- 任何跨语言结论放入 [[11-Comparisons & Decision Records (技术选型与决策记录)/00-Decision Map (决策地图)|Decision Map]]，避免散落在语言笔记中。

## 本阶段验收

- [ ] 不看笔记解释 RAII、`shared_ptr`、data race、memory order 的边界。
- [ ] 用 Sanitizer / gdb 定位一个自造 bug。
- [ ] 画出 `accept → read → parse → handle → write → close` 的服务路径。
- [ ] 说明何时 C++ 比 Go 合适，以及代价是什么。

## 关联

- [[00-Start Here (学习入口)/01-Backend Learning Roadmap (后端学习路线)|两年学习路线]]
- [[09-Go Programming (Go 编程)/08-C++ to Go Decision Notes (C++ 到 Go 迁移决策)|C++ to Go Decision Notes]]
- [[12-Backend Projects (后端项目)/00-Project Map (项目地图)|Project Map]]
