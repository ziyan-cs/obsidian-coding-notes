---
status: stable
confidence: high
content_verified: 2026-09-18
tags: [cs/architecture, cs/io, learning/foundation]
verified: 2026-10-17
review_stage: learn
review_due: 2026-10-17
---

> [!abstract] 阅读方式
> 存储层级解决“容量、延迟、带宽和成本不能同时最优”，I/O 系统解决“速度和控制方式不同的设备如何安全搬运数据”。本文不背固定纳秒表，而是建立局部性、缓存、互连、中断、DMA 与 MMIO 的因果链，并要求用本机实验验证。

> [!summary] 核心摘要
>
> 缓存按块复制相邻层数据，命中依赖时间与空间局部性；性能由命中时间、缺失率、缺失代价、带宽和并行度共同决定。设备通过控制器与片上互连通信，CPU 可轮询或处理中断，设备可借助 DMA 搬运批量数据；MMIO、DMA、一致性与 IOMMU 决定数据何时对谁可见。

# 存储层级与局部性

## 为什么必须分层

没有一种存储技术能同时提供寄存器级延迟、DRAM 容量、持久存储成本和断电保持能力。系统把不同层组合起来，让常用数据尽量靠近执行单元：

```text
             smaller / lower latency / higher cost per byte
        ┌──────────────────────────────────────────┐
        │ registers                                │
        │ L1 caches                                │
        │ private/shared mid-level caches          │
        │ last-level cache                         │
        │ DRAM                                     │
        │ persistent storage                       │
        │ remote/object/archive storage            │
        └──────────────────────────────────────────┘
             larger / higher latency / lower cost per byte
```

具体层数、容量、缓存行大小和延迟都取决于处理器、NUMA 拓扑、频率、负载与设备；固定的“L1 1ns、内存 100ns、每层慢十倍”只能作为过时印象，不能写成工程事实。

## 局部性为何有效

- **时间局部性（temporal locality）**：刚访问的数据近期可能再次访问。
- **空间局部性（spatial locality）**：访问某地址后，邻近地址可能随后被访问。
- **指令局部性**：顺序代码、循环和热点函数让 instruction cache 有效。

局部性不是“90% 时间一定执行 10% 代码”的定律。数据集、访问步长、哈希、图遍历和工作集切换都可能让局部性很差，应通过 profile 与硬件计数器判断。

## 延迟、带宽和并行度

三个概念不能混用：

| 指标 | 问题 | 例子 |
| --- | --- | --- |
| Latency | 单次请求多久返回 | 指针追逐依赖前一次结果 |
| Bandwidth | 单位时间最多搬多少数据 | 顺序扫描大数组 |
| Memory-level parallelism | 能否同时保留多个未完成请求 | 多个独立数组访问 |

顺序扫描可能有较高单次延迟，却借助预取和并发接近带宽上限；链表随机追逐即使数据量不大，也可能被依赖链限制。

# Cache 的组织与缺失

## Cache line、set 与 way

缓存以 **cache line** 为传输和一致性单位。64 byte 在许多当前桌面/服务器 CPU 上常见，但不是 ISA 保证，代码不应把它当成所有平台常量。

```text
memory address
┌──────────────── tag ────────────────┬── set index ──┬─ block offset ─┐
└─────────────────────────────────────┴───────────────┴────────────────┘
                                      │
                                      ▼
                         select one cache set
                                      │
                    compare tags in all ways of the set
```

若 cache size 为 `sets × ways × line_size`，地址的 offset 选择行内字节，index 选择 set，tag 判断目标内存块是否驻留。

| 组织 | 含义 | 主要取舍 |
| --- | --- | --- |
| Direct-mapped | 每个块只有一个候选位置 | 命中判断简单，冲突多 |
| Set-associative | 固定 set 内可放多个 way | 当前通用缓存常见折中 |
| Fully associative | 可放任意位置 | 比较和替换成本高，适合较小结构 |

## Miss 不只有“数据太大”

- **Compulsory miss**：第一次访问该块。
- **Capacity miss**：活跃工作集超过可用容量。
- **Conflict miss**：多个热点块竞争同一 set。
- **Coherence miss**：其他核心写入导致本地副本失效。

平均访问时间可用教学公式建立直觉：

```text
AMAT = hit_time + miss_rate × miss_penalty
```

多级缓存、乱序执行、重叠请求和预取会让真实测量更复杂，但公式提醒我们：只降低 miss rate 或只比较命中延迟都不够。

## 替换和预取

硬件通常使用 LRU 的近似策略、随机化或实现相关启发式，并非所有层都采用严格 LRU。预取器擅长顺序或规则步长，对指针追逐和不规则图访问效果有限；错误预取还会消耗带宽并污染缓存。

# 写入、一致性与伪共享

## 写回、写直达与 store buffer

- **Write-through**：缓存写入同时向下一层传播，状态简单但流量更大。
- **Write-back**：先更新缓存行并标 dirty，淘汰或协议要求时再写下层。
- **Write-allocate / no-write-allocate**：写 miss 时是否先把目标行取入缓存。
- **Store buffer**：让核心不必等待写入对缓存层级完全可见；它与“脏缓存行以后写回”不是同一结构。

具体组合由实现和内存类型决定。普通 DRAM、持久内存与 MMIO 可能采用不同缓存和排序属性。

## Cache coherence 不等于并发正确

一致性协议让多个核心对**同一缓存行**的副本达成规则化可见性；语言内存模型决定数据竞争、原子操作和 happens-before。硬件有 coherence 不代表不加同步就能安全共享普通变量。

## 正确理解 false sharing

两个线程写不同变量，但变量位于同一一致性单元时，缓存行会在核心间反复转移。这叫 false sharing。

