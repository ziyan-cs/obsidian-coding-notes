---
tags:
  - devtools/debug
status: 🌱
---


> **核心考点**：core dump 的产生条件、如何开启、如何用 GDB 分析崩溃现场

## 什么是 Core Dump

程序因信号（SIGSEGV、SIGABRT 等）异常崩溃时，操作系统将**进程的内存快照**转储到文件（core 文件）。通过 GDB 加载 core 文件，可以还原崩溃瞬间的完整现场：调用栈、变量值、内存状态。

---

## 开启 Core Dump

```bash
# 查看当前 core 文件大小限制（0 表示禁用）
ulimit -c

# 临时开启（当前 shell 有效）
ulimit -c unlimited

# 永久开启（写入 /etc/security/limits.conf）
* soft core unlimited
* hard core unlimited
```

### 设置 core 文件路径和命名

```bash
# /proc/sys/kernel/core_pattern 控制 core 文件的位置和命名
echo '/var/cores/core.%e.%p.%t' > /proc/sys/kernel/core_pattern
# %e = 可执行文件名, %p = PID, %t = 时间戳, %s = 导致崩溃的信号编号
```

---

## 用 GDB 分析 Core Dump

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

## 常见崩溃信号与原因

|信号|常见原因|
|---|---|
|SIGSEGV|空指针解引用、野指针、栈溢出、非法内存访问|
|SIGABRT|`assert()` 失败、`abort()` 调用、double free 被检测到|
|SIGFPE|整数除零、浮点异常|
|SIGBUS|内存对齐错误、mmap 文件被截断后访问|
|SIGILL|执行非法指令（通常是严重的内存踩踏）|

---

## 分析案例：空指针崩溃

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

## 主动触发 Core Dump（调试技巧）

```bash
# 向进程发送 SIGABRT，强制产生 core
kill -SIGABRT <pid>

# 在代码中主动 dump（不退出进程）
#include <sys/resource.h>
void enable_core_dump() {
    struct rlimit rl = {RLIM_INFINITY, RLIM_INFINITY};
    setrlimit(RLIMIT_CORE, &rl);
}
```

---

## 关联笔记

- [GDB Essentials：breakpoint, watch, backtrace (GDB核心用法)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)
- [Valgrind：Memory Leak Detection (内存泄漏检测)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03c-Valgrind：Memory%20Leak%20Detection%20(内存泄漏检测)%20⭐.md)
- [AddressSanitizer & UBSan (编译期检测工具)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03d-AddressSanitizer%20&%20UBSan%20(编译期检测工具)%20⭐.md)
- [perf：CPU Profiling & Flamegraph (性能火焰图)](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03e-perf：CPU%20Profiling%20&%20Flamegraph%20(性能火焰图).md)
- [Core Concepts：Working Tree, Index, HEAD (三区模型)](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)
