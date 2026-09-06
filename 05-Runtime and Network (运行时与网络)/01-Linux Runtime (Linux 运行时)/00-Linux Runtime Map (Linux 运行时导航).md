---
tags: [vault/navigation, linux/system]
status: seed
verified: 2026-09-05
---

# 00-Linux Runtime Map (Linux 运行时导航)

> [!abstract] 阅读定位
>
> Linux 是 C++ 后端的运行底座；按「基础 → 进程线程 → I/O → 系统调用」四层组织，最终服务于 C++ 网络服务器。

## 主题分组
- 基础与命令：[[04-Linux & System (Linux 系统)/01-Linux Fundamentals (Linux 基础)/01-Shell & Basic Commands (命令行与Shell编程)|Shell 与命令]] · [[04-Linux & System (Linux 系统)/01-Linux Fundamentals (Linux 基础)/02-File System & Permissions (文件系统与权限)|文件与权限]] · [[04-Linux & System (Linux 系统)/01-Linux Fundamentals (Linux 基础)/03-System Administration Basics (系统管理基础)|系统管理]]
- 进程与线程：[[04-Linux & System (Linux 系统)/02-Processes & Threads (进程与线程)/04-Process Fundamentals (进程基础) ⭐/04a-Process Lifecycle： fork, exec, wait (生命周期)|进程]] · [[04-Linux & System (Linux 系统)/02-Processes & Threads (进程与线程)/05-Threads & Synchronization (线程与同步) ⭐/05a-POSIX Thread： pthread_create & lifecycle (线程生命周期)|线程与同步]] · [[04-Linux & System (Linux 系统)/02-Processes & Threads (进程与线程)/06-Signals & Signal Handling (信号机制)|信号]]
- I/O 模型：[[04-Linux & System (Linux 系统)/03-IO Models (输入输出模型)/07-IO Models Deep Dive (五种IO模型) ⭐/07c-IO Multiplexing： select, poll, epoll (多路复用对比)|五种 I/O 模型]] · [[04-Linux & System (Linux 系统)/03-IO Models (输入输出模型)/08-epoll Internals (epoll底层原理) ⭐/08a-epoll_create, epoll_ctl, epoll_wait (API详解)|epoll 底层]] · [[04-Linux & System (Linux 系统)/03-IO Models (输入输出模型)/09-Memory-mapped IO & sendfile (零拷贝)|零拷贝]]
- 系统调用与工具：[[04-Linux & System (Linux 系统)/04-System Calls & Tools (系统调用与工具)/10-System Calls Overview (常用系统调用速查)|系统调用速查]] · [[04-Linux & System (Linux 系统)/04-System Calls & Tools (系统调用与工具)/11-Dynamic Library & Shared Object (动态库原理) ⭐|动态库]] · [[04-Linux & System (Linux 系统)/04-System Calls & Tools (系统调用与工具)/12-Debugging & Tracing： strace, gdb, perf (调试追踪) ⭐|调试追踪]]

## 学习顺序
1. 基础命令与权限 → 2. 进程/线程/信号 → 3. 五种 I/O 模型 → 4. epoll 底层 → 5. 零拷贝 → 6. 调试追踪。

## 与 C++ / 项目入口
- 并发：[[03-C++ Programming (编程语言)/05-Concurrency Programming (并发编程)/01-Thread Basics POSIX & std thread (线程基础) ⭐|C++ 线程]]
- 网络：[[05-Network Programming (网络编程)/02-Socket Programming (Socket 编程)/05-Reactor & Proactor Pattern (事件驱动模型) ⭐/05c-Multi Reactor Multi Thread： one loop per thread (主从reactor)|Reactor 模型]]
- 项目：[[12-Backend Projects (后端项目)/00-Project Map (项目地图)|项目地图]]
