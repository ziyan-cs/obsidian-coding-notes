---
tags:
  - career/interview
status: 🌱
---

# 01-C++ Backend Interview Top 100 (C++ 后端面试高频 100 题)

> [!abstract] 核心考点：> C++ 后端面试最高频的 100 道题，每题指向笔记库中的对应章节

## 30 秒回答

**核心结论**：核心考点：> C++ 后端面试最高频的 100 道题，每题指向笔记库中的对应章节


## 说明

每题标注难度 ★~★★★ 和对应笔记位置。答案不在本篇展开，而是指向已有笔记——确保你复习时回到原笔记的完整上下文。

---

## C++ 语言（30 题）

### 基础语法

| #   | 题目                                | 难度  | 笔记位置                                                                                                                                                                         |
| --- | --------------------------------- | --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1   | `const` 在不同位置的含义（指针、函数、成员函数）      | ★   | [Const & Static](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)                                                                  |
| 2   | `static` 的 5 种用法                  | ★   | [Const & Static](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)                                                                  |
| 3   | 左值引用 vs 右值引用 vs 万能引用              | ★★  | [Lvalue vs Rvalue](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03a-Lvalue%20vs%20Rvalue%20(左值右值区分).md)        |
| 4   | 指针和引用的区别（底层实现角度）                  | ★   | [Pointers & References](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)                                           |
| 5   | `sizeof` 和 `alignof` 在空类、虚继承下的结果  | ★★  | [Memory Model & Layout](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04a-Stack,%20Heap,%20BSS,%20Text%20Segments%20(四区详解).md) |
| 6   | 重载、重写、隐藏的区别                       | ★   | [Constructor & Destructor Order](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/06-Class%20Internals%20(类的底层)%20⭐/06a-Constructor%20&%20Destructor%20Order%20(构造析构顺序).md)      |
| 7   | `inline` 函数什么时候有用？什么时候无效？         | ★   | [Functions](/02-C++%20Backend%20(C++%20后端)/01-Basic%20Syntax%20(基础语法)/03-Functions%20(函数).md)                                                                                     |
| 8   | 函数指针 vs `std::function` vs lambda | ★★  | [Lambda & Function Object](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Lambda%20&%20Function%20Object%20(Lambda与函数对象)%20⭐.md)                                        |

### OOP

| #   | 题目                  | 难度  | 笔记位置                                                                                                                                                                                           |
| --- | ------------------- | --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 9   | 虚函数表（vtable）的内存布局   | ★★★ | [Virtual Function & VTable Layout](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md) |
| 10  | 多继承的菱形问题与虚继承        | ★★★ | [Multiple & Virtual Inheritance](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07d-Multiple%20&%20Virtual%20Inheritance%20(多继承与虚继承).md)        |
| 11  | 构造函数和析构函数中调用虚函数会怎样  | ★★  | [Multiple & Virtual Inheritance](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07d-Multiple%20&%20Virtual%20Inheritance%20(多继承与虚继承).md)        |
| 12  | 为什么析构函数要用 virtual   | ★   | [Abstract Class & Pure Virtual](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07c-Abstract%20Class%20&%20Pure%20Virtual%20(抽象类).md)            |
| 13  | Rule of Five（三/五法则） | ★★  | [Copy Control & Rule of 5](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/06-Class%20Internals%20(类的底层)%20⭐/06b-Copy%20Control%20&%20Rule%20of%205%20(拷贝控制与五法则).md)                            |
| 14  | 拷贝构造函数参数为什么必须是引用    | ★   | [Pointers & References](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)                                                           |

### 内存

| #   | 题目                                            | 难度  | 笔记位置                                                                                                                                                                                                                                                                                                                              |
| --- | --------------------------------------------- | --- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 15  | 程序内存布局：stack / heap / BSS / data / text       | ★★  | [Stack, Heap, BSS, Text Segments](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04a-Stack,%20Heap,%20BSS,%20Text%20Segments%20(四区详解).md) |
| 16  | 内存对齐的原则与 `#pragma pack`                       | ★★  | [Memory Alignment](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04b-Memory%20Alignment%20(内存对齐).md)                                           |
| 17  | 栈上对象 vs 堆上对象的生命周期                             | ★   | [Memory Alignment](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04b-Memory%20Alignment%20(内存对齐).md)                                           |
| 18  | 内存池的原理和适用场景                                   | ★★  | [Memory Pool Implementation](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04c-Memory%20Pool%20Implementation%20(内存池实现).md)            |
| 19  | placement new 和自定义分配器                         | ★★★ | [Custom Allocators & Placement New](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/10-Custom%20Allocators%20&%20Placement%20New%20(自定义分配器).md)                                                                                                 |
| 20  | 智能指针：`unique_ptr` / `shared_ptr` / `weak_ptr` | ★★  | [Smart Pointers](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/04-Smart%20Pointers%20(智能指针)%20⭐/04a-unique%20ptr%20Ownership%20Model%20(独占所有权).md)                                                                          |

### Modern C++

| #   | 题目                                               | 难度  | 笔记位置                                                                                                                                                                                                             |
| --- | ------------------------------------------------ | --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 21  | 移动语义干了什么？什么时候用 `std::move`？                      | ★★  | [Move Constructor & Move Assignment](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03b-Move%20Constructor%20&%20Move%20Assignment%20(移动构造与移动赋值).md) |
| 22  | 完美转发的原理：引用折叠 + `std::forward`                    | ★★★ | [Perfect Forwarding & Universal Reference](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/05-Perfect%20Forwarding%20&%20Universal%20Reference%20(完美转发)%20⭐.md)                                                 |
| 23  | `auto` 和 `decltype` 的类型推导规则                      | ★★  | [Modern C++ Overview](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)                                                                                        |
| 24  | lambda 的捕获方式和生命周期                                | ★★  | [Lambda & Function Object](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/02-Lambda%20&%20Function%20Object%20(Lambda与函数对象)%20⭐.md)                                                                            |
| 25  | `constexpr` 编译期计算 vs `consteval`                 | ★★  | [constexpr & Compile-time Computation](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/06-constexpr%20&%20Compile-time%20Computation%20(编译期计算).md)                                                              |
| 26  | `if constexpr` 如何替代 SFINAE                       | ★★  | [constexpr & Compile-time Computation](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/06-constexpr%20&%20Compile-time%20Computation%20(编译期计算).md)                                                              |
| 27  | `std::optional` / `std::variant` / `std::any`    | ★   | [optional & variant](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/07-optional%20&%20variant%20(新类型工具).md)                                                                                                    |
| 28  | Coroutines：`co_await` / `co_yield` / `co_return` | ★★★ | [Coroutines Overview](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/10-Coroutines%20Overview%20(协程入门).md)                                                                                                     |
| 29  | Concept 解决了什么问题                                  | ★★  | [Concepts & SFINAE](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/09-Concepts%20&%20SFINAE%20(概念与模板元编程)%20⭐.md)                                                                                               |
| 30  | Structured Binding（结构化绑定）                        | ★   | string_view & Structured Bindings                                                                                                                                                                                |

### 并发

| # | 题目 | 难度 | 笔记位置 |
|---|------|------|---------|
| 31 | `std::thread` vs POSIX 线程 | ★ | [Thread Basics](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/01-Thread%20Basics%20POSIX%20&%20std%20thread%20(线程基础)%20⭐.md) |
| 32 | `std::mutex` / `std::lock_guard` / `std::unique_lock` | ★ | [Mutex & Lock](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/02-Mutex%20&%20Lock%20(互斥锁与锁管理)%20⭐.md) |
| 33 | 死锁的四个必要条件与预防 | ★★ | [Mutex & Lock](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/02-Mutex%20&%20Lock%20(互斥锁与锁管理)%20⭐.md) |
| 34 | `std::atomic` 和 memory order | ★★★ | [Atomic & Memory Order](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/04-Atomic%20&%20Memory%20Order%20(原子操作与内存序)%20⭐.md) |
| 35 | 条件变量为什么必须配合 mutex | ★★ | [Condition Variable & Semaphore](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/03-Condition%20Variable%20&%20Semaphore%20(条件变量与信号量)%20⭐.md) |
| 36 | 线程池的设计要点（任务队列、动态扩缩） | ★★ | [Thread Pool Implementation](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/05-Thread%20Pool%20Implementation%20(线程池手写)%20⭐.md) |
| 37 | 伪共享（False Sharing）的成因和解决 | ★★★ | [Concurrency & Performance Optimization](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/07-C++%20Concurrency%20&%20Performance%20Optimization：Lock-free,%20Cache,%20NUMA%20(并发性能优化)%20⭐.md) |
| 38 | `std::future` / `std::promise` / `std::async` | ★★ | [Lock-free Structures](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/06-Lock-free%20Structures%20Overview%20(无锁结构概念).md) |
| 39 | 读写锁 (`shared_mutex`) 适用场景 | ★★ | [Concurrency & Performance Optimization](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/07-C++%20Concurrency%20&%20Performance%20Optimization：Lock-free,%20Cache,%20NUMA%20(并发性能优化)%20⭐.md) |
| 40 | lock-free 编程的基本思路 | ★★★ | [Lock-free Structures](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/06-Lock-free%20Structures%20Overview%20(无锁结构概念).md) |

