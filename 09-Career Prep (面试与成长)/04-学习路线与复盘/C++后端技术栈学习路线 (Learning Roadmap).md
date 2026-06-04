---
tags:
  - career
  - roadmap
  - interview
---

> **核心考点**：C++ 后端工程师从入门到进阶的完整学习路线

## 阶段一：基础扎实（2-3 个月）

### C++ 语言


1. 语法基础（指针、引用、const、static）
   → [Variables, Types & Operators](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)

2. 内存模型（栈/堆、内存对齐、RAII）
   → [Stack, Heap, BSS, Text Segments](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/04-Memory%20Model%20&%20Layout%20(内存模型与布局)%20⭐/04a-Stack,%20Heap,%20BSS,%20Text%20Segments%20(四区详解).md)

3. OOP（虚函数、vtable、多继承、菱形问题）
   → [Virtual Function & VTable Layout](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md)

4. Modern C++（移动语义、智能指针、lambda）
   → [Modern C++ Overview](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)

5. STL 容器原理（vector/map/unordered_map 底层）
   → [STL Cheat Sheet](/03-C++%20Programming%20(编程语言)/04%20·%20STL/06-STL%20Cheat%20Sheet%20(STL速查总览).md)


### 计算机基础


1. 操作系统（进程线程、内存管理、文件系统）
   → [OS Overview（操作系统总览）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/00-OS%20Overview（操作系统总览）.md)

2. 网络基础（TCP/IP、HTTP）
   → [TCP/IP Stack Overview（协议栈总览）](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/01-TCP⧸IP%20Stack%20Overview%20(协议栈总览).md)

3. Linux 基础（Shell、文件系统、权限）
   → [Shell & Basic Commands（命令行与Shell编程）](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md)


**里程碑：** 能徒手写一个线程安全的 LRU Cache，说出 STL 容器的底层实现。

---

## 阶段二：后端核心（3-4 个月）

### 网络编程


1. Socket API + epoll
   → [socket, bind, listen, accept（核心API）](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/04-Socket%20API%20(Socket编程接口)%20⭐/04a-socket,%20bind,%20listen,%20accept,%20connect%20(核心API).md)

2. Reactor 模型（单线程→多线程→主从）
   → [Single Reactor Single Thread（单reactor单线程）](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05a-Single%20Reactor%20Single%20Thread%20(单reactor单线程).md)

3. HTTP 协议 + 状态机解析
   → [Request & Response Structure（请求响应结构）](/05-Network%20Programming%20(网络编程)/03%20·%20HTTP与应用层/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06a-Request%20&%20Response%20Structure%20(请求响应结构).md)

4. gRPC + Protobuf
   → [09-gRPC & Protobuf](/05-Network%20Programming%20(网络编程)/03%20·%20HTTP与应用层/09-gRPC%20&%20Protobuf%20(RPC框架与序列化)%20⭐.md)

5. 服务器设计（定时器、连接池、缓冲区）
   → [Connection Pool Design（连接池设计）](/05-Network%20Programming%20(网络编程)/04%20·%20服务器设计模式/10-Connection%20Pool%20Design%20(连接池设计)%20⭐.md)


### 数据库


1. SQL 基础 + 多表查询
   → [DDL, DML, DQL（SQL基础语法）](/06-Database%20(MySQL)/01%20·%20SQL基础/01-DDL,%20DML,%20DQL%20(SQL基础语法).md)

2. InnoDB 引擎（B+树、索引、事务、MVCC、锁）
   → [Page Structure & Buffer Pool（页结构与缓冲池）](/06-Database%20(MySQL)/02%20·%20INNODB存储引擎/04-InnoDB%20Storage%20Engine%20(InnoDB存储引擎)%20⭐/04a-Page%20Structure%20&%20Buffer%20Pool%20(页结构与缓冲池).md)

3. 查询优化（EXPLAIN、慢查询）
   → [EXPLAIN & Execution Plan（执行计划分析）](/06-Database%20(MySQL)/03%20·%20查询优化/08-Query%20Optimization%20(查询优化)%20⭐/08a-EXPLAIN%20&%20Execution%20Plan%20Analysis%20(执行计划分析).md)

4. Redis（数据结构、持久化、缓存模式）
   → [SDS：Simple Dynamic String（简单动态字符串）](/08-Distributed%20&%20Middleware%20(分布式与中间件)/01%20·%20Redis%20(缓存与数据结构)/01a-Data%20Structures%20Internals%20(底层数据结构实现)%20⭐/01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md)


### Linux 系统编程


1. IO 模型（阻塞/非阻塞/多路复用/异步）
   → [Blocking vs Non-blocking IO（阻塞与非阻塞）](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07a-Blocking%20vs%20Non-blocking%20IO%20(阻塞与非阻塞).md)

2. epoll 底层（红黑树、就绪队列）
   → [epoll_create, epoll_ctl, epoll_wait（API详解）](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/08-epoll%20Internals%20(epoll底层原理)%20⭐/08a-epoll_create,%20epoll_ctl,%20epoll_wait%20(API详解).md)

3. 进程/线程/信号/IPC
   → [Process Lifecycle：fork, exec, wait（生命周期）](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)

4. 系统调用 + 动态库
   → [System Calls Overview（常用系统调用速查）](/04-Linux%20&%20System%20(Linux%20系统)/04%20·%20系统调用与工具/10-System%20Calls%20Overview%20(常用系统调用速查).md)


**里程碑：** 基于 epoll 实现一个 echo 服务器、轻量级 HTTP 服务器。

---

## 阶段三：进阶与分布式（3-4 个月）

