---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Memory Safety Tools (内存安全工具)

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

## Valgrind Memory Leaks (Valgrind 内存泄漏)

> [!note] 本节重点心考点：Memcheck 的错误类型、如何读报告、抑制误报

## Valgrind 是什么

Valgrind 是一个动态分析框架，其最常用工具 **Memcheck** 能在程序运行时检测：

- 内存泄漏（Memory Leak）
- 使用未初始化的内存
- 越界访问（堆缓冲区）
- 使用已释放的内存（use-after-free）
- 重复释放（double free）

> 代价：程序运行速度降低 10~30 倍，仅用于调试阶段。

---

## 基本用法

```bash
g++ -g -O0 -o myapp main.cpp

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind_report.txt \
         ./myapp arg1
```

|选项|含义|
|---|---|
|`--leak-check=full`|详细报告每处泄漏的调用栈|
|`--show-leak-kinds=all`|显示所有类型的泄漏（含间接泄漏）|
|`--track-origins=yes`|追踪未初始化内存的来源（较慢）|
|`--error-exitcode=1`|有错误时返回非零退出码（CI 用）|

---

## 报告解读

### 内存泄漏

```
==1234== LEAK SUMMARY:
==1234==    definitely lost: 40 bytes in 1 blocks    ← 直接泄漏（最严重）
==1234==    indirectly lost: 24 bytes in 1 blocks    ← 间接泄漏（被泄漏对象持有的内存）
==1234==      possibly lost: 0 bytes in 0 blocks     ← 可能泄漏（有指针但不在起始位置）
==1234==    still reachable: 1,024 bytes in 3 blocks ← 程序退出时仍可达（通常是全局对象，可接受）

==1234== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==1234==    at 0x4C2FB0F: malloc (vg_replace_malloc.c:309)
==1234==    by 0x10879B: createNode (node.cpp:8)      ← 在这里分配
==1234==    by 0x108812: main (main.cpp:15)            ← main 调用了 createNode
```

### 使用未初始化内存

```
==1234== Conditional jump or move depends on uninitialised value(s)
==1234==    at 0x108756: process (main.cpp:23)
==1234==  Uninitialised value was created by a stack allocation
==1234==    at 0x108700: main (main.cpp:10)
```

### Use-After-Free

```
==1234== Invalid read of size 4
==1234==    at 0x108756: process (main.cpp:30)
==1234==  Address 0x5204e80 is 0 bytes inside a block of size 4 free'd
==1234==    at 0x4C30D3B: free (vg_replace_malloc.c:530)
==1234==    by 0x108730: cleanup (main.cpp:20)
```

---

## 抑制误报（Suppression）

某些第三方库或系统库会触发 Valgrind 警告，可通过抑制文件忽略：

bash

```bash
valgrind --gen-suppressions=all ./myapp   # 生成抑制规则
valgrind --suppressions=my.supp ./myapp  # 使用抑制文件
```

---

## Valgrind 其他工具

|工具|用途|启动方式|
|---|---|---|
|Memcheck|内存错误检测（默认）|`valgrind`|
|Callgrind|CPU 性能分析，生成调用图|`valgrind --tool=callgrind`|
|Cachegrind|缓存命中率分析|`valgrind --tool=cachegrind`|
|Helgrind|线程竞争条件检测|`valgrind --tool=helgrind`|
|Massif|堆内存使用量分析|`valgrind --tool=massif`|

---

## 关联笔记

- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Core Dump Analysis (核心转储分析)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

## AddressSanitizer and UBSan (Sanitizer 工具)

> [!note] 本节重点心考点：ASan/UBSan 的检测能力、与 Valgrind 的对比、如何开启

> [!warning] Sanitizer 是测试工具，不是正确性证明
> 它只能覆盖实际执行到的路径，且编译器、平台、标准库和启用的子选项都会影响效果。将 Sanitizer 构建作为测试配置运行，发现报告先定位根因，不要仅压制错误。

## AddressSanitizer（ASan）

AddressSanitizer（ASan）通过编译器插桩检测常见内存错误，通常比 Valgrind Memcheck 更适合日常开发与 CI；具体性能开销取决于程序、平台和编译器，应以本机测量为准（NEEDS_VERIFY）。