### STL

| #   | 题目                                 | 难度  | 笔记位置                                                                                                                                                                                    |
| --- | ---------------------------------- | --- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 41  | `vector` 扩容机制与 `reserve`           | ★★  | [vector Dynamic Array](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01a-vector%20Dynamic%20Array%20&%20Reallocation%20(动态扩容原理)%20⭐.md) |
| 42  | `map` vs `unordered_map` 底层结构对比    | ★★  | [STL Cheat Sheet](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/06-STL%20Cheat%20Sheet%20(STL速查总览).md)                                                                                  |
| 43  | `deque` 的块状存储结构                    | ★★  | [STL Cheat Sheet](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/06-STL%20Cheat%20Sheet%20(STL速查总览).md)                                                                                  |
| 44  | 迭代器失效的场景（vector/map/unordered_map） | ★★  | [Iterators & Iterator Categories](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/04-Iterators%20&%20Iterator%20Categories%20(迭代器分类).md)                                                  |
| 45  | `std::sort` 的实现原理（内省排序）            | ★★  | [Algorithm Library](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/05-Algorithm%20Library%20(算法库速查).md)                                                                                  |

---

## 网络（15 题）

| # | 题目 | 难度 | 笔记位置 |
|---|------|------|---------|
| 46 | 三次握手和四次挥手 State 变化 | ★★ | [Three-Way Handshake & Four-Way Teardown](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/01-Network%20Fundamentals%20(网络基础)/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02a-Three-Way%20Handshake%20&%20Four-Way%20Teardown%20(三次握手四次挥手).md) |
| 47 | TIME_WAIT 为什么是 2MSL | ★★ | [TIME_WAIT](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/01-Network%20Fundamentals%20(网络基础)/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02c-TIME_WAIT：Why%20&%20How%20to%20Handle%20(TIME_WAIT原因与处理).md) |
| 48 | TCP 拥塞控制的四种算法 | ★★★ | [Flow Control & Congestion Control](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/01-Network%20Fundamentals%20(网络基础)/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02d-Flow%20Control%20&%20Congestion%20Control%20(流量控制与拥塞控制).md) |
| 49 | 粘包怎么解决（代码实现） | ★★ | [Sticky Packet Problem & Solutions](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/01-Network%20Fundamentals%20(网络基础)/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02e-Sticky%20Packet%20Problem%20&%20Solutions%20(粘包问题与解决).md) |
| 50 | select / poll / epoll 对比 | ★★★ | [epoll vs select](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08c-epoll%20vs%20select：%20Red-Black%20Tree%20&%20Event%20Queue%20(底层实现对比).md) |
| 51 | epoll ET vs LT 模式 | ★★★ | [Level Trigger vs Edge Trigger](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08b-Level%20Trigger%20vs%20Edge%20Trigger：%20LT⧸ET%20(触发模式).md) |
| 52 | Reactor 和 Proactor 的区别 | ★★★ | [Reactor & Proactor Pattern](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05a-Single%20Reactor%20Single%20Thread%20(单reactor单线程).md) |
| 53 | HTTPS TLS 握手过程 | ★★ | [HTTPS & TLS Overview](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/07-HTTPS%20&%20TLS%20Overview%20(HTTPS原理概览).md) |
| 54 | HTTP/2 多路复用的原理 | ★★ | [HTTP/2 Key Features](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/08-HTTP⧸2%20Key%20Features%20(HTTP2核心特性了解).md) |
| 55 | gRPC 四种通信模式 | ★★ | [gRPC & Protobuf](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md) |
| 56 | Protobuf 编码原理（varint） | ★★★ | [gRPC & Protobuf](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md) |
| 57 | 零拷贝：mmap + sendfile | ★★ | [Memory-mapped IO & sendfile](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/09-Memory-mapped%20IO%20&%20sendfile%20(零拷贝).md) |
| 58 | 非阻塞 connect 怎么实现 | ★★ | [Non-blocking Socket](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04b-Non-blocking%20Socket%20&%20O_NONBLOCK%20(非阻塞Socket).md) |
| 59 | SO_REUSEADDR 解决什么 | ★ | [SO_REUSEADDR & SO_KEEPALIVE](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04c-SO_REUSEADDR%20&%20SO_KEEPALIVE%20(套接字选项).md) |
| 60 | HTTP 状态机解析怎么实现 | ★★ | [State Machine Parser Implementation](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06d-State%20Machine%20Parser%20Implementation%20(状态机解析实现).md) |

---

## 操作系统（10 题）

| #   | 题目             | 难度  | 笔记位置                                                                                                                                                                            |
| --- | -------------- | --- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 61  | 进程 vs 线程 vs 协程 | ★   | [Process vs Thread](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/01-Process%20vs%20Thread%20(进程与线程)%20⭐.md)                                                                  |
| 62  | 上下文切换的成本       | ★★  | [Context Switching](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/02-Context%20Switching%20(上下文切换).md)                                                                     |
| 63  | 虚拟内存解决了什么问题    | ★★  | [Virtual Memory](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/04-Memory%20Management%20(内存管理)/01-Virtual%20Memory%20(虚拟内存).md)                                              |
| 64  | 分页 vs 分段       | ★★  | [Paging & Segmentation](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/04-Memory%20Management%20(内存管理)/02-Paging%20&%20Segmentation%20(分页与分段).md)                             |
| 65  | 页面置换算法（LRU 实现） | ★★  | LRU（当前无专题笔记） |
| 66  | 死锁的四个条件和解决方案   | ★★  | [Deadlock](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/03-Deadlock%20(死锁).md)                                                                 |
| 67  | 互斥锁和信号量的本质区别   | ★   | [Mutex](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/01-Mutex%20(互斥锁).md)                                                                    |
| 68  | CPU 调度算法对比     | ★★  | [CPU Scheduling](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/03-CPU%20Scheduling%20(CPU调度).md)                                                                         |
| 69  | 硬链接 vs 软链接     | ★   | [File System Basics](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/06-File%20System%20Basics%20(文件系统基础).md)                                                              |
| 70  | 零拷贝的几种实现方式     | ★★  | [Memory-mapped IO & sendfile](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/09-Memory-mapped%20IO%20&%20sendfile%20(零拷贝).md)                                            |

---

## 数据库（15 题）

