---
status: stable
confidence: medium
verified: 2026-09-17
---

> [!abstract] 一句话结论：goroutine 很轻但并非免费；调度、栈增长、分配和 GC 都会影响尾延迟，性能优化先用 profile 证明问题存在。

> [!summary]- 复述检查：学完后再展开
>
> **回答展开**：goroutine 的阻塞、逃逸分配和堆增长都会影响调度与 GC；先用 CPU、heap、trace 等 profile 定位，再针对瓶颈降低分配或并发争用。

# 心智模型

- Go 运行时调度 goroutine 到 OS thread 上执行；阻塞系统调用、忙循环和无界 goroutine 都可能影响调度效率。
- 堆分配会增加 GC 工作；逃逸分析可帮助理解变量为何离开栈，但不是为了“零分配”而写难懂代码。
- 排查性能先测吞吐、P95/P99、goroutine 数、分配与 CPU profile，再做小幅验证。

# 从一次请求看运行时

```text
request arrives
      |
goroutine runs on an M (OS thread)
      |
blocking / allocation / CPU work
      |
scheduler chooses next runnable goroutine
```

`G` 是 goroutine，`M` 是 OS thread，`P` 是执行 Go 代码所需的调度资源。初学阶段不必背调度器细节；先养成两个判断：阻塞 I/O 应使用支持取消的 API，CPU 密集循环要限制并发。出现慢请求时，先用 `go test -bench`、`pprof` 或运行时指标找证据，不能只凭“goroutine 很轻”无限创建任务。

# 最小实验

用 `go test -bench=. -benchmem` 比较“循环中 `fmt.Sprintf`”与“复用 `strings.Builder`”的 `allocs/op`。随后用 `go tool pprof` 看 CPU 或 alloc profile。实验的结论应是某段代码在当前输入下的证据，而不是“所有分配都必须消除”。

> [!warning]- 易错点
> - 每个请求都永久启动后台 goroutine，却没有退出路径。
> - 用 channel 代替所有同步，而不考虑所有权、缓冲和关闭责任。
