---
tags:
  - linux/syscall
status: 🌱
---

> [!important] **核心考点**：strace 追踪系统调用、gdb 断点/堆栈/内存调试、perf 性能采样分析

## strace：系统调用追踪

strace 拦截和记录进程发出的所有系统调用及其返回值，是排查"程序为什么不工作"的第一工具。

```bash
# 跟踪一个命令
strace ls -l

# 跟踪已运行的进程
strace -p 1234

# 统计系统调用耗时（性能分析）
strace -c ls -l
# 输出：
# % time  seconds  usecs/call  calls  syscall
# 45.23  0.002134     12.34    173  write
# 32.11  0.001512      8.91    170  mmap

# 只跟踪特定系统调用
strace -e trace=open,read,write ./prog

# 显示文件描述符路径
strace -e trace=openat -f -p 1234

# 输出到文件（避免干扰终端）
strace -o trace.log ./prog
```

**常用场景：**
- 程序启动时"找不到文件" → `strace -e trace=openat,stat ./prog`
- 程序卡住不动 → `strace -p <pid>` 看停在哪个系统调用
- 性能瓶颈 → `strace -c ./prog` 统计系统调用耗时分布

## gdb：交互式调试器

gdb 用于分析崩溃（coredump）、检查运行时状态、单步执行。

```bash
gdb ./prog             # 启动调试
gdb ./prog core        # 分析 coredump
gdb -p 1234            # 附加到运行进程
```

**核心命令速查：**

| 类别 | 命令 | 作用 |
|------|------|------|
| 断点 | `break main` / `b 42` | 在函数/行号设断点 |
| 断点 | `watch x` | 变量 x 变化时中断 |
| 断点 | `info breakpoints` | 查看所有断点 |
| 运行 | `run` / `r` | 运行程序 |
| 运行 | `continue` / `c` | 继续执行 |
| 单步 | `step` / `s` | 进入函数 |
| 单步 | `next` / `n` | 跳过函数 |
| 单步 | `finish` | 执行到函数返回 |
| 查看 | `print x` / `p x` | 打印变量值 |
| 查看 | `bt` / `backtrace` | 查看调用堆栈 |
| 查看 | `frame n` / `f n` | 切换到栈帧 n |
| 查看 | `info locals` | 查看局部变量 |
| 内存 | `x/10gx addr` | 以 16 进制查看 10 个 8 字节 |
| 内存 | `x/s addr` | 查看字符串 |
| 线程 | `info threads` | 查看所有线程 |
| 线程 | `thread n` | 切换到线程 n |

**调试崩溃流程：**
```
$ ulimit -c unlimited          # 开启 coredump
$ ./prog
Segmentation fault (core dumped)
$ gdb ./prog core
(gdb) bt                       # 查看崩溃时的堆栈
(gdb) frame 3                  # 跳到可疑帧
(gdb) info locals              # 看局部变量
(gdb) p ptr                    # 检查指针是否合法
```

**条件断点示例：**
```gdb
break file.c:42 if i > 100     # i > 100 时中断
```

## perf：性能采样分析

perf 是 Linux 的性能分析器，基于硬件性能计数器和内核采样。

```bash
# 采样运行程序的性能数据
perf stat ./prog               # 统计周期数、指令数、缓存未命中
perf record ./prog             # 采样并记录
perf report                    # 查看采样报告（热点函数）

# 采样运行进程
perf top -p 1234               # 实时查看热点
perf record -p 1234 -g sleep 10  # 采样 10 秒，带调用栈

# 高级分析
perf record -e cache-misses ./prog   # 专门分析缓存未命中
perf stat -e cycles,instructions,branch-misses ./prog
```

**perf stat 输出解读：**
```
Performance counter stats for './prog':
   1,234,567,890      cycles                    #  2.5 GHz
     987,654,321      instructions              #  0.80 insn per cycle
      12,345,678      cache-misses              #  3.4% of cache refs
       1,234,567      branch-misses             #  2.1% of branches

关键指标：
- instructions per cycle (IPC) < 1 → 可能内存瓶颈
- cache-misses 比例高 → 优化数据局部性
- branch-misses 比例高 → 优化分支预测
```

## 实战排查流程

```
问题：服务器 CPU 100%

1. top 找到高 CPU 进程 PID
2. perf top -p PID          → 看热点函数
3. perf record -p PID -g    → 采样调用栈
4. perf report              → 分析代码路径
5. strace -c -p PID         → 检查系统调用频率
6. 如果热点在用户态 → 优化算法/数据结构
   如果热点在内核态 → 减少系统调用/优化 IO 模式
```

```
问题：程序崩溃/异常

1. ulimit -c unlimited && ./prog    → 复现
2. gdb ./prog core                  → 加载 coredump
3. bt                               → 看崩溃堆栈
4. frame N → info locals            → 检查上下文
5. p variable                       → 看具体值
6. 如果指针非法 → 检查空指针/野指针
   如果栈损坏 → 检查缓冲区溢出
   如果断言失败 → 检查前置条件
```

> [!tip]- **工程要点**：三把刀各有所长——strace 看"程序在做什么系统调用"，gdb 看"程序内部状态是什么"，perf 看"程序时间花在哪"。排查问题时先从 strace/perf 宏观定位，再到 gdb 微观确认。**不要上来就用 gdb 单步调试几万行的服务器程序。**

调试与追踪工具见 → [System Calls Overview (常用系统调用速查)](/04-Linux%20&%20System%20(Linux%20系统)/04%20·%20系统调用与工具/10-System%20Calls%20Overview%20(常用系统调用速查).md) · [Dynamic Library & Shared Object (动态库原理)](/04-Linux%20&%20System%20(Linux%20系统)/04%20·%20系统调用与工具/11-Dynamic%20Library%20&%20Shared%20Object%20(动态库原理)%20⭐.md)
