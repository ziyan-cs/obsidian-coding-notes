---
study_stage: backlog
---

> [!abstract] 学习定位
> goroutine 很轻但并非免费；调度、栈、分配和 GC 都会影响尾延迟，优化必须从 profile 证据开始。

> [!summary] 核心摘要
>
> 阻塞、逃逸分配和堆增长都会影响调度与 GC；先测 CPU、heap、goroutine 与 trace，再针对已证明的瓶颈降低分配、阻塞或争用。

# G-M-P 心智模型

`G` 表示 goroutine，`M` 表示 OS thread，`P` 是运行 Go 代码所需的调度资源。调度器把可运行的 G 安排到持有 P 的 M 上。这个模型解释许多 goroutine 如何复用较少线程，但具体队列、抢占和栈实现会随版本演进，不应背成稳定 API。

```text
runnable G ----+
runnable G ----+--> P schedules work --> M / OS thread --> CPU
blocked G waits for I/O, timer, channel or lock before runnable again
```

网络轮询器可让等待网络 I/O 的 goroutine 暂停；阻塞系统调用可能需要运行时安排其他线程继续工作。无界 goroutine 仍占用栈、调度元数据、引用对象和下游连接，因此“轻量”不代表免费。

# 栈、逃逸与分配

goroutine 栈可按需增长。逃逸分析决定对象是否需要比当前栈帧活得更久；逃逸到堆会增加 GC 可追踪对象。用 `go build -gcflags=-m` 可观察编译器判断，但目标不是强迫所有值留在栈上。

常见分配来源包括格式化、字符串与字节切片转换、闭包捕获、接口值和切片反复增长。是否优化取决于调用频率、对象大小和 profile，占比很小的分配不应牺牲可读性。

# GC 的取舍

存活堆越大、指针越多、分配速率越高，GC 工作通常越多。调优是在 CPU、内存和延迟之间取舍：

- 降低短命对象分配可减轻 GC，但对象池会增加生命周期与错误复用风险。
- 降低回收频率通常允许更高内存占用。
- 内存目标不能替代容器限制、容量规划和泄漏排查。
- goroutine 泄漏会让引用对象保持可达，表现为内存与 goroutine 数共同增长。

具体参数与指标语义以项目使用的 Go 版本为准，不把经验值写成通用常数。

# 调度压力从哪里来

`GOMAXPROCS` 控制可同时执行 Go 代码的逻辑处理器数量，不等于 goroutine 数，也不直接等于进程能够创建的 OS thread 数。CPU 密集任务的可运行 goroutine 远多于 P 时，会增加调度等待；大量阻塞 syscall、CGO 调用或锁竞争则可能增加线程和 off-CPU 时间。

| 现象 | 可能原因 | 需要的证据 |
| --- | --- | --- |
| goroutine 数持续增长 | 下游无超时、通道无人接收、ticker 未停止 | goroutine profile、创建栈、取消路径 |
| CPU 高但吞吐不升 | 忙等、序列化、GC、锁自旋 | CPU profile、trace、runtime 指标 |
| P99 周期性尖峰 | GC、批量任务、连接池等待 | trace 时间线、heap、依赖延迟 |
| RSS 高于 Go heap | mmap、线程栈、CGO、页未归还 OS | runtime/metrics、进程内存映射 |

容器中要同时观察 Go runtime 看到的 CPU/内存目标与实际 cgroup limit。Go 版本会改进容器感知行为，部署升级后应重新验证 `GOMAXPROCS`、内存峰值和延迟，而不是沿用旧经验。

# 堆目标与内存上限

`GOGC` 影响下一次 GC 的堆增长目标；较低值通常以更多 CPU 换更低堆峰值，较高值反之。`GOMEMLIMIT` 给运行时一个软内存目标，不是操作系统强制上限，也不能覆盖非 Go heap 内存。设置得过低可能导致 GC 频繁运行并吞噬 CPU。

调优前先区分：

- `inuse_space`：当前仍存活或持有的堆对象；
- `alloc_space`：一段时间内累计分配，用来发现高分配热点；
- RSS：进程实际驻留物理内存，还包含栈、代码、mmap 与运行时开销；
- allocation rate：单位时间分配量，常比单次 heap 快照更能解释 GC 压力。

对象池只适合生命周期清晰、重置成本可控且 profile 已证明频繁分配的对象。`sync.Pool` 内容可在 GC 时被清除，不能当缓存或资源池使用。

# 在线诊断

服务中可以在受保护的管理端口暴露 `net/http/pprof`，在故障窗口采集短时 profile。生产采样要限制权限、持续时间与并发，避免诊断端点泄露内部状态或反过来制造负载。

```bash
go tool pprof http://127.0.0.1:6060/debug/pprof/profile?seconds=30
go tool pprof http://127.0.0.1:6060/debug/pprof/heap
curl -o trace.out 'http://127.0.0.1:6060/debug/pprof/trace?seconds=5'
go tool trace trace.out
```

配合 [runtime/metrics](https://pkg.go.dev/runtime/metrics) 和 [Go Execution Tracer](https://go.dev/doc/diagnostics#execution-tracer) 观察版本对应的指标语义。

# 证据驱动的排查

1. 固定输入，记录吞吐、P95/P99、CPU、RSS、goroutine 数和分配率。
2. CPU profile 找热点；heap/alloc profile 区分存活对象与累计分配。
3. goroutine profile 查阻塞与泄漏；trace 观察调度、阻塞和 GC 时间线。
4. 一次只改一个因素，再以同样负载比较。

```bash
go test -bench=. -benchmem ./...
go test -run=^$ -bench=BenchmarkName -cpuprofile=cpu.out ./path
go tool pprof cpu.out
```

> [!question]- 理解检查
> 如果 P99 上升且 goroutine 数持续增长，应怎样区分下游变慢、锁竞争和 goroutine 泄漏？

> [!info]- 官方参考
> - [Diagnostics](https://go.dev/doc/diagnostics)
> - [A Guide to the Go Garbage Collector](https://go.dev/doc/gc-guide)