| #   | 题目                               | 难度  | 笔记位置                                                                                                                                                                                         |
| --- | -------------------------------- | --- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 71  | 索引的 B+ Tree 结构                   | ★★  | [B+ Tree Index Structure](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04b-B+%20Tree%20Index%20Structure%20(B+树索引结构).md)                     |
| 72  | 聚簇索引 vs 二级索引 vs 覆盖索引             | ★★★ | [Clustered vs Secondary Index](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04c-Clustered%20vs%20Secondary%20Index%20(聚簇索引与二级索引).md)         |
| 73  | 索引下推是什么                          | ★★  | [Index Pushdown & Covering Index](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04d-Index%20Pushdown%20&%20Covering%20Index%20(索引下推与覆盖索引).md) |
| 74  | 四种隔离级别和并发问题                      | ★★  | [Isolation Levels](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05a-Isolation%20Levels：RU,%20RC,%20RR,%20Serializable%20(四种隔离级别).md)               |
| 75  | MVCC 实现原理（undo log + ReadView）   | ★★★ | [MVCC Internals](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/05-Transaction%20&%20ACID%20(事务与ACID)%20⭐/05c-MVCC%20Internals：undo%20log%20&%20read%20view%20(MVCC底层实现).md)                   |
| 76  | Next-Key Lock 解决幻读               | ★★★ | [Gap Lock & Next-Key Lock](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/06-Locks%20In%20MySQL%20(MySQL锁机制)%20⭐/06b-Gap%20Lock%20&%20Next-Key%20Lock%20(间隙锁与临键锁).md)                          |
| 77  | EXPLAIN 怎么看（type/key/rows/Extra） | ★★  | [EXPLAIN & Execution Plan Analysis](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08a-EXPLAIN%20&%20Execution%20Plan%20Analysis%20(执行计划分析).md)                 |
| 78  | 最左前缀匹配原则                         | ★★  | [EXPLAIN & Execution Plan Analysis](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/03-Query%20Optimization%20(查询优化)/08-Query%20Optimization%20(查询优化)%20⭐/08a-EXPLAIN%20&%20Execution%20Plan%20Analysis%20(执行计划分析).md)                 |
| 79  | Redo Log 崩溃恢复原理                  | ★★★ | [Redo Log](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07b-Redo%20Log：Crash%20Recovery%20(崩溃恢复).md)                                |
| 80  | Binlog vs Redo Log 区别            | ★★  | [Binlog vs Redo Log](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/02-InnoDB%20Storage%20Engine%20(InnoDB%20存储引擎)/07-Redo%20Log%20&%20Undo%20Log%20&%20Binlog%20(三大日志)%20⭐/07c-Binlog%20vs%20Redo%20Log：Differences%20(两者区别).md)             |
| 81  | 主从复制的三种模式                        | ★★  | [Master-Slave Replication](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/10-Master-Slave%20Replication%20(主从复制原理).md)                                                                             |
| 82  | JOIN 和子查询的性能差异                   | ★★  | [Joins & Subqueries](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/01-SQL%20Fundamentals%20(SQL%20基础)/02-Joins%20&%20Subqueries%20(多表查询与子查询).md)                                                                                      |
| 83  | 分库分表的策略                          | ★★  | [Sharding & Partitioning](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/11-Sharding%20&%20Partitioning%20Overview%20(分库分表概念).md)                                                                  |
| 84  | 缓存穿透/击穿/雪崩                       | ★★  | [MySQL vs Redis](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/12-MySQL%20vs%20Redis：Caching%20Strategy%20(缓存策略对比).md)                                                                            |
| 85  | Redis 为什么快（单线程模型）                | ★   | [Redis Single Thread Model](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01e-Redis%20Single%20Thread%20Model%20(单线程模型为何高性能)%20⭐.md)                             |

---

## 分布式系统（10 题）

| # | 题目 | 难度 | 笔记位置 |
|---|------|------|---------|
| 86 | CAP 理论和 PACELC | ★★ | [CAP Theorem & BASE Theory](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) |
| 87 | Raft 的 Leader 选举过程 | ★★★ | [Leader Election](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md) |
| 88 | Raft 日志复制和安全性 | ★★★ | [Log Replication](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c2-Log%20Replication%20(日志复制)%20⭐.md) |
| 89 | Paxos 和 Raft 的区别 | ★★★ | [Paxos Overview](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04d-Paxos%20Overview%20(Paxos概念了解).md) |
| 90 | 2PC 和 Saga 的优缺点 | ★★ | [Distributed Transaction](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04e-Distributed%20Transaction：2PC%20&%20Saga%20(分布式事务).md) |
| 91 | Kafka 消息可靠性的三种 ACK 模式 | ★★ | [Kafka Message Delivery](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/03-Message%20Queues%20(消息队列)/03b-Kafka%20Architecture%20Overview%20(Kafka架构概览)/03b2-Message%20Delivery%20Guarantees%20(消息可靠性).md) |
| 92 | 一致性哈希解决了什么问题 | ★★ | [Reverse Proxy & Load Balancing](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md) |
| 93 | Redis 过期策略和内存淘汰 | ★★ | [Expiration & Eviction Strategy](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01c-Expiration%20&%20Eviction%20Strategy%20(过期与淘汰策略)%20⭐.md) |
| 94 | Redis 持久化：RDB vs AOF | ★★ | [Redis Persistence](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01b-Persistence：RDB%20&%20AOF%20(持久化机制)%20⭐/01b1-RDB：Snapshot%20&%20BGSAVE%20(快照原理).md) |
| 95 | 布隆过滤器的原理 | ★★ | [Cache Penetration](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/01-Redis%20(缓存与数据结构)/01d-Cache%20Patterns%20&%20Problems%20(缓存三大问题)%20⭐/01d1-Cache%20Penetration%20(缓存穿透：布隆过滤器).md) |

---

## 系统设计（5 题）

| # | 题目 | 要点 | 笔记位置 |
|---|------|------|---------|
| 96 | 设计一个短 URL 系统 | 发号器 + 哈希 + 重定向 | [CAP Theorem & BASE Theory](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md) |
| 97 | 设计一个即时通讯系统 | WebSocket + 消息存储 + 推拉 | [gRPC & Protobuf](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md) |
| 98 | 设计一个秒杀系统 | 限流 + 缓存 + 异步下单 | [MySQL vs Redis](/03-Backend%20Systems%20(后端系统)/03-Database%20(数据库)/04-High%20Availability%20&%20Architecture%20(高可用与架构)/12-MySQL%20vs%20Redis：Caching%20Strategy%20(缓存策略对比).md) |
| 99 | 设计一个配置中心 | etcd + watch + 版本管理 | [Leader Election](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/04-Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md) |
| 100 | 设计一个 API 网关 | 路由 + 限流 + 鉴权 + 熔断 | [Backend Architecture Patterns](/03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/04-Server%20Design%20Patterns%20(服务器设计模式)/14-Backend%20Architecture%20Patterns：分层架构,%20CQRS,%20事件驱动%20(后端架构模式)%20⭐.md) |

> **复习建议**：100 题不要求全背。把★的答到流利，★★的理解原理能画图，★★★的至少能说出核心思想。标注的笔记位置指向完整笔记，不要只看本题——回到原笔记深挖。

---

## 答案速查

### C++ 语言

1. **`const` 在不同位置的含义（指针、函数、成员函数）**
   `const` 修饰变量表示该变量不可修改。`const int* p` 表示指向的内容不可变，`int* const p` 表示指针本身不可变。在成员函数中，`const` 声明该函数不会修改对象状态（修饰 this 指针），使得该函数能被 const 对象调用，也允许 const 引用调用成员函数。

2. **`static` 的 5 种用法**
   （1）静态局部变量：生命周期为整个程序运行期间，但作用域仍限于函数内；（2）静态全局变量：仅在本编译单元可见，避免不同文件的名字冲突；（3）静态函数：作用域仅限于本文件，类似 internal linkage；（4）静态成员变量：属于类而非某个对象，所有实例共享同一份；（5）静态成员函数：没有 this 指针，只能访问类的静态成员。

3. **左值引用 vs 右值引用 vs 万能引用**
   左值引用（`T&`）只能绑定左值；右值引用（`T&&`）只能绑定右值，用于实现移动语义和延长临时对象生命周期。万能引用在模板推导或 `auto&&` 中表现为 `T&&`，当传入左值时被推导为 `T&`，传入右值时推导为 `T&&`，从而支持完美转发。

4. **指针和引用的区别（底层实现角度）**
   底层汇编实现上指针和引用都是通过地址来操作对象，没有本质区别。语法上引用必须在定义时初始化且不可改变指向，而指针可以重新赋值；引用没有空引用问题（更安全），使用起来像普通变量，而指针需要判空。

5. **`sizeof` 和 `alignof` 在空类、虚继承下的结果**
   空类的 `sizeof` 为 1（编译器插入一个字节使不同对象有唯一地址）；含虚函数的类增加一个 vptr（8 字节/32 位下 4 字节）。虚继承会引入虚基类指针（vbptr），对象体积增大。`alignof` 通常返回最宽基本成员的对齐值，对空类返回 1。

6. **重载、重写、隐藏的区别**
   重载（Overload）发生在同一作用域，函数名相同但参数列表不同。重写（Override）是派生类覆盖基类的虚函数，要求签名完全一致，实现多态。隐藏（Hide）是派生类定义同名函数屏蔽了基类的所有同名函数，无论参数是否相同——这是名字查找优先在派生类作用域停止所致。

