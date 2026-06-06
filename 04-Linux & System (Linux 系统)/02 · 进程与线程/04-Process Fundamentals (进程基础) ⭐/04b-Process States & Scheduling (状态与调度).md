---
tags:
  - linux
  - process
---

> **核心考点**：进程三态/五态模型、就绪/运行/阻塞状态切换、Linux 调度策略与优先级

## 进程状态

```
              fork()
CREATED ──────────────────> READY
                              │
              Schedule器选中        │  Wait I/O / Signal / Lock
              ↓               ↓
           RUNNING ─────────> WAITING/BLOCKED
              │                    │
              │ Time Slice耗尽           │ 条件full足
              ↓                    ↓
            READY <─────────────── READY
              │
              │ exit()
              ↓
           ZOMBIE ──（父进程 wait）──> 消亡
```

|状态|ps 显示|含义|
|---|---|---|
|Running|R|正在 CPU 上执行，或在运行队列中|
|Sleeping（可中断）|S|等待事件（I/O、信号），可被信号唤醒|
|Sleeping（不可中断）|D|等待内核 I/O（如磁盘），**不能被信号中断**|
|Stopped|T|被 SIGSTOP / SIGTSTP 暂停|
|Zombie|Z|已退出但父进程未 wait|

> **D 状态（不可中断睡眠）** 很危险：进程无法被 kill，通常意味着磁盘 I/O 卡住或 NFS 挂载问题，只能等待或重启。

## Linux 调度器（CFS）

Linux 默认使用 **CFS（Completely Fair Scheduler，完全公平调度器）**：

- 以**虚拟运行时间（vruntime）** 为核心指标，vruntime 最小的进程优先运行
- 用**红黑树**组织所有就绪进程，最左节点（vruntime 最小）即下一个运行的进程
- nice 值（-20 ~ 19）影响 vruntime 增长速度：nice 越低（优先级越高），vruntime 增长越慢，获得更多 CPU 时间

```bash
nice -n -10 ./myapp    # 以高优先级启动
renice 5 -p 1234       # 修改运行中进程的 nice 值
```

---

## 关联笔记

- [Process Lifecycle： fork, exec, wait (生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)
- [Zombie & Orphan Process (僵尸进程与孤儿进程)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04c-Zombie%20&%20Orphan%20Process%20(僵尸进程与孤儿进程).md)
- [Inter-Process Communication： pipe, fifo, mmap (IPC)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04d-Inter-Process%20Communication：%20pipe,%20fifo,%20mmap%20(IPC).md)
- [Shell & Basic Commands (命令行与Shell编程)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md)
- [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md)
