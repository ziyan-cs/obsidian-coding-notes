---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [cs/os, learning/foundation]
---

> [!abstract] 学习目标
> 分清模式转换、线程切换与地址空间切换，理解调度目标之间的冲突，并用当前 Linux 的调度类与 EEVDF 模型解释实际行为。

# 一次切换究竟换了什么

**上下文（context）**是任务继续执行所需的状态，包括程序计数器、栈指针、通用寄存器、部分控制状态以及内核维护的调度信息。切换时保存多少状态取决于架构、内核路径和是否使用了相关扩展状态。

| 场景 | 特权级变化 | 当前线程变化 | 地址空间变化 |
|---|---:|---:|---:|
| 普通系统调用后返回 | 是 | 通常否 | 通常否 |
| 同进程线程 A → B | 不一定 | 是 | 通常否 |
| 进程 A → B | 不一定 | 是 | 通常是 |
| 中断处理后返回原任务 | 是 | 否 | 通常否 |

因此，“进入内核 = 上下文切换 = 刷新全部 TLB”是错误链条。地址空间切换可能改变页表根，但现代处理器可用 PCID/ASID 给 TLB 项打标签，内核也会尽量避免无条件全量失效。

## 成本不只是保存寄存器

直接成本包括内核调度路径和寄存器保存/恢复；间接成本常更大：

- 新任务的指令与数据工作集不在缓存；
- 分支预测、TLB 和预取状态与新任务不匹配；
- 多核迁移带来缓存一致性与 NUMA 远端访问；
- 任务过多造成运行队列竞争和尾延迟。

不存在跨机器通用的“切换固定耗时”。测量必须说明 CPU、内核、频率策略、负载、是否跨核和统计分位数。

# 调度要优化哪些目标

调度器面对互相冲突的目标：

- **吞吐量（throughput）**：单位时间完成更多工作；
- **周转时间（turnaround time）**：提交到完成；
- **响应时间（response time）**：提交到首次获得服务；
- **公平性（fairness）**：长期 CPU 份额符合权重；
- **截止期（deadline）**：在约束时间前完成；
- **开销与局部性**：减少调度和迁核成本。

经典算法帮助建立直觉：FCFS 简单但有护航效应；SJF/SRTF 能降低理想条件下的平均等待，却需要预测运行时间；Round Robin 用时间片换响应性；优先级调度可能饥饿，需老化等机制；MLFQ 用历史行为近似区分交互与 CPU 密集任务。

# Linux 调度框架

Linux 按调度类组织策略，常见顺序与语义如下：

| 类/策略 | 典型用途 | 核心约束 |
|---|---|---|
| Deadline：`SCHED_DEADLINE` | 有 runtime/deadline/period 模型的任务 | 需要准入与严格预算 |
| 实时：`SCHED_FIFO`、`SCHED_RR` | 低延迟、明确优先级 | 高优先级任务可长期压制普通任务 |
| 公平类：`SCHED_OTHER` | 普通进程 | 按权重分配 CPU，并兼顾响应性 |
| Idle | 仅在没有其他任务时 | 最低优先级 |

不要把 `nice` 值理解为毫秒时间片；它影响普通任务的相对权重。

## 从 CFS 到 EEVDF

旧资料常用 CFS 的虚拟运行时间与红黑树解释公平类。Linux 自 6.6 起开始向 **EEVDF（Earliest Eligible Virtual Deadline First）** 过渡，当前学习应保留历史背景，但以 EEVDF 为主：

1. 根据任务应得 CPU 份额计算 **lag**；非负 lag 表示任务尚“欠着”CPU 时间。
2. 在 eligible 的任务中比较虚拟截止期（virtual deadline）。
3. 选择最早虚拟截止期任务，使权重公平与低延迟请求能统一表达。

实现会随内核版本演进，不应背诵某个树节点字段作为永久接口。生产分析首先记录 `uname -r`，再对应版本文档和源码。

# 多核调度的额外问题

- **CPU affinity** 限制任务可运行的 CPU；绑核能改善局部性，也可能造成负载不均。
- 调度域需要在核心、共享缓存、NUMA 节点之间做负载均衡。
- SMT 逻辑 CPU 共享执行资源，并不等于两个完整物理核心。
- cgroup CPU 控制用于容器份额和上限；被 throttling 的任务不一定是代码变慢。

# 观察与实验

```bash
uname -r
ps -eo pid,tid,psr,stat,ni,cls,rtprio,comm | head
pidstat -w 1
vmstat 1
perf sched record -- sleep 5
perf sched latency
```

`pidstat -w` 区分自愿与非自愿切换；数量高不自动等于问题。把切换率与 CPU 利用率、运行队列、延迟分位数及业务吞吐一起解释。

# 检查理解

1. 系统调用为什么通常不是调度意义上的上下文切换？
2. 同进程线程切换与跨进程切换，哪些缓存/地址状态可能不同？
3. 公平、响应性和吞吐为什么无法同时无限提高？
4. EEVDF 的 eligible、lag 和 virtual deadline 各解决什么问题？

> [!summary] 本篇结论
> 上下文切换的真正成本来自执行状态和工作集变化；调度则是在公平、响应、吞吐与实时约束间做策略选择。分析 Linux 时必须绑定内核版本，当前公平类应从 EEVDF 而非只从旧 CFS 叙述出发。

## 权威依据

- [Linux EEVDF Scheduler](https://docs.kernel.org/scheduler/sched-eevdf.html)
- [Linux Scheduler documentation](https://docs.kernel.org/scheduler/index.html)
- [sched(7)](https://man7.org/linux/man-pages/man7/sched.7.html)

下一步：[08-Virtual Memory Paging and Allocation (虚拟内存、分页与分配)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/08-Virtual%20Memory%20Paging%20and%20Allocation%20(虚拟内存、分页与分配).md)
