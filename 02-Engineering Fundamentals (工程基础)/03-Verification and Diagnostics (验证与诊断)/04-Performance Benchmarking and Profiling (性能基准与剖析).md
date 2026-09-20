---
study_stage: backlog
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 性能工作先建立可重复基线，再用 profile 解释资源花在哪里。基准给出“多慢”，剖析回答“慢在哪里”，系统模型解释“为什么”；没有相同条件下的前后证据，就不能宣称优化有效。

# 先定义性能契约

明确场景、负载和约束：吞吐量、延迟分布、错误率、资源成本、数据规模、并发连接、预热时间与持续时间。平均延迟会掩盖长尾，至少观察 P50/P95/P99 和最大值，并区分成功与失败请求。

```text
workload -> load generator -> system under test -> dependencies
              |                    |
          request result       CPU/memory/I/O/profile
```

协调遗漏（Coordinated Omission）会让压测器在系统变慢时减少发请求，从而低估尾延迟。区分 closed model（固定并发）与 open model（固定到达率），选择符合真实业务的模型。

# 可复现实验设计

每次报告记录：revision、release/debug 构建、机器与 CPU、内核、依赖版本、配置、数据集、负载模型、预热、持续时间和原始结果。固定 CPU 频率、容器限额、后台任务和网络条件，或明确它们是实验变量。

操作系统缓存、JIT、连接建立和内存分配会造成冷启动差异。预热应模拟真实生命周期；既不能把冷启动藏掉，也不能把只发生一次的初始化混入稳态结论。

至少重复多轮并展示分布或置信区间。差异小于运行噪声时，结论应是“未观察到稳定改善”。

# 从系统指标到 profile

先按资源分类，再选工具：

| 现象 | 先看 | 深入工具 |
|---|---|---|
| 单核或多核 CPU 高 | user/system、run queue | `perf record`、火焰图 |
| CPU 不高但延迟高 | off-CPU、锁、I/O wait | off-CPU profile、`strace`、eBPF |
| 内存增长 | RSS、heap、page fault | heap profile、Massif、allocation profile |
| 网络慢 | RTT、重传、队列、socket | `ss`、`tcpdump`、协议 trace |
| 磁盘慢 | IOPS、吞吐、await、queue | `iostat`、block tracing |

不要直接从“CPU 高”跳到重写算法。可能是无效重试、序列化、锁竞争、内核调用或观测开销。

## perf 与火焰图

```bash
perf stat -r 5 -- ./app
perf record -F 99 -g -- ./app
perf report
perf annotate
```

`perf stat` 用于比较周期、指令、分支、cache miss 和上下文切换；硬件计数器支持依赖 CPU 与权限。IPC 不是越高越好，它必须结合工作量、stall 和算法变化解释。

火焰图横向宽度代表样本占比，不代表时间顺序；纵向是调用栈。顶部宽平台常是直接消耗 CPU 的位置，但采样相关性不自动证明因果。确认符号、调用栈展开方式和采样频率，否则 `[unknown]` 或断裂栈会误导判断。

# 微基准、组件压测与端到端压测

微基准隔离一个操作，适合比较算法、分配或序列化，但容易被编译器消除、常量折叠和不现实的数据分布污染。使用框架提供的 `DoNotOptimize`/black box，并让输入规模跨越缓存层级。

组件压测验证服务与真实数据库/缓存的协作；端到端压测验证容量、尾延迟和降级。三者回答不同问题，不能用微基准的纳秒差异推导线上吞吐收益。

## 优化循环与停止条件

```text
baseline -> hypothesis -> measurement -> change
         -> same measurement -> correctness regression -> decision
```

优先优化对用户目标贡献最大的瓶颈，并同时回归正确性。Amdahl 定律提醒：局部加速受该部分占总时间比例限制。停止条件可以是达到 SLO、成本预算、收益小于复杂度，或瓶颈已转移。

最终报告应包含原始数据和负面结果，避免只保留成功优化。线上灰度还需观察错误率、资源成本和长尾是否恶化。

# 实践与资料

- [ ] 为一个 HTTP 服务定义 open/closed 两种负载并解释选择。
- [ ] 保存基线、火焰图和修改后结果，证明同条件下是否改善。
- [ ] 分别制造 CPU、锁和 I/O 瓶颈，选择不同证据定位。
- [ ] 检查压测器自身 CPU、网络和连接数，证明它不是瓶颈。

## 关联专题

- [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)：从线上指标进入有假设的性能分析。
- [Benchmarking (压测)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/05-Benchmarking%20(压测).md)：服务器网络压测的工具与参数。

## 参考资料

- [Linux perf documentation](https://perf.wiki.kernel.org/)
- [Brendan Gregg - Flame Graphs](https://www.brendangregg.com/flamegraphs.html)
- [Google Benchmark User Guide](https://benchmark.readthedocs.io/)