### 分布式


1. CAP / BASE / PACELC
   → [04a-CAP Theorem](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04a-CAP%20Theorem%20&%20BASE%20Theory%20(CAP理论)%20⭐.md)

2. Raft 共识算法
   → [Leader Election（领导者选举）](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04c-Raft%20Consensus%20Algorithm%20(Raft共识算法)%20⭐/04c1-Leader%20Election%20(领导者选举)%20⭐.md)

3. 分布式事务（2PC、Saga）
   → [04e-Distributed Transaction](/08-Distributed%20&%20Middleware%20(分布式与中间件)/04%20·%20Distributed%20Protocols%20(分布式协议)/04e-Distributed%20Transaction：2PC%20&%20Saga%20(分布式事务).md)

4. 消息队列（Kafka）
   → [Why MQ（消息队列三大作用）](/08-Distributed%20&%20Middleware%20(分布式与中间件)/03%20·%20Message%20Queue%20(消息队列)/03a-Why%20MQ：Decoupling,%20Peak%20Shaving,%20Async%20(消息队列三大作用).md)

5. Nginx 架构 + 反向代理
   → [Nginx Architecture（架构模型）](/08-Distributed%20&%20Middleware%20(分布式与中间件)/02%20·%20Nginx%20(反向代理与负载均衡)/02a-Nginx%20Architecture：Master%20&%20Worker%20Process%20(架构模型).md)


### 并发与性能


1. 并发编程（线程、锁、条件变量、原子操作）
   → [Thread Basics（线程基础）](/03-C++%20Programming%20(编程语言)/05%20·%20并发编程/01-Thread%20Basics%20POSIX%20&%20std%20thread%20(线程基础)%20⭐.md)

2. 性能优化（伪共享、NUMA、内存序）
   → [07-Concurrency & Performance Optimization](/03-C++%20Programming%20(编程语言)/05%20·%20并发编程/07-C++%20Concurrency%20&%20Performance%20Optimization：Lock-free,%20Cache,%20NUMA%20(并发性能优化)%20⭐.md)

3. 工具链（GDB、perf、Sanitizer）
   → [GDB Essentials（GDB核心用法）](/07-Development%20Tools%20(工程开发工具)/03%20·%20Debug%20&%20Profiling%20(调试与性能分析)/03a-GDB%20Essentials：breakpoint,%20watch,%20backtrace%20(GDB核心用法)%20⭐.md)


### 工程能力


1. CMake + 包管理
   → [CMakeLists.txt Structure（项目结构模板）](/07-Development%20Tools%20(工程开发工具)/02%20·%20CMake%20(构建系统)/02a-CMakeLists․txt%20Structure%20(项目结构模板)%20⭐.md)

2. Git 协作
   → [Core Concepts：Three Areas（三区模型）](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01a-Core%20Concepts：Working%20Tree,%20Index,%20HEAD%20(三区模型)%20⭐.md)

3. CI/CD + Docker 部署
   → [01f-CI/CD](/07-Development%20Tools%20(工程开发工具)/01%20·%20Git%20(版本控制)/01f-CI⧸CD%20for%20C++：GitHub%20Actions,%20Static%20Analysis,%20Automation%20(CI⧸CD流水线)%20⭐.md)
   → [04e-Docker](/07-Development%20Tools%20(工程开发工具)/04%20·%20Other%20Tools%20(其他工具速查)/04e-Docker%20Multi-stage%20Build%20for%20C++：Deploy%20Optimization%20(Docker多阶段构建与部署).md)

4. 可观测性（日志 + 指标 + 追踪）
   → [09-Observability](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/09-Observability：Logging,%20Metrics,%20Tracing%20(可观测性)%20⭐.md)


**里程碑：** 设计并实现一个具备完整可观测性的分布式 key-value 缓存服务。

---

## 阶段四：面试冲刺（1-2 个月）


1. C++ 后端高频 100 题（本文件夹）
   → [C++后端面试高频100题](/09-Career%20Prep%20(面试与成长)/01-八股文（面试知识）/01-C++专项面试⭐/C++后端面试高频100题%20⭐.md)

2. 算法刷题（LeetCode Hot 100 + 剑指 Offer）
   → [Array & Two Pointers（数组与双指针）](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)

3. 项目深挖（讲好你的项目）
   → [C++后端项目难点总结](/09-Career%20Prep%20(面试与成长)/02-项目讲解/C++后端项目难点总结%20(Project%20Deep%20Dive).md)

4. 系统设计
   → [C++后端面试高频100题（最后 5 题）](/09-Career%20Prep%20(面试与成长)/01-八股文（面试知识）/01-C++专项面试⭐/C++后端面试高频100题%20⭐.md)


---

## 推荐的实践项目

| 项目 | 覆盖知识点 | 建议用时 |
|------|-----------|---------|
| Web Server（Reactor + HTTP） | epoll, 非阻塞IO, 状态机, 定时器 | 2-3 周 |
| 分布式缓存（一致性哈希 + Raft） | 一致性哈希, Raft, gRPC | 3-4 周 |
| 消息队列（生产消费模型 + 持久化） | 队列, 磁盘IO, 网络 | 3-4 周 |
| RPC 框架（服务注册发现 + 负载均衡） | gRPC, Protobuf, 注册中心 | 3-4 周 |

> **核心建议：** 不要追求看完所有笔记再动手。**看完阶段一的 C++ + 网络 + Linux，就可以开始写 Web Server 了。** 写项目过程中遇到不会的回来查笔记，理解最深。笔记是字典不是小说，边用边翻效果好过从头读到尾。