7. **`inline` 函数什么时候有用？什么时候无效？**
   `inline` 对函数体很小（如 getter/setter）且高频调用的场景最有用，可以消除函数调用开销。编译器有权忽略 `inline`：函数体过大、递归函数、通过函数指针调用时，大多编译器不会内联。现代编译器会基于启发式自动内联，`inline` 更多是建议而非强制。

8. **函数指针 vs `std::function` vs lambda**
   函数指针是 C 风格的可调用对象，类型必须严格匹配。`std::function` 是多态包装器，可存储任何可调用对象（函数指针、lambda、bind 表达式），但有 type erasure 带来的虚函数级开销。Lambda 是匿名函数对象，编译器为其生成匿名类，C++14 后支持泛型和初始化捕获，性能最优。

9. **虚函数表（vtable）的内存布局**
   每个包含虚函数的类有一张虚函数表（存储虚函数指针的数组），每个对象有一个 vptr 指向所属类对应的 vtable。多继承下对象有多个 vptr，每个基类子对象对应一个 vtable。派生类重写的虚函数会覆盖基类 vtable 中对应槽位。vtable 通常位于只读数据段（`.rodata`）。
   **追问方向：** vptr 在子类构造过程中何时被赋值？虚函数表在内存的哪个区域？

10. **多继承的菱形问题与虚继承**
    菱形继承（Diamond Inheritance）使派生类包含两个基类子对象副本，导致数据冗余和二义性。虚继承通过虚基类指针（vbptr）指向虚基类表（vbtable），让所有派生类共享同一个虚基类子对象。虚基类子对象通常放在对象内存末尾，vbptr 中存储相对偏移。
    **追问方向：** 虚继承的对象内存布局具体是怎样的？vbptr 和 vbtable 存了什么？

11. **构造函数和析构函数中调用虚函数会怎样**
    不会发生动态绑定——调用的是当前类自己的版本。因为构造时 vptr 指向当前正在构造的类的 vtable（子类还没构造完成），析构时 vptr 已经切换回当前类（子类部分已析构）。这是 C++ 保证对象构造/析构期间类型一致性的设计。

12. **为什么析构函数要用 virtual**
    当通过基类指针 `delete` 派生类对象时，若析构函数非虚则调用基类的析构函数，派生类资源不会被释放，导致内存泄漏。使用虚析构能确保调用最派生类的析构函数，再按继承链反向依次调用各基类析构，从而正确释放所有资源。

13. **Rule of Five（三/五法则）**
    若类管理了堆内存等动态资源，通常需要自定义：析构函数、拷贝构造函数、拷贝赋值运算符（三法则）。C++11 引入移动语义后增加移动构造函数和移动赋值运算符（五法则），用于窃取临时对象的资源而非深拷贝。若什么都不定义，编译器会逐成员拷贝/移动，对指针成员只会浅拷贝导致 double free。

14. **拷贝构造函数参数为什么必须是引用**
    如果拷贝构造函数的参数不是引用而是值传递，则调用拷贝构造函数前需要先拷贝实参，又会触发拷贝构造函数的调用，形成无限递归（编译报错或栈溢出）。用 const 引用传参可以避免这一递归，同时 const 保证不会修改原对象。

15. **程序内存布局：stack / heap / BSS / data / text**
    Text（代码段）存放只读的机器指令。Data 段存放已初始化的全局/静态变量。BSS 段存放未初始化的全局/静态变量（运行时初始化为 0）。Heap（堆）由 `malloc`/`new` 动态分配，向高地址增长。Stack（栈）存放局部变量和函数调用信息，向低地址增长。越界写栈可覆盖返回地址（栈溢出攻击）。

16. **内存对齐的原则与 `#pragma pack`**
    对齐原则：结构体成员的起始地址必须是其自身大小的整数倍，结构体总大小是最宽成员对齐值的整数倍，编译器会在成员间插入 padding。`#pragma pack(n)` 强制以 n 字节对齐，常用于网络协议和二进制文件读写以节省空间，但会牺牲访问效率。

17. **栈上对象 vs 堆上对象的生命周期**
    栈上对象的生命周期由作用域决定：进入作用域时构造，离开作用域时自动析构（RAII）。堆上对象用 `new` 创建，生命周期由程序员通过 `delete` 控制，不释放则内存泄漏。栈分配仅移动栈指针（O(1)），堆分配涉及系统调用和空闲链表管理，开销大得多。

18. **内存池的原理和适用场景**
    内存池预先从系统申请一大块内存，在应用层进行小块管理，避免频繁 `malloc`/`free` 的系统调用和内存碎片。适用场景：大量小对象的频繁分配释放（如网络服务器的连接对象、游戏引擎的实体创建）。Google 的 TCMalloc 和 Intel 的 TBB 中有成熟实现。

19. **placement new 和自定义分配器**
    Placement new 在已分配的内存上构造对象而不分配新内存：`new(buffer) T(args)`。自定义分配器可实现 `allocate`/`deallocate` 策略，STL 容器支持通过模板参数传入。两者配合可用于内存池场景：从池中拿内存 + placement new 构造对象，销毁时显式调用析构函数而非 `delete`。
    **追问方向：** Placement new 如何与内存池配合使用？如何正确销毁 placement new 创建的对象？

20. **智能指针：`unique_ptr` / `shared_ptr` / `weak_ptr`**
    `unique_ptr` 独占所有权，不可拷贝只能移动（`std::move`），析构自动释放，性能与裸指针相当。`shared_ptr` 通过引用计数共享所有权：拷贝时计数 +1，析构时 -1，减到 0 时释放资源。`weak_ptr` 不增加引用计数，用于打破循环引用（如观察者模式）或检测资源是否仍存在（`lock()` 返回空的 `shared_ptr` 表示已释放）。

21. **移动语义干了什么？什么时候用 `std::move`？**
    移动语义允许将资源所有权从一个对象转移到另一个对象，避免深拷贝开销。本质是"窃取"源对象的资源指针，再将源对象置为有效但未指定的空状态。`std::move` 不移动任何东西，只是将左值强制转换为右值引用，使其匹配移动构造函数/移动赋值运算符。

22. **完美转发的原理：引用折叠 + `std::forward`**
    引用折叠规则：`T& &`、`T& &&`、`T&& &` 都折叠为 `T&`，只有 `T&& &&` 折叠为 `T&&`。`std::forward` 根据模板参数 T 的推导结果有条件地返回左值或右值引用——当 T 被推导为 `T&` 时返回左值引用，推导为 `T&&` 时返回右值引用，从而实现参数的"完美转发"。
    **追问方向：** 引用折叠有几种可能的组合？转发引用和右值引用的本质区别？

23. **`auto` 和 `decltype` 的类型推导规则**
    `auto` 使用模板类型推导规则，忽略引用和顶层 const/volatile（除非声明 `auto&` 或 `const auto`）。`decltype` 不进行推导，直接返回表达式的精确类型（包括引用和 const）。特别注意 `decltype((x))` 对变量加括号会推导为引用类型。

24. **lambda 的捕获方式和生命周期**
    按值捕获（`[=]`）在 lambda 创建时拷贝变量到匿名函数对象中；按引用捕获（`[&]`）捕获变量的引用，必须保证 lambda 被调用时原始变量仍存活。C++14 支持初始化捕获（`[x = std::move(obj)]`）和泛型 lambda。默认按值捕获的变量不可修改，除非加 `mutable` 关键字。

25. **`constexpr` 编译期计算 vs `consteval`**
    `constexpr` 函数既可以在编译期也可以在运行时求值，取决于参数是否在编译期已知。`consteval`（C++20）强制函数只能在编译期求值，若无法在编译期求值则编译错误。两者都可用于编译期计算，但 `consteval` 提供了更严格的保证，适合需要绝对编译期确定的场景（如数组大小）。

26. **`if constexpr` 如何替代 SFINAE**
    `if constexpr` 在编译期根据常量表达式选择分支，未选中的分支不会被实例化（连语法检查都不做），从而替代传统 SFINAE（Substitution Failure Is Not An Error）的繁琐模板技巧。相比 `enable_if` 的多重重载，`if constexpr` 将条件逻辑集中在一个函数模板内，代码更清晰、更易维护。

27. **`std::optional` / `std::variant` / `std::any`**
    `optional<T>` 表示可能包含 T 值或空（类似可为 null 的语义），避免使用哨兵值。`variant<T1, T2, ...>` 是类型安全的联合体，同时只持有一种类型的值，用 `std::visit` 访问。`any` 可以持有任意类型（类型擦除），性能开销大，应优先用 `variant`。

