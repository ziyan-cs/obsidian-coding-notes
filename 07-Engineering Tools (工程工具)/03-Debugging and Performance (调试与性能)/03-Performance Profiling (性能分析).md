---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Performance Profiling (性能分析)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 30 秒回答

**03-Performance Profiling (性能分析)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## perf and Flamegraphs (perf 与火焰图)

> [!abstract] 核心考点：perf 采样原理、常用子命令、如何生成火焰图并解读

## perf 基础

`perf` 是 Linux 内核提供的性能分析工具，通过**采样**方式统计程序在各函数上花费的时间，几乎无性能损耗（采样频率通常 99Hz）。

```bash
sudo apt install linux-perf    # Ubuntu
sudo yum install perf          # CentOS

perf stat ./myapp              # 统计 CPU 事件（缓存命中、分支预测等）
perf top                       # 实时查看热点函数（类似 top）
perf record ./myapp            # 采样记录，生成 perf.data
perf report                    # 分析 perf.data，交互式查看
perf annotate                  # 在源码/汇编级别显示热点
```

---

## perf stat 输出解读

```bash
perf stat ./myapp

 Performance counter stats for './myapp':

    1,234.56 msec task-clock          # CPU 时间
           3      context-switches    # 上下文切换次数
           0      cpu-migrations      
       1,024      page-faults         # 缺页中断

   5,678,901,234  cycles              # CPU 周期
   4,123,456,789  instructions        # 执行指令数
           0.73   insn per cycle      # IPC（越高越好，理想值 >1）

   1,234,567,890  cache-references    # 缓存访问
     123,456,789  cache-misses        # 缓存未命中（10%，偏高）
```

---

## perf record & report

```bash
perf record -g --call-graph dwarf -o perf.data ./myapp

perf report -i perf.data
```

---

## 生成火焰图（Flamegraph）

Brendan Gregg 的火焰图是可视化性能热点的最佳工具：

```bash
git clone https://github.com/brendangregg/FlameGraph.git

perf record -F 99 -g -p <pid> -- sleep 30
perf record -F 99 -g ./myapp

perf script | ./FlameGraph/stackcollapse-perf.pl | \
    ./FlameGraph/flamegraph.pl > flamegraph.svg

```

---

## 如何读火焰图

```
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  main()
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   ░░░░░░░░░░░░░░░  processA()    parseB()
▓▓▓▓▓▓▓▓▓  ▒▒▒▒▒▒   ░░░░░  ░░░░░░░░  sort()  hash()  read()  decode()
```

- **X 轴**：函数在采样中出现的比例（宽 = 耗时多），**不代表时间顺序**
- **Y 轴**：调用栈深度，下面是调用者，上面是被调用者
- **顶部平坦的宽条**：性能瓶颈所在（在此处花了大量 CPU 时间且没有继续向下调用）
- **点击某个框**：缩放到该函数，查看其调用树

---

## 关联笔记

- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Core Dump Analysis (核心转储分析)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 03-Performance Profiling (性能分析) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
