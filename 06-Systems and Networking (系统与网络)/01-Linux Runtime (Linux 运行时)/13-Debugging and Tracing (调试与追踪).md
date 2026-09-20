---
study_stage: backlog
---

> [!note] 方法论坐标
> 通用定位链见 [Debugging and Failure Localization (调试与故障定位)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/02-Debugging%20and%20Failure%20Localization%20(调试与故障定位).md)，性能实验见 [Performance Benchmarking and Profiling (性能基准与剖析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/04-Performance%20Benchmarking%20and%20Profiling%20(性能基准与剖析).md)；本篇保留 strace、GDB、perf 等 Linux 现场工具。

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 调试先稳定复现并缩小范围，再分别观察系统调用、进程状态、网络和性能热点。strace、gdb、perf 等工具回答不同问题，证据链应从现象连接到具体代码路径。


> [!note] 本节重点：strace 追踪系统调用、gdb 断点/堆栈/内存调试、perf 性能采样分析

# strace：系统调用追踪

`strace` 可跟踪指定进程/线程的系统调用及返回值，适合回答“卡在哪个调用、返回了什么 errno”；`-f` 才会跟踪新创建的子进程/线程。附加追踪会改变时序并增加开销，线上先缩小时间窗和调用集合，不把它当成所有故障的第一工具。

```bash
strace ls -l

strace -p 1234

strace -c ls -l

strace -e trace=open,read,write ./prog

strace -e trace=openat -f -p 1234

strace -o trace.log ./prog
```

**常用场景：**
- 程序启动时"找不到文件" → `strace -e trace=openat,stat ./prog`
- 程序卡住不动 → `strace -p <pid>` 看停在哪个系统调用
- 系统调用疑点 → `strace -c ./prog` 看被追踪调用的计数与时间；这不是低开销的精确性能剖析，热点仍应用 `perf`/应用指标交叉验证

# gdb：交互式调试器

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
$ ulimit -c unlimited          # 允许生成 core，实际保存位置还受 core_pattern/systemd-coredump 控制
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

# perf：性能采样分析

perf 是 Linux 的性能分析器，基于硬件性能计数器和内核采样。

```bash
perf stat ./prog               # 统计周期数、指令数、缓存未命中
perf record ./prog             # 采样并记录
perf report                    # 查看采样报告（热点函数）

perf top -p 1234               # 实时查看热点
perf record -p 1234 -g sleep 10  # 采样 10 秒，带调用栈

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
- IPC、cache-miss、branch-miss 必须和同一机器、同一负载的基线比较；IPC < 1 不能单独诊断为内存瓶颈
- 先确认采样是否覆盖用户态/内核态、符号是否可解析、CPU 是否被其他进程占用，再定位具体调用路径
```

# 实战排查流程

```
问题：服务器 CPU 100%

1. top 找到高 CPU 进程 PID
2. perf top -p PID          → 看热点函数
3. perf record -p PID -g    → 采样调用栈
4. perf report              → 分析代码路径
5. 只有怀疑系统调用等待/错误时，限时使用 strace 并说明观测开销
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

> [!tip]- **工程要点**：先用日志、指标与最小复现界定故障，再选工具：`strace` 查 syscall/errno，`gdb` 查进程内部状态与 core，`perf` 查采样热点。附加调试器会暂停进程，追踪会改变时序；在生产环境先评估权限、敏感数据与性能影响。