```bash
g++ -fsanitize=address -fno-omit-frame-pointer -g -O1 -o myapp main.cpp
./myapp
```

### 能检测的问题

|错误类型|说明|
|---|---|
|Heap buffer overflow|堆上越界读写|
|Stack buffer overflow|栈上越界读写|
|Global buffer overflow|全局变量越界|
|Use-after-free|访问已释放内存|
|Use-after-return|返回局部变量地址后使用|
|Use-after-scope|超出变量作用域后使用|
|Double free|重复释放|
|Memory leaks|内存泄漏（需加 `LeakSanitizer`）|

### 报告示例

```
==1234==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000014
READ of size 4 at 0x602000000014 thread T0
    #0 0x401234 in process /home/user/main.cpp:25
    #1 0x401456 in main /home/user/main.cpp:42

0x602000000014 is located 4 bytes to the right of 16-byte region
[0x602000000000,0x602000000010) allocated by thread T0 here:
    #0 0x7f... in operator new[](unsigned long)
    #1 0x401200 in main /home/user/main.cpp:18
```

---

## UndefinedBehaviorSanitizer（UBSan）

检测 C++ 中的**未定义行为**：

```bash
g++ -fsanitize=undefined -g -o myapp main.cpp
```

### 能检测的未定义行为

```bash
-fsanitize=null             # 空指针解引用
-fsanitize=bounds           # 数组越界
-fsanitize=alignment        # 内存对齐错误
-fsanitize=shift            # 移位操作越界
-fsanitize=signed-integer-overflow  # 有符号整数溢出
-fsanitize=float-divide-by-zero     # 浮点除零
-fsanitize=vptr             # 虚函数表指针错误（检测错误的多态调用）
```

报告示例：

```
main.cpp:15:5: runtime error: signed integer overflow: 2147483647 + 1
  cannot be represented in type 'int'
```

---

## 组合使用（推荐）

```bash
g++ -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    -g -O1 \
    -o myapp main.cpp
```

---

## ASan vs Valgrind

| |ASan|Valgrind Memcheck|
|---|---|---|
|性能损耗|因程序/平台而异，需本机测量|因程序/平台而异，通常更重|
|检测方式|编译时插桩|运行时模拟|
|需要重新编译|是|否|
|栈越界检测|✅|❌|
|未初始化内存|❌（用 MSan）|✅|
|适用场景|开发/CI 日常检测|无源码时、精细分析|

> **MemorySanitizer（MSan）**：检测使用未初始化内存，需要所有依赖库都用 MSan 编译，使用门槛较高。

---

## ThreadSanitizer（TSan）

检测多线程的数据竞争（Data Race）：

```bash
g++ -fsanitize=thread -g -o myapp main.cpp
```

```
WARNING: ThreadSanitizer: data race (pid=1234)
  Write of size 4 at 0x... by thread T2:
    #0 increment() main.cpp:10

  Previous read of size 4 at 0x... by thread T1:
    #0 read_value() main.cpp:5
```

> ASan 和 TSan **不能同时使用**（会冲突），需要分开跑。

## 30 秒回答

ASan 主要发现越界、use-after-free 等内存错误，UBSan 发现部分未定义行为，TSan 发现数据竞争。它们通过不同的插桩和运行时工作，通常应拆成独立测试配置；报告是否出现取决于测试是否真正走到问题路径，不能代替单元测试、代码审查或性能测量。

## 自测

1. 为什么 ASan 与 TSan 通常要分开构建和运行？
2. “Sanitizer 没报错”为什么不能证明程序没有内存或并发问题？
3. 遇到 sanitizer report 时，为什么应先做最小复现而不是关掉检查？

## Sources

- [Clang Sanitizers documentation](https://clang.llvm.org/docs/index.html)
- [GCC instrumentation options](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html)
- 验证日期：2026-09-05

---

## 关联笔记 · 延伸要点 2
- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Core Dump Analysis (核心转储分析)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)



## 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

## 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Memory Safety Tools (内存安全工具)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
