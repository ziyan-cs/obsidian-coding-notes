---
tags:
  - devtools/debug
status: learning
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 04-AddressSanitizer and UBSan (Sanitizer 工具)

> [!abstract] 核心考点：ASan/UBSan 的检测能力、与 Valgrind 的对比、如何开启

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
# 常用子项（不同编译器支持集合不同，先查所用编译器文档）
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

## 关联笔记

- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Core Dump Analysis (核心转储分析)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