28. **Coroutines：`co_await` / `co_yield` / `co_return`**
    C++20 协程是无栈协程，函数中含 `co_await`/`co_yield`/`co_return` 即视为协程，可被挂起和恢复。`co_await` 等待一个可等待对象使协程挂起（如异步 IO 完成后再恢复）；`co_yield` 产生一个值并挂起（Generator 模式）；`co_return` 返回结果并结束协程。协程状态存储在堆上分配的协程帧中。
    **追问方向：** 协程帧（coroutine frame）在堆上还是栈上分配？`co_await` 背后的 awaitable 和 awaiter 机制？

29. **Concept 解决了什么问题**
    Concept 约束模板参数必须满足的语义接口（如 `std::integral<T>`、`std::convertible_to<T, U>`），替代了 SFINAE 的编译期技巧。它提供更清晰、可读的错误消息（不再是一大堆模板实例化堆栈），使用 `requires` 子句可精确表达接口约束，使重载解析和模板使用更加直观。

30. **Structured Binding（结构化绑定）**
    用 `auto [a, b, c] = expr` 一次性解构数组、tuple-like 对象或结构体的成员，编译器展开为对 `std::tuple_size`/`get<>` 或成员变量的访问。常用于函数返回多值（返回 `tuple`/`pair`/`struct`）的解包，使代码更简洁。可以加 `&` 引用绑定避免拷贝。

### 并发

31. **`std::thread` vs POSIX 线程**
    `std::thread` 是 C++11 提供的跨平台线程抽象，内部封装 POSIX 线程（Linux）或 Windows 线程，API 统一。相比直接使用 pthread，`std::thread` 与 RAII 结合更好（析构自动 `join` 或 `detach`），配合 `std::jthread`（C++20）还能自动响应取消请求。

32. **`std::mutex` / `std::lock_guard` / `std::unique_lock`**
    `std::mutex` 是最基础的互斥量。`lock_guard` 是 RAII 封装：构造时加锁，析构时解锁，不可手动干预。`unique_lock` 更灵活，支持延迟加锁、尝试加锁、提前解锁，可与条件变量配合，但持有锁管理状态（是否已锁），有一定性能开销。优先用 `lock_guard`，需要灵活性时用 `unique_lock`。

33. **死锁的四个必要条件与预防**
    四个条件：互斥（资源不可共享）、持有并等待、不可剥夺、循环等待。预防可破坏后三者：一次性申请全部资源（破坏持有并等待）、申请不到时释放已有资源（破坏不可剥夺）、按固定顺序申请锁或用 `std::lock()` 同时锁定多个 mutex（破坏循环等待）。实践中规范加锁顺序是最有效的方法。

34. **`std::atomic` 和 memory order**
    `std::atomic` 保证变量的操作是原子且线程安全的，避免数据竞争（data race）。memory order 控制可见性和排序约束：`memory_order_relaxed` 只保证原子性不保证顺序；`memory_order_acquire/release` 成对使用保证 release 之前的写对 acquire 之后的读可见（释放-获取语义）；默认 `memory_order_seq_cst` 提供全局顺序一致性，性能开销最大。
    **追问方向：** `memory_order_relaxed` 在实际中有什么应用场景？acquire-release 语义如何保证线程间同步？

35. **条件变量为什么必须配合 mutex**
    条件变量的 `wait` 操作需要"检查条件"和"挂起等待"两个步骤，存在"唤醒丢失"问题——在检查和挂起之间信号可能被发送而错过。mutex 保护条件检查，`wait` 内部原子地释放锁并进入休眠，确保线程在检查条件时不会被信号干扰。`wait` 返回时重新获取锁，再重新检查条件（防止虚假唤醒）。

36. **线程池的设计要点（任务队列、动态扩缩）**
    核心组件：线程集合 + 任务队列（`std::deque` + mutex + condition_variable）。要点：线程数量的确定（CPU 密集型 = 核心数，IO 密集型可更大）、任务队列的线程安全（生产者-消费者模型）、动态扩缩（根据任务积压数调整线程数，可用两个阈值触发扩缩容）、优雅关闭（通知所有线程退出 + 等待剩余任务完成）。

37. **伪共享（False Sharing）的成因和解决**
    多个线程访问同一缓存行（cache line，通常 64 字节）中不同变量时，某个核修改变量会使整个缓存行在其他核上失效，导致频繁缓存同步，严重降低性能。解决方法：用 `alignas(64)` 或 `alignas(std::hardware_destructive_interference_size)` 将热点变量对齐到独立的缓存行。
    **追问方向：** 如何检测和定位伪共享？`std::hardware_destructive_interference_size` 的值是多少？

38. **`std::future` / `std::promise` / `std::async`**
    `std::promise` 和 `std::future` 是一对异步通道：promise 端写入值或异常，future 端通过 `get()` 阻塞获取结果。`std::async` 是高级封装，内部创建线程执行任务并返回 future，可以指定启动策略：`launch::async`（立即创建线程）或 `launch::deferred`（`get()` 时同步执行，即惰性求值）。

39. **读写锁（`shared_mutex`）适用场景**
    读写锁允许多个线程同时读（共享），但写操作独占访问，在读多写少的场景下比普通 mutex 能显著提升并发性能。C++17 提供 `std::shared_mutex`，读线程用 `lock_shared()`/`unlock_shared()`，写线程用 `lock()`/`unlock()`。注意写饥饿问题：如果读请求持续不断，写线程可能永远得不到执行。

40. **lock-free 编程的基本思路**
    核心是使用 CAS（Compare-And-Swap）原子操作代替互斥锁，通过循环重试解决冲突（乐观并发）。常用手段：`std::atomic` 管理共享指针、利用 tagged pointer 解决 ABA 问题、使用 hazard pointer 或 epoch-based reclamation（如 crossbeam）管理内存回收。lock-free 能避免死锁和优先级反转，但实现极其复杂，建议优先用现成库。
    **追问方向：** 什么是 ABA 问题？如何通过 tagged pointer 解决？

### STL

41. **`vector` 扩容机制与 `reserve`**
    `vector` 容量不足时申请新内存（通常是旧容量的 1.5 或 2 倍），然后将原元素移动或拷贝到新内存，再释放旧内存。扩容是昂贵操作，涉及元素重新构造和所有迭代器失效。用 `reserve(n)` 预分配足够容量可避免多次扩容，在已知元素数量或大致范围时应优先使用。

42. **`map` vs `unordered_map` 底层结构对比**
    `map` 底层是红黑树，元素按 key 排序，插入/删除/查找 O(log n)，支持范围查询和有序遍历。`unordered_map` 底层是哈希表，平均 O(1) 查找（最坏 O(n)），元素无序。需要有序迭代或范围查找用 `map`，追求单点查询性能且不关心顺序用 `unordered_map`。

43. **`deque` 的块状存储结构**
    `deque` 由多个固定大小的连续缓冲区（块）组成，中央是映射（map 数组）管理块指针，实现双向增长。两端插入/删除 O(1)（分摊），中间插入需移动元素 O(n)。`operator[]` 需要先定位到块再块内偏移，常数比 vector 略大但仍是 O(1)。

44. **迭代器失效的场景**
    `vector` 在扩容或插入/删除导致元素位移时全部迭代器和引用失效。`map`/`set` 只使被删除元素的迭代器失效，插入不影响其他（红黑树结构稳定）。`unordered_map` 在 rehash 时全部失效。避免技巧：用 `reserve` 避免 vector 扩容，删除用 `erase(iter++)`（C++11 后 `erase` 返回下一迭代器）。

45. **`std::sort` 的实现原理（内省排序）**
    `std::sort` 使用 IntroSort（内省排序）：开始用快速排序（三数取中选 pivot），当递归深度超过 `2*logN` 时切换为堆排序（避免快排退化到 O(n²)），当分区规模小于阈值（通常 16）时切换到插入排序（小数据插入排序最快）。混合策略保证平均和最坏都是 O(n log n)。

### 网络

46. **三次握手和四次挥手 State 变化**
    三次握手：Client 发送 SYN → SYN_SENT；Server 回复 SYN+ACK → SYN_RCVD；Client 发送 ACK → ESTABLISHED。四次挥手：主动方发 FIN → FIN_WAIT_1；被动方回 ACK → CLOSE_WAIT（主动方到 FIN_WAIT_2）；被动方发 FIN → LAST_ACK；主动方回 ACK → TIME_WAIT（2MSL 后 CLOSED）。