错误修复：

```cpp
struct alignas(64) BadFix {
    std::atomic<std::uint64_t> a;
    std::atomic<std::uint64_t> b;
}; // 结构体对齐了，但 a 与 b 仍可能在同一行
```

更可靠的表达是让每个独立热点拥有自己的对齐对象，并用实测确认：

```cpp
struct alignas(64) Counter {
    std::atomic<std::uint64_t> value{0};
};

Counter counters[2];
```

C++17 提供 `std::hardware_destructive_interference_size` 作为实现给出的参考值，但可移植代码仍要处理该常量不可用或平台拓扑更复杂的情况。

# I/O 路径：轮询、中断与 DMA

## 设备如何进入系统

现代系统不一定存在所有部件共享的一条物理“总线”。CPU、内存控制器、PCIe root complex、设备控制器和片上网络通过分层、点对点或交换式互连通信。地址、数据和控制仍是有用的逻辑分类，但不要把老式三总线图当成唯一物理实现。

吞吐上限可从“每次传输有效位数 × transfer rate × lanes/channels × 编码效率”推导；协议代际、通道数和方向必须明确。产品表中的 Gbit/s、GB/s、单向、双向和 aggregate 不可混写。

## Polling 与 interrupt 是调度选择

**轮询（polling）**持续检查状态：延迟可控、实现简单，但空闲时浪费 CPU。**中断（interrupt）**让设备主动通知 CPU：低负载效率高，但每次通知涉及上下文、缓存和调度开销。

高速网络与存储常使用混合策略：中断通知“有一批工作”，随后驱动在预算内轮询队列、批量处理并做 interrupt coalescing。不能简单写成“键盘用中断、磁盘用 DMA”两张互斥表。

## DMA 搬数据，CPU 仍负责控制

DMA 表示设备或 DMA engine 能在设备与内存之间传输数据，而不让 CPU 逐字节复制。典型流程：

```text
CPU/driver allocate and map buffers
        │
        ├─ build descriptors, program device queue
        ▼
device performs DMA reads/writes
        │
        ├─ completion entry / interrupt
        ▼
driver validates result, synchronizes ownership, unmaps or reuses buffer
```

重要边界：

- 设备看到的是 DMA address，不保证等同于 CPU virtual/physical address。
- IOMMU 可提供地址转换、隔离和 scatter/gather 支持。
- 非 coherent 平台需要显式 cache maintenance；即使 coherent，也仍需遵守 ownership、ordering 和 DMA API。
- DMA 不代表零 CPU 成本：队列管理、中断/轮询、映射、错误处理仍由软件承担。

## MMIO 与顺序

Memory-mapped I/O 把设备寄存器暴露在处理器地址空间的特殊窗口中，CPU 用 load/store 形式访问；这些区域通常具有不同于普通 RAM 的缓存和排序属性。

驱动不能只靠普通 C++ 指针或 `volatile` 推导跨设备顺序。正确方式是使用操作系统提供的 MMIO accessor、DMA API 和 memory barrier；它们封装架构、编译器和设备要求。

# 页面缓存与多层缓存叠加

一次文件读取可能同时经过：

```text
application buffer
    ↕ copy / mapping
kernel page cache
    ↕ block layer and device queues
controller / device cache
    ↕
persistent media
```

CPU cache、TLB、page cache、数据库 buffer pool 和设备 cache 缓存的是不同对象，具有不同一致性和持久化边界。`write()` 返回、数据进入 page cache、设备确认写入以及介质真正持久化不是同一时刻；需要根据故障模型使用 `fsync`、barrier/FUA 或数据库 WAL 等机制。

# 实验与掌握标准

## 顺序访问、跨步访问和指针追逐

写三个 benchmark，并保证编译器不能删除结果：

1. 连续扫描大数组；
2. 以不同 stride 扫描同一数组；
3. 随机排列下标后做依赖式 pointer chasing。

```bash
g++ -O2 -march=native benchmark.cpp -o benchmark
perf stat -r 5 -e cycles,instructions,cache-references,cache-misses ./benchmark
```

记录 CPU 型号、编译器、参数、数据规模、线程绑定和重复次数。不要把一次运行的纳秒数字抄成通用结论。

## False sharing 对照

让两个固定在线程不同核心上的线程分别递增：

- 同一 cache line 内的两个 atomic；
- 两个独立对齐的 `Counter`。

比较吞吐并观察结果是否随核心位置、迭代次数和编译参数变化。

## I/O 观察

```bash
strace -c ./reader large-file
iostat -xz 1
```

区分应用发起的系统调用次数、page cache 命中与真实设备 I/O。需要绕过 page cache 的实验必须说明对齐、块大小与平台限制，不能把 `O_DIRECT` 当成默认更快。

> [!warning] 常见误区
> - 背固定延迟表，却不记录硬件、频率、NUMA、工作集和测量方法。
> - 用结构体整体 `alignas(64)`，却让两个热点成员仍留在同一缓存行。
> - 把 cache coherence 当作语言级线程安全。
> - 认为 DMA 绕过 CPU 就同时绕过 IOMMU、缓存一致性和驱动管理。
> - 把 `write()` 成功等同于数据已经持久化到介质。

# 资料与后续

- [Intel 64 and IA-32 Optimization Reference Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel64-and-ia32-architectures-optimization.html)
- [Linux kernel: Dynamic DMA mapping](https://kernel.org/doc/html/next/core-api/dma-api.html)
- [Linux kernel memory barriers](https://kernel.org/doc/html/latest/core-api/wrappers/memory-barriers.html)
- 下一步：[Operating System Overview and Boot (操作系统总览与启动)](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/05-Operating%20System%20Overview%20and%20Boot%20(操作系统总览与启动).md)
