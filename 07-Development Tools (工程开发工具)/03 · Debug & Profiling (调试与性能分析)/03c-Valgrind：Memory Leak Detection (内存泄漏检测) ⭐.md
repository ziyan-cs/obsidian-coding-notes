---
tags:
  - devtools/debug
status: 🌱
---


> [!important] **核心考点**：Memcheck 的错误类型、如何读报告、抑制误报

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

bash

```bash
# 编译（保留调试信息）
g++ -g -O0 -o myapp main.cpp

# 运行 Valgrind Memcheck
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

- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Core Dump Analysis (核心转储分析)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