47. **TIME_WAIT 为什么是 2MSL**
    2MSL 确保最后一个 ACK 能到达对端（若 ACK 丢失，对端会重发 FIN，主动方需时间重发 ACK），也保证本连接的全部报文在网络中消失，避免旧连接的报文干扰使用了相同 IP 和端口的新连接。MSL 是报文最大生存时间（通常 2 分钟，实际实现常设为 30s-60s）。

48. **TCP 拥塞控制的四种算法**
    （1）慢启动：cwnd 从 1 开始，每收到一个 ACK 指数增长，到 ssthresh 后转为拥塞避免；（2）拥塞避免：cwnd 线性增长（每 RTT +1），探测网络容量；（3）快速重传：收到 3 个冗余 ACK 立即重传，不等超时；（4）快速恢复：快速重传后 cwnd 减半但不回到慢启动，继续保持线性增长。Linux 默认使用 CUBIC 变体。
    **追问方向：** BBR 算法和传统基于丢包的拥塞控制算法有什么区别？

49. **粘包怎么解决（代码实现）**
    TCP 是流式协议，没有消息边界，多个消息可能"粘"在一起。常用方案：（1）固定长度消息（实现简单但浪费空间）；（2）特殊分隔符（如 `\r\n`，HTTP 使用，但消息体需转义避免混淆）；（3）长度前缀法（最常用：头部 4 字节存 payload 长度 + 消息体，接收方先读 4 字节确定长度再读对应字节）。

50. **select / poll / epoll 对比**
    select 用固定位图（FD_SETSIZE=1024 限制），每次需全量遍历和用户态/内核态拷贝。poll 用链表数组解决 1024 限制但仍有遍历 O(n) 问题。epoll（Linux 特有）使用红黑树管理 fd + 就绪链表（回调），只返回就绪事件，性能与活跃连接数成正比（O(1)），适合 C10K+ 高并发场景。
    **追问方向：** epoll 的 ET 模式为什么性能更好？使用时需要注意什么？

51. **epoll ET vs LT 模式**
    LT（水平触发）是默认模式：fd 有数据可读时 epoll_wait 一直返回，编程简单但可能重复通知。ET（边缘触发）只在状态变化（如从无数据变为有数据）时通知一次，之后必须循环读直到 EAGAIN，效率更高（减少 epoll 调用次数），但必须配合非阻塞 IO 使用（否则最后一次阻塞 read 会卡死线程）。
    **追问方向：** 为什么 ET 模式必须配合非阻塞 IO 使用？

52. **Reactor 和 Proactor 的区别**
    Reactor 是同步事件多路复用：IO 就绪后通知应用，应用自己读写（如 epoll + 非阻塞 IO）。Proactor 是异步 IO：操作系统完成读写后通知应用（如 Windows IOCP）。Reactor 更适合 IO 密集型且事件处理轻量的场景，Proactor 在 Windows 上原生支持，在 Linux 上需用 libaio 或 boost.asio 模拟。
    **追问方向：** Proactor 模式为什么在 Linux 上需要模拟？Boost.Asio 的实现思路？

53. **HTTPS TLS 握手过程**
    （1）ClientHello：客户端发送支持的 TLS 版本、密码套件列表和随机数；（2）ServerHello：服务端选择密码套件、发送证书和随机数；（3）客户端验证证书（CA 链），生成预主密钥并用服务端公钥加密发送；（4）双方用 DH 或 RSA 交换生成对称密钥，后续通信使用对称加密。TLS 1.3 握手减少到 1-RTT（首次）或 0-RTT（恢复）。

54. **HTTP/2 多路复用的原理**
    HTTP/2 在 TCP 连接内引入流（Stream）和帧（Frame）的概念，多个请求（流）复用一条 TCP 连接，不同流的帧在连接上交错传输（二进制分帧层）。相比 HTTP/1.1 的队头阻塞（一个请求必须等前一个完成），多路复用大幅减少连接数量和延迟。但 TCP 级别的队头阻塞仍然存在（丢包阻塞所有流）。

55. **gRPC 四种通信模式**
    （1）一元 RPC（Unary）：客户端发一个请求，服务端回一个响应，类似传统 RPC；（2）服务端流式：客户端一个请求，服务端返回一组消息（如订阅推送）；（3）客户端流式：客户端发送一组消息，服务端一个响应（如批量上传）；（4）双向流式：双方各自独立发送和接收，全双工通信（如实时聊天）。底层基于 HTTP/2 流实现。

56. **Protobuf 编码原理（varint）**
    Varint 用可变长度编码整数：每个字节最高位为 continuation bit（1 表示后续还有字节），低 7 位存数据，小整数更省空间（0-127 仅 1 字节）。Protobuf 采用键值对编码：key = (field_number << 3) | wire_type。解码时根据 wire_type 可跳过未知字段，这保证了向后兼容性。
    **追问方向：** Zigzag 编码的原理是什么？为什么配合 sint64 使用？

57. **零拷贝：mmap + sendfile**
    零拷贝的目标是避免数据在内核和用户空间之间多次拷贝。mmap 将内核页缓存映射到用户空间地址，应用直接操作页缓存，省去一次 read 到用户缓冲区的拷贝。sendfile 在文件描述符与 socket 之间直接传输（内核内部完成），完全不经过用户态，实现真正的零拷贝。

58. **非阻塞 connect 怎么实现**
    设置 socket 为 O_NONBLOCK 后调用 connect，立即返回 -1（errno = EINPROGRESS），然后用 select/epoll 监听 socket 的可写事件。socket 可写时连接建立成功；若连接失败，可通过 `getsockopt(SO_ERROR)` 获取错误码。这种方式在等待连接时能干其他事，常用于高并发客户端（如批量建连）。

59. **SO_REUSEADDR 解决什么**
    解决两个问题：（1）服务端重启时能绑定到仍处于 TIME_WAIT 状态的端口（崩溃或重启后快速恢复服务）；（2）多个进程可绑定同一个地址和端口（用于多播场景）。Linux 3.9+ 还有 `SO_REUSEPORT`，允许多个 socket 绑定同一端口实现内核级负载均衡。

60. **HTTP 状态机解析怎么实现**
    用有限状态机逐字节解析 HTTP 请求：主要状态包括 METHOD、URI、VERSION、HEADER_LINE（以 `\r\n` 为界）、HEADER_END（空行分隔）、BODY。每个状态处理对应字符并决定下一状态，收到完整 header 后根据 Content-Length 或 Transfer-Encoding（chunked）读取 body。FSM 适合非阻塞 IO 场景，控制流清晰。

### 操作系统

61. **进程 vs 线程 vs 协程**
    进程是资源分配的最小单位，拥有独立的地址空间（页表、文件描述符等），切换成本高（需切换页表/TLB）。线程是 CPU 调度的最小单位，共享进程地址空间，切换需内核态但 TLB 等不必刷新（同一进程内）。协程是用户态轻量级"线程"，由程序自身调度（非抢占式），切换只需保存寄存器，开销在纳秒级。

62. **上下文切换的成本**
    线程上下文切换需保存/恢复 CPU 寄存器、程序计数器、栈指针，还涉及 TLB 刷新和 cache miss（切换进程时）。一次切换在微秒级，但对高并发（万级线程）累积成本很大，尤其引起大量 cache miss 对性能影响显著。这是为什么协程和异步编程在高并发场景下更有优势。

63. **虚拟内存解决了什么问题**
    （1）隔离保护：每个进程独立虚拟地址空间，互不干扰；（2）简化内存管理：程序员无需关心物理内存分布，分段加载使大程序可在小内存运行；（3）共享内存：多个进程可映射同一物理页（如共享库代码在物理内存中只存一份）；（4）按需加载：只有访问到的页面才从磁盘加载（请求分页 / demand paging）。

64. **分页 vs 分段**
    分段按逻辑划分（代码段、数据段、栈段等），段大小可变，便于共享和保护，但会产生外部碎片且管理复杂。分页将内存划分为固定大小（通常 4KB）的页，通过多级页表管理虚拟地址到物理地址的映射，消除外部碎片。现代 64 位操作系统主要使用分页（分段只用作兼容），Linux 将分段基址设为 0 来"绕过"分段。

