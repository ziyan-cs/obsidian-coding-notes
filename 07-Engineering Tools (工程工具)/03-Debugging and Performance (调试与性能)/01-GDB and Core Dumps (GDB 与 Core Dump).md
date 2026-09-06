---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。

# 30 秒回答

**核心结论**：学习定位：把工具当成可重现的工程流程，理解配置、输入、产物、失败诊断与自动化，而不是背命令。


# GDB Essentials (GDB 核心用法)

> [!note] 本节重点：核心考点：GDB 启动方式、断点控制、变量观察、调用栈分析、多线程调试

# 启动方式

```bash
gdb ./myapp                        # 直接调试程序
gdb ./myapp core                   # 调试 core dump
gdb -p 12345                       # attach 到正在运行的进程
gdb --args ./myapp arg1 arg2       # 带参数启动
```

>编译时必须加 `-g` 保留调试信息，建议同时加 `-O0` 禁止优化：
> ```bash
> g++ -g -O0 -o myapp main.cpp
> ```

---

# 断点（Breakpoint）

```bash
b main                    # 在函数入口打断点
b src/main.cpp:42         # 在指定文件第 42 行
b MyClass::method         # 在成员函数
b main.cpp:42 if x > 10  # 条件断点（满足条件才停）

info breakpoints          # 列出所有断点（简写 i b）
disable 2                 # 禁用编号为 2 的断点
enable 2                  # 启用
delete 2                  # 删除
delete                    # 删除所有断点

tbreak main.cpp:50
```

---

# 执行控制

```bash
run                       # 启动程序（简写 r）
run arg1 arg2             # 带参数运行
continue                  # 继续运行到下一个断点（简写 c）
next                      # 单步执行，不进入函数（简写 n）
step                      # 单步执行，进入函数（简写 s）
finish                    # 执行完当前函数并返回
until 60                  # 运行到第 60 行（跳出循环用）
return                    # 强制从当前函数返回（可指定返回值）
kill                      # 终止程序
quit                      # 退出 GDB（简写 q）
```

---

# 查看变量与内存

```bash
print x                   # 打印变量 x（简写 p）
print *ptr                # 打印指针指向的值
print arr[0]@5            # 打印数组前 5 个元素
print (int)var            # 强制类型转换后打印

display x                 # 每次停下来自动打印 x
undisplay 1               # 取消 display

info locals               # 打印当前栈帧所有局部变量
info args                 # 打印当前函数参数

x/10xw 0x7fff1234        # 查看内存：10个单元，十六进制，word(4字节)
```

---

# 观察点（Watchpoint）

当变量值改变时自动停下，用于追踪"某个变量是在哪里被修改的"：

```bash
watch x                   # 当 x 的值改变时停下
watch *0x7fff1234         # 监视内存地址
rwatch x                  # 当 x 被读取时停下
awatch x                  # 当 x 被读或写时停下
info watchpoints          # 查看所有观察点
```

---

# 调用栈（Backtrace）

```bash
backtrace                 # 打印调用栈（简写 bt）
bt full                   # 打印调用栈 + 每帧局部变量
bt 5                      # 只显示最近 5 帧

frame 3                   # 切换到第 3 帧（简写 f 3）
info frame                # 显示当前帧详情
up                        # 上移一帧（调用者）
down                      # 下移一帧（被调用者）
```

---

# 多线程调试

```bash
info threads              # 列出所有线程
thread 3                  # 切换到线程 3
thread apply all bt       # 打印所有线程的调用栈（排查死锁必备）
thread apply all bt full  # 所有线程的完整调用栈 + 局部变量

set scheduler-locking on  # 只运行当前线程，其他线程冻结（单步调试用）
set scheduler-locking off # 恢复所有线程运行
```

---

# 常用 TUI 模式

```bash
gdb -tui ./myapp          # 启动带源码窗口的 TUI 模式
layout src                # 显示源码窗口
layout asm                # 显示汇编窗口
layout split              # 源码 + 汇编同时显示
Ctrl+L                    # 刷新屏幕（TUI 花屏时用）
```

---

# 关联笔记

- [Core Dump Analysis (核心转储分析)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03b-Core%20Dump%20Analysis%20(核心转储分析)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

---

# Core Dump Analysis (核心转储分析)

> [!note] 本节重点：核心考点：core dump 的产生条件、如何开启、如何用 GDB 分析崩溃现场

# 什么是 Core Dump

程序因信号（SIGSEGV、SIGABRT 等）异常崩溃时，操作系统将**进程的内存快照**转储到文件（core 文件）。通过 GDB 加载 core 文件，可以还原崩溃瞬间的完整现场：调用栈、变量值、内存状态。

---

# 开启 Core Dump

```bash
ulimit -c

ulimit -c unlimited

* soft core unlimited
* hard core unlimited
```

## 设置 core 文件路径和命名

```bash
echo '/var/cores/core.%e.%p.%t' > /proc/sys/kernel/core_pattern
```

---

# 用 GDB 分析 Core Dump

```bash
gdb ./myapp /var/cores/core.myapp.1234.1720000000
```

进入 GDB 后，程序已经停在崩溃的那一刻：

```bash
bt                        # 第一步：看调用栈，定位崩溃位置
frame N                   # 切换到崩溃帧
info locals               # 查看局部变量
print ptr                 # 检查可疑指针
info registers            # 查看寄存器（RIP = 崩溃时的指令地址）
x/s 0x...                 # 检查字符串内存
thread apply all bt       # 多线程程序看所有线程栈
```

---

# 常见崩溃信号与原因

|信号|常见原因|
|---|---|
|SIGSEGV|空指针解引用、野指针、栈溢出、非法内存访问|
|SIGABRT|`assert()` 失败、`abort()` 调用、double free 被检测到|
|SIGFPE|整数除零、浮点异常|
|SIGBUS|内存对齐错误、mmap 文件被截断后访问|
|SIGILL|执行非法指令（通常是严重的内存踩踏）|

---

# 分析案例：空指针崩溃

```
(gdb) bt
#0  0x0000000000401234 in Node::getValue (this=0x0) at node.cpp:15
#1  0x0000000000401456 in process (node=0x0) at main.cpp:42
#2  0x0000000000401678 in main () at main.cpp:87

(gdb) frame 1
(gdb) print node
$1 = (Node *) 0x0      ← 空指针！
```

结论：`process()` 接收到空指针 `node`，在第 15 行调用成员函数时崩溃。

---

# 主动触发 Core Dump（调试技巧）

```bash
kill -SIGABRT <pid>

#include <sys/resource.h>
void enable_core_dump() {
    struct rlimit rl = {RLIM_INFINITY, RLIM_INFINITY};
    setrlimit(RLIMIT_CORE, &rl);
}
```

---

# 关联笔记 · 延伸要点 2
- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/04-Engineering%20Tools%20(工程工具)/03-Debugging%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/04-Engineering%20Tools%20(工程工具)/01-Version%20Control%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)



# 零基础阅读路径

先从最短命令路径跑通一次，再回来看配置字段与高级选项。每读一段命令，都要知道它读取什么、生成什么以及怎样撤销或诊断。

# 常见误区

- 只记命令，不理解它改变了哪些输入、产物或运行环境，发生故障时无法恢复。
- 没有在临时项目中亲自执行并保留输出，就把工具流程当成已经掌握。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-GDB and Core Dumps (GDB 与 Core Dump)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
