---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

> [!summary]- 复述检查：学完后再展开
>
> **回答**：性能分析先定义指标和负载，再用 CPU、内存、锁和 I/O profile 找热点。每次只验证一个假设，并比较修改前后的相同条件；采样器看到相关性，不自动证明因果。

# perf and Flamegraphs (perf 与火焰图)

> [!note] 本节重点：perf 采样原理、常用子命令、如何生成火焰图并解读

# perf 基础

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

# perf stat 输出解读

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

# perf record & report

```bash
perf record -g --call-graph dwarf -o perf.data ./myapp

perf report -i perf.data
```

---

# 生成火焰图（Flamegraph）

Brendan Gregg 的火焰图是可视化性能热点的最佳工具：

```bash
git clone https://github.com/brendangregg/FlameGraph.git

perf record -F 99 -g -p <pid> -- sleep 30
perf record -F 99 -g ./myapp

perf script | ./FlameGraph/stackcollapse-perf.pl | \
    ./FlameGraph/flamegraph.pl > flamegraph.svg

```

---

# 如何读火焰图

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

> [!info]- 延伸阅读
> - GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)
> - Core Dump Analysis (核心转储分析)
> - Valgrind：Memory Leak Detection (内存泄漏检测)
> - AddressSanitizer & UBSan (编译期检测工具)
> - Core Concepts：Working Tree, Index, HEAD (三区模型)