65. **页面置换算法（LRU 实现）**
    LRU 置换最久未被访问的页面。O(1) 实现：哈希表（快速查找）+ 双向链表（维护访问顺序）。每次访问时将节点移到链表头，淘汰时移除链表尾节点。手写 LRU Cache 是经典面试题。实际 OS 常用近似 LRU（如 Clock 算法）以减少硬件支持成本。

66. **死锁的四个条件和解决方案**
    四个条件：互斥、持有并等待、不可剥夺、循环等待。解决方案分四类：（1）预防——破坏上述一个条件（如按固定顺序加锁）；（2）避免——银行家算法（但实际 OS 较少用）；（3）检测与恢复——检测到死锁后强制终止或回滚线程；（4）鸵鸟策略——忽略（Linux/Windows 均采用）。

67. **互斥锁和信号量的本质区别**
    互斥锁（Mutex）用于互斥访问，只能由持有锁的线程释放，无计数功能。信号量（Semaphore）是计数同步原语，允许多个线程同时访问有限资源（初始值 = 资源数），任何线程均可通过 `sem_post` 递增（不区分持有者）。信号量初始值 = 1 时可作互斥锁用，但语义不同（不强制谁释放）。

68. **CPU 调度算法对比**
    FCFS 简单但平均等待时间长（convoy effect，长任务阻塞短任务）。SJF 平均等待时间最短但无法预知 CPU 突发长度。时间片轮转（RR）保证公平和响应时间，但时间片太短则上下文切换频繁。多级反馈队列（MLFQ）综合多策略：按优先级调度，用完时间片降级，优先级高的时间片小。Linux CFS（完全公平调度）用红黑树按虚拟运行时间调度。

69. **硬链接 vs 软链接**
    硬链接指向同一 inode（文件数据），删除原文件不影响硬链接，不能跨文件系统和链接目录。软链接（符号链接）是独立文件，内容为目标路径，原文件删除后链接变成"悬空链接"，可跨文件系统和链接目录。Linux 中 `ls -li` 看 inode 号可区分。

70. **零拷贝的几种实现方式**
    （1）mmap：文件映射到进程地址空间，避免 read 到用户缓冲区的拷贝；（2）sendfile：在文件描述符间直接传输（完全内核态，不需用户态参与）；（3）splice：在两个文件描述符间移动数据，不经过用户态；（4）带 DMA scatter/gather 的 sendfile：数据不必在内核空间连续存放，进一步减少拷贝。这些技术常用于静态文件服务器和消息中间件。

### 数据库

71. **索引的 B+ Tree 结构**
    B+ Tree 是多路平衡查找树：所有数据存储在叶子节点（非叶子节点只存 key 作为路由索引），叶子节点通过链表相连形成有序链表。相比 B 树（数据在全部节点），B+ 树非叶子节点可存更多 key（树更矮，减少磁盘 IO），且范围查询只需遍历叶子链表，磁盘顺序读性能极好。

72. **聚簇索引 vs 二级索引 vs 覆盖索引**
    聚簇索引（InnoDB 主键索引）的叶子节点直接存储整行数据，表数据就是索引，每表只能有一个。二级索引叶子节点存储主键值，通过二级索引查数据需要回表（再查一次聚簇索引）。覆盖索引是二级索引叶子节点已包含查询所需全部列，无需回表，Extra 显示 "Using index"，性能最好。
    **追问方向：** 为什么 InnoDB 推荐使用自增主键？UUID 做主键有什么问题？

73. **索引下推是什么**
    索引下推（Index Condition Pushdown，ICP）是 MySQL 5.6 引入的优化：在索引遍历过程中提前对索引包含的字段做 WHERE 条件过滤，减少回表次数。不加 ICP 时，存储引擎遍历索引到叶子节点后回表获取全行再交给 server 层过滤。ICP 将部分条件下推到存储引擎层，边遍历边过滤，减少 IO 开销。

74. **四种隔离级别和并发问题**
    RU（读未提交）可能脏读（读到其他事务未提交的数据）；RC（读已提交）避免脏读但不可重复读（同一事务内两次读不一致）；RR（可重复读）避免脏读和不可重复读但有幻读（同一条件范围两次读行数不同）；Serializable 最严格但性能最差。MySQL InnoDB 默认 RR，通过 MVCC 解决不可重复读，通过 Next-Key Lock 解决幻读。

75. **MVCC 实现原理（undo log + ReadView）**
    MVCC 通过隐藏字段（DB_TRX_ID 事务 ID、DB_ROLL_PTR 回滚指针）实现多版本并发控制。读时根据 ReadView 判断版本可见性：ReadView 记录当前活跃事务列表，只能看到已提交的快照版本。写时生成 undo log 保存旧版本。读不加锁（快照读），写加行锁，实现读写互不阻塞。
    **追问方向：** ReadView 在 RC 和 RR 级别下的生成时机有何不同？

76. **Next-Key Lock 解决幻读**
    Next-Key Lock = 行锁（Record Lock）+ 间隙锁（Gap Lock），锁住一个左开右闭区间。RR 级别下对范围查询不仅给匹配行加行锁，还在匹配行之间加 Gap Lock，阻止其他事务在间隙插入新行，从而解决幻读。注意：唯一索引的等值查询会退化为行锁（只锁存在的那一行），因为唯一性保证不会有幻行。
    **追问方向：** 什么情况下 Gap Lock 会退化为行锁？唯一索引等值查找时 Next-Key Lock 的行为？

77. **EXPLAIN 怎么看（type/key/rows/Extra）**
    type 表示访问类型，最优到最差：system > const > eq_ref > ref > range > index > ALL。key 表示实际使用的索引。rows 是估算的扫描行数，越小越好。Extra 中 "Using index" 表示覆盖索引，"Using where" 表示回表后过滤，"Using temporary" 表示用了临时表（需优化），"Using filesort" 表示文件排序（尽量优化为索引排序）。

78. **最左前缀匹配原则**
    复合索引的 B+ Tree 按照定义时的列顺序建立，查询必须从最左边的列开始匹配才能使用索引。例如索引 (a, b, c)，条件必须包含 a 列才能用到该索引；跳过中间列（查询 a 和 c 但不查 b）只能用到 a 列部分的索引（c 列的过滤无法走索引）。应将选择性高的列放在左边。

79. **Redo Log 崩溃恢复原理**
    Redo Log 记录已提交事务的物理修改（页级别修改），遵循 WAL（Write-Ahead Logging）：事务提交前先将修改写入 Redo Log（顺序写），再写入数据页（随机写）。崩溃恢复时，通过 Redo Log 重放（redo）尚未刷盘的数据页修改，保证持久性（Durability）。Checkpoint 机制记录已刷盘的 LSN，可加快恢复速度。
    **追问方向：** 为什么 Redo Log 比直接写数据页快？Checkpoint 如何加速恢复？

80. **Binlog vs Redo Log 区别**
    本质不同：Redo Log 是 InnoDB 引擎层的物理日志（记录对数据页的修改），循环写，用于崩溃恢复。Binlog 是 MySQL Server 层的逻辑日志（记录 SQL 或行变更），追加写，用于主从复制和数据恢复。Binlog 有三种格式：statement（SQL）、row（行变更，推荐）、mixed。两者配合保证了 crash-safe（崩溃后 binlog 与数据一致）。

81. **主从复制的三种模式**
    （1）异步复制：主库提交即返回，不等待从库确认，性能最好但有数据丢失风险（主库宕机时未同步的数据丢失）；（2）半同步复制：主库等待至少一个从库写入 relay log 后才返回，在性能和一致性间平衡，生产常用；（3）全同步复制：主库等待所有从库确认，一致性最强但延迟最大。

82. **JOIN 和子查询的性能差异**
    MySQL 优化器通常将子查询改写为 JOIN 或 semi-join 执行，但某些情况下（如 IN 子查询返回大量数据，或相关子查询）会逐行执行导致性能很差。JOIN 通常更高效，尤其是能用上索引的 INNER JOIN。但 JOIN 过多也会产生临时表。一律用 `EXPLAIN` 分析执行计划，而不是凭直觉选择。

83. **分库分表的策略**
    水平分表按数据行拆分：哈希分片（取模/一致性哈希）、范围分片（按 ID 区间或时间）、列表分片（按地域）。垂直分库按业务拆分（不同库放不同业务表）。关键考虑：跨节点 JOIN 和聚合查询的代价、分布式事务（建议尽量避免跨分片事务）、全局唯一 ID、数据迁移与扩容的平滑性。

