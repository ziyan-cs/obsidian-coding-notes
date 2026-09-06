---
tags:
  - cpp/engineering
status: 🌱
---

# Debugging gdb & Sanitizers — 调试工具

> [!important] **核心考点**：GDB 核心命令、AddressSanitizer 使用、Segment Fault 调试、Core Dump 分析

## GDB 核心命令

```bash
# 编译时加 -g 开启调试符号
g++ -g -O0 main.cpp -o main

# 启动 GDB
gdb ./main
gdb ./main core        # 分析 core dump
gdb ./main 1234        # 附加到进程 1234
```

### 常用命令速查

| 命令 | 缩写 | 作用 |
|------|------|------|
| `break main` | `b main` | 在 main 函数设断点 |
| `break file.cpp:42` | `b file.cpp:42` | 在文件某行设断点 |
| `run` | `r` | 运行程序 |
| `next` | `n` | 单步跳过（不进入函数）|
| `step` | `s` | 单步进入 |
| `finish` | `fin` | 运行到当前函数返回 |
| `continue` | `c` | 继续运行到下一个断点 |
| `print var` | `p var` | 打印变量值 |
| `backtrace` | `bt` | 查看调用栈 |
| `frame N` | `f N` | 切换到第 N 帧 |
| `info locals` | `i lo` | 查看当前帧局部变量 |
| `list` | `l` | 显示源代码 |
| `watch expr` | — | 监视表达式变化 |
| `display expr` | — | 每次停顿时自动打印 |

```gdb
# 常用组合
gdb> b main               # 断点
gdb> r arg1 arg2          # 带参数运行
gdb> bt                   # 崩溃时先看调用栈
gdb> f 3                  # 切换到怀疑的帧
gdb> p variable           # 查看变量
gdb> l                    # 看附近源码
```

## AddressSanitizer (ASan)

```bash
# 编译时开启（具体支持组合以当前 GCC/Clang 文档为准）
g++ -fsanitize=address -g -O1 main.cpp -o main

# 运行即可检测内存错误
./main
# 出现错误时输出详细的调用栈和内存分配信息
```

### ASan 检测的问题类型

```cpp
int* p = new int[10];

p[10] = 42;     // ✅ ASan 检测：heap-buffer-overflow
delete[] p;
*p = 1;         // ✅ ASan 检测：heap-use-after-free

int* q = (int*)malloc(4);
free(q);
free(q);        // ✅ ASan 检测：double-free

int a;
int* r = &a;
delete r;       // ✅ ASan 检测：delete on stack variable
```

## 其他 Sanitizers

```bash
# 未定义行为检测（UB Sanitizer）
g++ -fsanitize=undefined -g main.cpp -o main

# 线程错误检测（Thread Sanitizer）
g++ -fsanitize=thread -g -O1 main.cpp -o main

# 内存泄漏检测（Leak Sanitizer）
g++ -fsanitize=leak -g main.cpp -o main

# 全部开启（调试用）
g++ -fsanitize=address,undefined -g -O1 main.cpp -o main
```

```cpp
// UBSan 检测的典型 UB：
int x = INT_MAX;
x + 1;            // signed overflow
int* p = nullptr;
*p = 42;          // null dereference
int a = 0;
int b = 1 / a;    // division by zero
```

## Core Dump 分析

```bash
# 开启 core dump
ulimit -c unlimited
echo "core.%p" > /proc/sys/kernel/core_pattern

# 运行程序产生 crash 后会生成 core 文件
gdb ./main core.1234

# 在 GDB 中：
# bt      — 查看崩溃时的调用栈
# frame N — 切换到怀疑的帧
# info locals — 查看局部变量
# print *this — 查看当前对象
```

## Valgrind 基础

```bash
# Memcheck：检测内存错误
valgrind --tool=memcheck ./main

# Callgrind：性能分析
valgrind --tool=callgrind ./main

# 性能开销受程序、平台和工具版本影响；先在本机测量
# ASan 与 Valgrind 覆盖面不同，按复现条件选择，不存在“上线前必跑”的通用顺序
```

| 工具 | 用途 | 速度 |
|------|------|------|
| **AddressSanitizer** | 内存错误检测 | 开销因程序/平台而异，适合日常测试 |
| **Valgrind Memcheck** | 指令级内存检查 | 通常开销更高，适合针对性复现 |
| **ThreadSanitizer** | 数据竞争检测 | 开销因程序/平台而异，需独立测试配置 |
| **UBSan** | 部分未定义行为检测 | 开销因启用检查项而异 |

> [!tip]- **工程要点**：现代 C++ 调试首选 **AddressSanitizer**（快、准）。在 CI 中应开启 ASan + UBSan。与 GDB 配合使用：ASan 告诉你问题类型和位置，GDB 帮你分析上下文。

> [!tip]- **工程要点**：把 Sanitizer 作为可重复的测试配置，而不是“跑过一次就安全”。ASan/UBSan 常适合日常 CI；TSan 通常独立运行；GDB 用于观察真实崩溃现场。具体组合以项目平台、依赖与测试时长为准。

---

性能分析工具详见 → [Performance Profiling perf & valgrind (性能分析)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/06-Performance%20Profiling%20perf%20&%20valgrind%20(性能分析)%20⭐.md)
