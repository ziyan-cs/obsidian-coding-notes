---
tags:
  - devtools
  - debug
---


> **核心考点**：perf 采样原理、常用子命令、如何生成火焰图并解读

## perf 基础

`perf` 是 Linux 内核提供的性能分析工具，通过**采样**方式统计程序在各函数上花费的时间，几乎无性能损耗（采样频率通常 99Hz）。

```bash
# 安装
sudo apt install linux-perf    # Ubuntu
sudo yum install perf          # CentOS

# 常用子命令
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
# 采样记录（-g 记录调用栈，--call-graph dwarf 更准确）
perf record -g --call-graph dwarf -o perf.data ./myapp

# 交互式分析
perf report -i perf.data
# 按 Enter 展开函数，查看调用者/被调用者
# 按 a 进入 annotate 模式，看到热点汇编行
```

---

## 生成火焰图（Flamegraph）

Brendan Gregg 的火焰图是可视化性能热点的最佳工具：

```bash
# 1. 克隆火焰图工具
git clone https://github.com/brendangregg/FlameGraph.git

# 2. 采样（频率 99Hz，持续 30 秒）
perf record -F 99 -g -p <pid> -- sleep 30
# 或直接运行程序
perf record -F 99 -g ./myapp

# 3. 生成火焰图
perf script | ./FlameGraph/stackcollapse-perf.pl | \
    ./FlameGraph/flamegraph.pl > flamegraph.svg

# 4. 浏览器打开 SVG（可交互点击缩放）
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