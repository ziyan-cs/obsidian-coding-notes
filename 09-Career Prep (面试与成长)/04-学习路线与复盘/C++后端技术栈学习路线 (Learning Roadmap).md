---
tags:
  - career
  - roadmap
  - interview
---

> **核心考点**：C++ 后端工程师从入门到进阶的完整学习路线

## 阶段一：基础扎实（2-3 个月）

### C++ 语言

```
1. 语法基础（指针、引用、const、static）
   → 笔记：03-C++/01-基础语法

2. 内存模型（栈/堆、内存对齐、RAII）
   → 笔记：03-C++/02-核心机制/04-Memory Model

3. OOP（虚函数、vtable、多继承、菱形问题）
   → 笔记：03-C++/02-核心机制/07-VTable

4. Modern C++（移动语义、智能指针、lambda）
   → 笔记：03-C++/03-现代 C++

5. STL 容器原理（vector/map/unordered_map 底层）
   → 笔记：03-C++/04-STL
```

### 计算机基础

```
1. 操作系统（进程线程、内存管理、文件系统）
   → 笔记：01-CS Core/03-OS

2. 网络基础（TCP/IP、HTTP）
   → 笔记：05-Network/01-网络基础

3. Linux 基础（Shell、文件系统、权限）
   → 笔记：04-Linux/01-Linux基础
```

**里程碑：** 能徒手写一个线程安全的 LRU Cache，说出 STL 容器的底层实现。

---

## 阶段二：后端核心（3-4 个月）

### 网络编程

```
1. Socket API + epoll
   → 笔记：05-Network/02-Socket编程

2. Reactor 模型（单线程→多线程→主从）
   → 笔记：05-Network/02-Socket编程/05-Reactor

3. HTTP 协议 + 状态机解析
   → 笔记：05-Network/03-HTTP

4. gRPC + Protobuf
   → 笔记：05-Network/03-HTTP/09-gRPC

5. 服务器设计（定时器、连接池、缓冲区）
   → 笔记：05-Network/04-服务器设计
```

### 数据库

```
1. SQL 基础 + 多表查询
   → 笔记：06-Database/01-SQL基础

2. InnoDB 引擎（B+树、索引、事务、MVCC、锁）
   → 笔记：06-Database/02-INNODB（全部 12 篇）

3. 查询优化（EXPLAIN、慢查询）
   → 笔记：06-Database/03-查询优化

4. Redis（数据结构、持久化、缓存模式）
   → 笔记：08-Distributed/01-Redis
```

### Linux 系统编程

```
1. IO 模型（阻塞/非阻塞/多路复用/异步）
   → 笔记：04-Linux/03-IO模型

2. epoll 底层（红黑树、就绪队列）
   → 笔记：04-Linux/03-IO模型/08-epoll

3. 进程/线程/信号/IPC
   → 笔记：04-Linux/02-进程与线程

4. 系统调用 + 动态库
   → 笔记：04-Linux/04-系统调用与工具
```

**里程碑：** 基于 epoll 实现一个 echo 服务器、轻量级 HTTP 服务器。

---

## 阶段三：进阶与分布式（3-4 个月）

### 分布式

```
1. CAP / BASE / PACELC
   → 笔记：08-Distributed/04-协议/04a-CAP

2. Raft 共识算法
   → 笔记：08-Distributed/04-协议/04c-Raft（全部 3 篇）

3. 分布式事务（2PC、Saga）
   → 笔记：08-Distributed/04-协议/04e-Distributed Transaction

4. 消息队列（Kafka）
   → 笔记：08-Distributed/03-MQ

5. Nginx 架构 + 反向代理
   → 笔记：08-Distributed/02-Nginx
```

### 并发与性能

```
1. 并发编程（线程、锁、条件变量、原子操作）
   → 笔记：03-C++/05-并发编程

2. 性能优化（伪共享、NUMA、内存序）
   → 笔记：03-C++/05-并发编程/07-Performance Optimization

3. 工具链（GDB、perf、Sanitizer）
   → 笔记：07-Development Tools/03-Debug
```

### 工程能力

```
1. CMake + 包管理
   → 笔记：07-Development Tools/02-CMake

2. Git 协作
   → 笔记：07-Development Tools/01-Git

3. CI/CD + Docker 部署
   → 笔记：07-Development Tools/01-Git/01f-CICD
   → 笔记：07-Development Tools/04-Other/04e-Docker

4. 可观测性（日志 + 指标 + 追踪）
   → 笔记：03-C++/06-工程实践/09-Observability
```

**里程碑：** 设计并实现一个具备完整可观测性的分布式 key-value 缓存服务。

---

## 阶段四：面试冲刺（1-2 个月）

```
1. C++ 后端高频 100 题（本文件夹）
   → 笔记：10-Career/01-八股文/C++后端面试高频100题

2. 算法刷题（LeetCode Hot 100 + 剑指 Offer）
   → 笔记：02-Algorithms

3. 项目深挖（讲好你的项目）
   → 笔记：10-Career/02-项目讲解

4. 系统设计
   → 笔记：10-Career/01-八股文/C++后端面试高频100题（最后 5 题）
```

---

## 推荐的实践项目

| 项目 | 覆盖知识点 | 建议用时 |
|------|-----------|---------|
| Web Server（Reactor + HTTP） | epoll, 非阻塞IO, 状态机, 定时器 | 2-3 周 |
| 分布式缓存（一致性哈希 + Raft） | 一致性哈希, Raft, gRPC | 3-4 周 |
| 消息队列（生产消费模型 + 持久化） | 队列, 磁盘IO, 网络 | 3-4 周 |
| RPC 框架（服务注册发现 + 负载均衡） | gRPC, Protobuf, 注册中心 | 3-4 周 |

> **核心建议：** 不要追求看完所有笔记再动手。**看完阶段一的 C++ + 网络 + Linux，就可以开始写 Web Server 了。** 写项目过程中遇到不会的回来查笔记，理解最深。笔记是字典不是小说，边用边翻效果好过从头读到尾。