84. **缓存穿透/击穿/雪崩**
    穿透（一直查不存在的数据）：每次请求跳过缓存直击数据库。解决：布隆过滤器拦截不存在的 key，或缓存空值（短过期时间）。击穿（热点 key 过期瞬间大量并发请求）：解决：互斥锁保证只有一个线程查数据库，或热点 key 设置永不过期加后台更新。雪崩（大量 key 同时过期或缓存宕机）：解决：过期时间加随机值打散、缓存集群高可用、本地缓存兜底。

85. **Redis 为什么快（单线程模型）**
    核心是单线程消除了锁竞争和上下文切换开销。此外依赖：（1）纯内存操作（最主要的因素）；（2）IO 多路复用（epoll 处理海量连接）；（3）数据结构简单高效（跳表、压缩列表等针对场景优化）。注意：Redis 6.0 后引入多线程处理网络 IO，但命令执行仍是单线程，所以原子性不受影响。

### 分布式系统

86. **CAP 理论和 PACELC**
    CAP 定理：分布式系统在发生网络分区（P）时，只能在一致性（C）和可用性（A）之间权衡（不能三者全满足）。PACELC 扩展：即使没有分区（E），也要在延迟（L）和一致性（C）之间权衡。大多互联网场景选 AP（可用性优先 + 最终一致性），金融场景选 CP（一致性优先，可用性可以降级）。

87. **Raft 的 Leader 选举过程**
    Follower 在选举超时内未收到 Leader 心跳，转为 Candidate，任期号 +1，并发起 RequestVote RPC 请求其他节点投票。获得超过半数（N/2 + 1）投票即成为新 Leader，然后定期发送心跳维持权威。若选票被瓜分（split vote），各 Candidate 随机增加超时时间后重试，降低再次冲突的概率。
    **追问方向：** Raft 如何防止一个任期内选出多个 Leader？

88. **Raft 日志复制和安全性**
    Leader 收到客户端请求后追加日志到本地，并行向所有 Follower 发送 AppendEntries RPC。日志被多数节点复制后即算提交（committed），然后应用到状态机。安全性保证：（1）选举限制——Candidate 必须拥有全部已提交日志才能当选；（2）日志匹配特性——AppendEntries 的一致性检查确保日志连续无空洞。
    **追问方向：** Raft 的"选举限制"具体指什么？日志匹配特性如何保证一致性？

89. **Paxos 和 Raft 的区别**
    两者都是共识算法。Paxos 更抽象（涉及 Prepare/Promise/Accept/Accepted 多轮，Multi-Paxos 概念复杂），理解和实现门槛高。Raft 将共识分解为 Leader 选举、日志复制、安全性三个相对独立的子问题，更可理解和实现。Raft 强调强 Leader（只有 Leader 处理写请求），日志是连续的（没有 Paxos 的日志空洞问题），所以更受欢迎并被 etcd/consul 采用。
    **追问方向：** Raft 的强 Leader 特性和 Paxos 相比有什么优势？Multi-Paxos 与 Raft 的异同？

90. **2PC 和 Saga 的优缺点**
    2PC：协调者先 Prepare 询问所有参与者，全部同意后 Commit。优点是强一致性（ACID），缺点是同步阻塞（Prepare 后资源锁定不能做其他事）和协调者单点故障。Saga：将大事务拆为多个本地事务 + 补偿事务，协调失败时反向补偿撤销。优点是高性能（无锁，高并发），缺点是最终一致性（隔离性弱，中间状态对外可见）。

91. **Kafka 消息可靠性的三种 ACK 模式**
    `acks=0`：Producer 发完即算成功，不确认服务端是否收到，吞吐量最大但可能丢消息。`acks=1`：Leader 写入本地日志即算成功，不等待 Follower 同步，兼顾吞吐和可靠性（Leader 挂了可能丢数据）。`acks=all`（或 -1）：Leader 等待 ISR 中所有 Replica 同步后才算成功，保证不丢但延迟最大，吞吐量最低。

92. **一致性哈希解决了什么问题**
    解决传统取模哈希在节点增删时大量缓存失效（缓存雪崩迁移）的问题。将节点和数据都映射到同一哈希环上，数据存储在顺时针最近节点，节点变化仅影响该节点附近的数据。引入虚拟节点解决实际节点不均匀导致的数据偏斜（每个物理节点有多个虚拟节点分布在环上）。

93. **Redis 过期策略和内存淘汰**
    过期策略：定期删除（每 100ms 随机抽查一批设置了过期时间的 key，删除已过期的）+ 惰性删除（访问时检查是否过期，过期则删）。内存淘汰机制（当内存到达 maxmemory 时触发）：noeviction（写入报错）、allkeys-lru（淘汰最久未使用的 key）、volatile-lru、allkeys-random、volatile-ttl（淘汰剩余 TTL 最短的）等。生产常用 allkeys-lru。

94. **Redis 持久化：RDB vs AOF**
    RDB 定期生成全量快照（fork 子进程写时复制），适合备份和灾难恢复，但可能丢失两次快照间的数据。AOF 记录每个写命令（追加写），可通过 `fsync` 策略配置安全等级：always（每命令同步，最安全最慢）、everysec（每秒同步，推荐）、no（由 OS 决定刷盘时机）。Redis 4.0+ 支持混合持久化（RDB 全量 + AOF 增量日志），兼顾快速加载和数据安全。

95. **布隆过滤器的原理**
    布隆过滤器用位数组 + k 个独立哈希函数判断元素是否存在。添加元素时用 k 个哈希函数算出 k 个位置，将对应位设为 1。查询时若所有位都是 1 则"可能存在"（有误判率 false positive），若有一位是 0 则"一定不存在"。不能删除元素（除非升级为计数布隆过滤器）。适合解决缓存穿透：将存在的 key 预先加入布隆过滤器拦截不存在的请求。

### 系统设计

96. **设计一个短 URL 系统**
    核心流程：发号器生成唯一 ID（分布式可用雪花算法），ID 转 Base62 编码得到短串，将长 URL 与短串映射存入数据库（读多写少，Redis 缓存热点短 URL）。重定向时根据短串查原始 URL 返回 302 跳转。需要考虑短码冲突检测、发号器高可用、过期清理、数据统计（点击次数）。

97. **设计一个即时通讯系统**
    客户端通过 WebSocket 保持长连接（在线状态管理）。消息发送：先写入消息存储（保证不丢），然后在线的走 WebSocket 实时推送，离线的走 APNs/FCM 推送通知。群聊的实现有两种方案：写扩散（每个群成员存一份私有的消息队列，读快写慢）和读扩散（消息只存一份，每个成员拉取自己时间线，读慢写快）。微信等大厂结合使用。

98. **设计一个秒杀系统**
    关键原则：前端限流 + 层层过滤 + 异步化。前端按钮置灰防止重复提交，CDN 静态化。后端：网关限流（令牌桶/IP 级别限流）→ Redis 预减库存（原子递减 + Lua 脚本防止超卖）→ 消息队列异步落库（削峰填谷，数据库只处理与库存相等的订单量）。最终一致性：库存扣减成功但订单落库失败时需要补偿。

99. **设计一个配置中心**
    核心组件：配置存储（etcd 或 Zookeeper 保证强一致性 + watch 机制实时推送变更），客户端 SDK 缓存配置到本地文件（启动时先加载本地再请求远程，容灾降级）。配置变更流程：运维在管理端修改配置 → etcd 存储 → 客户端 watch 到变更后更新本地缓存。灰度发布：按 IP/机房/用户比例逐步推送。

100. **设计一个 API 网关**
     核心功能：统一路由（根据 URL 前缀分发到后端微服务）、认证鉴权（JWT/OAuth2 统一校验）、限流熔断（令牌桶/滑动窗口，超时比例超阈值后快速失败保护下游）、日志/监控/全链路追踪。高可用：无状态设计多实例部署、配置中心动态路由更新、断路器模式（circuit breaker）防止雪崩。



## 零基础阅读路径

先将本页结论转换成自己的 30 秒表达；再补一个具体约束和项目证据；最后练习反例与取舍，避免把速记当成理解。

## 常见误区

- 只背标准答案，不把结论放进真实约束、取舍和项目证据中，表达会显得空泛。
- 不计时复述、不追问反例，也不通过项目日志或代码核对，就会形成虚假熟练。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-C++ Backend Interview Top 100 (C++ 后端面试高频 100 题)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
