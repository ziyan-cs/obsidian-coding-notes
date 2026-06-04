---
tags:
  - cs
  - fundamentals
---

> **核心考点**：延迟 vs 吞吐量、Amdahl 定律、CPU 性能公式、性能度量指标

## 延迟 vs 吞吐量

| 概念 | 定义 | 例子 |
|------|------|------|
| **延迟** (Latency) | 完成单个任务所需时间 | 一次 HTTP 请求用时 100ms |
| **吞吐量** (Throughput) | 单位时间完成的任务数 | 每秒处理 10000 个请求 |

优化往往需权衡：增加缓存降低延迟但消耗更多内存，多线程提升吞吐但可能增加延迟。

## CPU 性能公式

```
CPU 时间 = 指令数 × CPI × 时钟周期时间
```

| 因子 | 含义 | 受谁影响 |
|------|------|---------|
| **指令数** | 程序执行的指令总数 | 编译器、ISA、算法 |
| **CPI** | 每条指令平均时钟周期数 | 微架构、数据 locality |
| **时钟周期** | 每个时钟周期的时间 | 硬件工艺、频率 |

**优化思路：** 减少任一因子即可提升性能，但通常相互牵制（如更复杂的指令集可减指令数但可能增加 CPI）。

## Amdahl 定律

```
加速比 = 1 / ((1 - P) + P/S)
```

P = 可优化部分占比，S = 该部分的加速比

**核心含义：** 即使将 90% 部分提速 100 倍，整体加速也不会超过 10 倍——**瓶颈决定最终性能**。

```cpp
// 实际意义：不要过早优化非瓶颈部分
// 花 10 小时将 10% 的部分提速 10 倍 → 整体仅加速 1.1 倍
// 花 1 小时将 90% 的部分提速 2 倍 → 整体加速 1.8 倍
```

## 常用性能指标

| 指标 | 含义 | 场景 |
|------|------|------|
| QPS / TPS | 每秒查询/事务数 | 服务器吞吐量 |
| P99 延迟 | 99% 请求在此时间内完成 | 长尾延迟 |
| IPC | 每周期指令数（CPI 倒数） | CPU 效率 |
| FLOPS | 每秒浮点运算次数 | 科学计算 |
| IOPS | 每秒 I/O 操作数 | 存储性能 |

---

## 关联笔记

- [System Thinking（系统思维）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/05-System%20Thinking（系统思维⭐）.md)
- [What is a Computer（计算机本质）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/01-What%20is%20a%20Computer（计算机本质）.md)
- [Hardware vs Software（软硬件关系）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/02-Hardware%20vs%20Software（软硬件关系）.md)
- [System Abstraction（系统抽象层次）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/01-Computer%20Overview（计算机系统总览）/03-System%20Abstraction（系统抽象层次）.md)
- [Binary & Encoding（二进制与编码）](/01-CS%20Core%20(计算机核心基础)/01-Computer%20Fundamentals（计算机基础）/02-Data%20Representation（数据表示）/01-Binary%20&%20Encoding（二进制与编码）.md)
