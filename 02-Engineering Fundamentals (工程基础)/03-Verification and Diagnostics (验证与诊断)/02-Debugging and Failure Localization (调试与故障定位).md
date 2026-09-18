---
status: stable
confidence: high
content_verified: 2026-09-18
tags: [engineering/verification]
---

> [!abstract] 学习定位
> 调试不是随机改代码，而是把“现象”逐步收敛为可证伪的根因假设。本篇统一现场保护、复现、二分、动态调试和 core dump 分析；具体语言的异常模型留在语言专题。

# 一条可靠的故障定位链

```text
确认影响 -> 保存现场 -> 建立最小复现 -> 提出假设
        -> 设计区分实验 -> 找到首次错误状态 -> 修复并回归
```

先区分 symptom 与 root cause。超时可能来自连接池耗尽，连接池耗尽可能来自请求未取消，而未取消又可能来自错误的生命周期管理。只在超时处加大阈值通常是在隐藏症状。

## 先写故障事实

记录首次发生时间、影响范围、输入、版本、配置、环境和稳定复现步骤。事实与推断分栏：

| 类型 | 示例 |
|---|---|
| 事实 | build `abc123` 的 `/orders` P99 从 80 ms 升到 2 s |
| 推断 | 可能是数据库连接池饱和 |
| 实验 | 对比连接等待时间、活跃连接、慢查询和上一版本 |

一次实验只改变一个主要变量。若修改后“似乎好了”，仍需解释为什么、在什么边界下成立，以及怎样自动回归。

# 复现与最小化

复现包至少包含二进制或 revision、依赖版本、配置、输入、随机种子、时间条件和执行命令。最小化不是把系统删到最短，而是保留仍能触发问题的最小因果集合。

高效缩小范围的方法：

- 时间二分：用 `git bisect` 思维寻找首次坏版本，但不把相关提交直接当根因；
- 输入二分：缩小触发数据、请求序列或并发度；
- 组件隔离：用可控替代品判断问题在调用方、网络还是依赖方；
- 不变量断言：在状态首次偏离的位置失败，而不是等到末端崩溃。

Heisenbug 会被日志、断点或优化级别改变。此时优先使用采样、硬件 watchpoint、record/replay、core dump 和低扰动追踪。

## GDB 的问题驱动用法

使用带调试信息的构建；优化构建中的变量可能被消除或重排，必要时保留与线上完全匹配的符号文件。

```bash
gdb --args ./server --config test.yaml
gdb -p <pid>
gdb ./server core.<pid>
```

| 问题 | 命令 |
|---|---|
| 程序停在哪里 | `bt`、`frame N`、`list` |
| 参数与局部状态是什么 | `info args`、`info locals`、`p expr` |
| 谁改坏了值 | `watch expr`、`rwatch expr` |
| 是否所有线程都在等待 | `info threads`、`thread apply all bt` |
| 原始内存/寄存器是什么 | `x/16gx addr`、`info registers` |
| 源码与指令是否一致 | `disassemble /m`、`info line` |

条件断点和 watchpoint 能降低噪声：

```gdb
break handle_request if request_id == 42
watch state
commands
  silent
  bt 4
  continue
end
```

不要在不理解副作用时用 GDB 调用程序函数；它可能加锁、分配内存或改变现场。

## Core dump 与崩溃现场

core dump 保存崩溃时进程地址空间的可用快照。分析结果依赖四件套一致：可执行文件、debug symbols、共享库和源码 revision。

Linux 上先检查：

```bash
ulimit -c
cat /proc/sys/kernel/core_pattern
coredumpctl list
coredumpctl debug <PID-or-executable>
```

进入 GDB 后按顺序检查：

```gdb
bt full
thread apply all bt full
frame 0
info args
info locals
info registers
```

常见信号只是入口，不是结论：`SIGSEGV` 可能源于越界、use-after-free 或栈破坏；`SIGABRT` 可能是断言、显式 `abort` 或 allocator 检测到堆损坏；`SIGBUS` 可能来自对齐问题或访问被截断的 mmap 文件。

栈已损坏时，回溯可能不可信。结合 fault address、寄存器、反汇编、线程栈和 sanitizer 复现；不要只盯着最顶层帧。

# 卡死、死锁与超时

卡死程序先抓取所有线程栈并重复采样：若相同线程长期停在相同等待点，检查锁顺序、条件变量谓词、I/O 超时和线程池饥饿。

```bash
gdb -p <pid> -batch -ex 'thread apply all bt full'
strace -ff -tt -T -p <pid>
```

`strace` 适合回答进程是否阻塞在系统调用、返回码和等待时间；它不能直接解释用户态 CPU 热点。网络问题还需 socket 状态、抓包和服务端证据交叉验证。

# 修复与回归判据

- 最小复现从稳定失败变为稳定通过；
- 新测试能在旧实现上失败，在修复后通过；
- 修复解释了全部关键证据，没有只掩盖报警；
- 相关失败模式、并发条件和资源清理已回归；
- 线上验证定义了观测窗口与回滚条件。

# 实践与资料

- [ ] 制造一次空指针崩溃，保存符号和 core，独立还原调用链。
- [ ] 制造锁顺序反转，用所有线程栈画出 wait-for 关系。
- [ ] 对一个随机失败用例固定 seed，最小化输入并加入回归集。
- [ ] 为一次真实故障写“事实—假设—实验—结论”记录。

## 关联专题

- [Memory Safety and Dynamic Analysis (内存安全与动态分析)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/03-Memory%20Safety%20and%20Dynamic%20Analysis%20(内存安全与动态分析).md)：用 sanitizer 和动态分析验证内存/并发假设。
- [Observability Logs Metrics and Tracing (可观测性、日志、指标与追踪)](/02-Engineering%20Fundamentals%20(工程基础)/03-Verification%20and%20Diagnostics%20(验证与诊断)/05-Observability%20Logs%20Metrics%20and%20Tracing%20(可观测性、日志、指标与追踪).md)：从线上信号定位故障入口。

## 参考资料

- [GDB Documentation](https://sourceware.org/gdb/documentation/)
- [systemd-coredump](https://www.freedesktop.org/software/systemd/man/latest/systemd-coredump.html)
- [strace manual](https://man7.org/linux/man-pages/man1/strace.1.html)
