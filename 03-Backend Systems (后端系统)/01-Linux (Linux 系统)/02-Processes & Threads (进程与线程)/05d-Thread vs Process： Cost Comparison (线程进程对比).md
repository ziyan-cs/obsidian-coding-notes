---
tags:
  - linux/process
status: 🌱
---

# Thread vs Process： Cost Comparison — 线程进程对比

> [!important] **核心考点**：线程 vs 进程的创建/切换/通信开销、Linux clone 系统调用、共享资源差异

## 创建开销

|操作|典型耗时|原因|
|---|---|---|
|fork()|~100μs|需要复制页表、文件描述符表等（COW 优化后好很多）|
|pthread_create()|~10μs|只分配栈和 TCB，共享进程地址空间|
|协程切换|~100ns|用户态切换，只保存少量寄存器|

## 核心对比

|维度|进程|线程|
|---|---|---|
|地址空间|独立|共享（同一进程内）|
|通信|IPC（复杂）|共享内存（简单，但需同步）|
|隔离性|强（崩溃不影响其他进程）|弱（一个线程崩溃可能拖垮整个进程）|
|创建开销|大|小|
|上下文切换|大（需切换页表、刷 TLB）|小（同一地址空间，只换栈和寄存器）|
|适用场景|需要强隔离（浏览器多进程）|需要高效通信（Web 服务器工作线程）|

## 上下文切换的代价

线程切换：

1. 保存当前线程寄存器（通用寄存器、PC、SP）到 TCB
2. 恢复目标线程寄存器
3. 切换栈指针

进程切换额外还需： 4. 切换 CR3（页表基址寄存器）→ TLB 全部失效（代价最大） 5. 切换文件描述符表等内核资源

> **TLB 失效**是进程切换比线程切换慢的核心原因。现代 CPU 用 ASID（地址空间标识符）标记 TLB 条目，可部分缓解这个问题。

线程进程对比见 → [POSIX Thread (线程生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md) · [Mutex & Condition Variable (互斥锁与条件变量)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md)
