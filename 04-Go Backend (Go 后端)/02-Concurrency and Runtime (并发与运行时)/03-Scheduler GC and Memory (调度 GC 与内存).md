---
status: stable
review_due: 2026-09-13
confidence: medium
verified: 2026-09-06
---

# 03-Scheduler GC and Memory (调度 GC 与内存)

> [!abstract] 一句话结论：goroutine 很轻但并非免费；调度、栈增长、分配和 GC 都会影响尾延迟，性能优化先用 profile 证明问题存在。

## 心智模型

- Go 运行时调度 goroutine 到 OS thread 上执行；阻塞系统调用、忙循环和无界 goroutine 都可能影响调度效率。
- 堆分配会增加 GC 工作；逃逸分析可帮助理解变量为何离开栈，但不是为了“零分配”而写难懂代码。
- 排查性能先测吞吐、P95/P99、goroutine 数、分配与 CPU profile，再做小幅验证。

## 常见误区

- 每个请求都永久启动后台 goroutine，却没有退出路径。
- 用 channel 代替所有同步，而不考虑所有权、缓冲和关闭责任。
