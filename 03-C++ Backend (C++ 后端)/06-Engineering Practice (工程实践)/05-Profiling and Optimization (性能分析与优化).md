---
study_stage: backlog
---

> [!note] 方法论坐标
> 基准实验与剖析方法统一见 [Performance Benchmarking and Profiling (性能基准与剖析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md)；本篇聚焦 C++ release 构建、perf、分配器和 Google Benchmark。


> [!summary] 核心摘要
>
> 优化的起点是可复现测量：用 profiler 找到热点，提出单一假设并验证收益与回归；没有测量证据的“优化”通常只是复杂度转移。

# 从性能目标到可复现实验

性能优化首先要写出**要改善的量**：吞吐（requests/s）、延迟（尤其 P95/P99）、CPU 时间、内存峰值或分配率。不同目标可能冲突；单次微基准更快，不等于服务端到端延迟更低。保留输入规模、并发数、硬件、编译器与编译选项、运行环境及基线结果，才能比较“前后”。

1. 用真实或代表性的负载复现问题，确认瓶颈是 CPU、等待 I/O、锁竞争、分配还是下游服务。
2. 用合适的工具定位热点，提出**可证伪的假设**，一次只改一个主要变量。
3. 在相同条件下多次测量，报告波动和退化；同时跑正确性测试。
4. 收益不足以抵消复杂度、内存或维护成本时回退该优化。

“80% 时间必在 20% 代码”只是提醒先找热点的经验说法，不能用它推断本项目的实际占比。

## 计时与采样各解决什么

简短的墙钟计时可看一个操作的端到端耗时，但不能告诉你时间花在哪里；对跨线程或阻塞 I/O 的服务，CPU 时间与墙钟时间也不同。单调计时用 `steady_clock`，不要假设 `high_resolution_clock` 一定单调。

```cpp
#include <chrono>
#include <iostream>

auto start = std::chrono::steady_clock::now();
// 调用需要测量的操作
auto elapsed = std::chrono::steady_clock::now() - start;
std::cout << std::chrono::duration<double, std::milli>(elapsed).count()
          << " ms\n";
```

这一片段示意测量位置，不构成独立程序。短操作应重复多次并控制预热、缓存、输入分布和后台负载；不要只凭一次结果下结论。

Linux 上先用 `perf stat` 看整体事件，再用 `perf record` / `perf report` 采样调用栈。`-g` 记录调用链；`-a` 是系统范围采样，不应作为只分析本程序的默认参数。事件和堆栈可用性受 CPU、内核、权限与编译选项影响。

```bash
perf stat -- ./server --workload sample
perf record -g -- ./server --workload sample
perf report
```

`perf report` 中某函数占 45% 采样，并不直接证明“它造成 45% 请求延迟”。先看所采事件、是否包含等待时间、该函数是自身开销还是子调用累计开销，再结合调用栈和业务指标判断。必要时用火焰图观察宽栈，但火焰图横轴是采样数量，不是请求时间线。参考 [perf record 手册](https://man7.org/linux/man-pages/man1/perf-record.1.html)。

## 用微基准检验一个局部假设

Google Benchmark 适合验证局部实现差异。下面测量对给定长度数组的求和；编译须链接项目已安装的 Google Benchmark，示例不是 C++ 标准库自带工具。

```cpp
#include <benchmark/benchmark.h>
#include <numeric>
#include <vector>

static void BM_Sum(benchmark::State& state) {
    std::vector<int> values(static_cast<std::size_t>(state.range(0)), 1);
    for (auto _ : state) {
        benchmark::DoNotOptimize(values.data());
        auto total = std::accumulate(values.begin(), values.end(), 0LL);
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(BM_Sum)->Arg(1'000)->Arg(100'000);
BENCHMARK_MAIN();
```

`DoNotOptimize` 能减少结果被无用代码消除的风险，但**不保证表达式内部不被优化或不会循环外提**；必要时检查反汇编、换输入和对照组。报告重复测量的分布，不把最小值当作唯一事实。参考 [Google Benchmark User Guide](https://github.com/google/benchmark/blob/main/docs/user_guide.md)。

## 优化建议必须带前提

| 假设 | 可尝试的改动 | 必须验证 |
| --- | --- | --- |
| `std::vector` 增长反复分配 | 已知规模时 `reserve` | 容量是否过度预留、峰值内存与总耗时 |
| 对象复制占热点 | 减少不必要的复制 | 是否改变所有权/生存期；值传递是否本来可移动 |
| 数据遍历出现缓存未命中 | 比较连续容器与节点容器 | 插入删除成本、稳定引用需求和实际访问模式 |
| 锁争用严重 | 缩小临界区、分片或调整任务粒度 | 正确性、吞吐、尾延迟、饥饿与复杂度 |
| 虚调用处于热点 | 评估去虚化或其他分派方式 | 编译器是否已经优化、二进制尺寸和可维护性 |

不要把“`const std::string&` 一定比值传递快”“虚函数一定不能内联”“vector 一定胜过 list”写成定律。调用方可能传右值，值传递可能直接移动；编译器也可能去虚化。只有与 API 语义一致且在代表性负载上获益，改动才成立。

## 验收一项优化

保留基线与改动后的同一套测试：正确性、吞吐、P95/P99、CPU/内存、分配次数及异常路径。尤其检查高并发下锁竞争和尾延迟是否恶化。`ASan` / `UBSan` / `TSan` 分别用于发现特定类别错误，不能把它们的高开销运行结果直接当作 release 性能数据。

> [!note] 最小交付证据
> 写清“问题负载 → 指标基线 → profiler 证据 → 修改假设 → 对照结果 → 正确性回归”。如果无法复现原问题或收益落在测量噪声内，先不要宣称优化成功。
