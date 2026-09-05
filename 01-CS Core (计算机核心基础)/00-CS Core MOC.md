---
tags: [vault/navigation, cs/core]
status: seed
verified: 2026-09-05
---

# CS Core MOC — 计算机基础与操作系统导航

> 计算机基础与操作系统是 C++ 后端理解性能、内存与并发的底座；先建立「数据如何表示、程序如何执行、资源如何调度」的心智模型，再谈上层。

## 主题分组
- 计算机基础：[[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/01-Computer Overview (计算机系统总览)/01-What is a Computer（计算机本质）|计算机本质]] · [[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/02-Data Representation (数据表示)/01-Binary & Encoding（二进制与编码）|二进制与编码]] · [[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/02-Data Representation (数据表示)/03-Floating Point（浮点数）|浮点数]] · [[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/02-Data Representation (数据表示)/04-Bitwise Operations（位运算）|位运算]]
- 程序执行：[[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/03-Program Execution (程序执行机制)/01-Compilation & Linking (编译链接加载)|编译链接加载]] · [[01-CS Core (计算机核心基础)/01-Computer Fundamentals (计算机基础)/03-Program Execution (程序执行机制)/02-Instruction Cycle (指令执行周期)|指令执行周期]]
- 计算机组成：[[01-CS Core (计算机核心基础)/02-Computer Organization (计算机组成原理)/01-CPU Execution Model（CPU执行模型）|CPU 执行模型]] · [[01-CS Core (计算机核心基础)/02-Computer Organization (计算机组成原理)/03-Memory Hierarchy（存储层级结构⭐）|存储层级]] · [[01-CS Core (计算机核心基础)/02-Computer Organization (计算机组成原理)/04-Cache Mechanism（缓存机制⭐）|缓存机制]]
- 操作系统：[[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/00-OS Overview（操作系统总览）|OS 总览]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/01-Process vs Thread（进程与线程⭐）|进程与线程]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/03-CPU Scheduling（CPU调度）|CPU 调度]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/04-Memory Management (内存管理)/01-Virtual Memory（虚拟内存）|虚拟内存]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/04-Memory Management (内存管理)/02-Paging & Segmentation（分页与分段）|分页分段]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/05-Synchronization (同步与互斥)/01-Mutex（互斥锁）|互斥锁]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/05-Synchronization (同步与互斥)/02-Semaphore（信号量）|信号量]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/05-Synchronization (同步与互斥)/03-Deadlock（死锁）|死锁]] · [[01-CS Core (计算机核心基础)/03-Operating System (操作系统)/06-File System Basics（文件系统基础）|文件系统]]

## 学习顺序
1. 计算机基础（数据表示、位运算）→ 2. 组成原理（CPU、缓存、存储层级）→ 3. 操作系统（进程线程、内存、同步、文件）。

## 与 C++ / Linux 入口
- 内存与缓存 → [[03-C++ Programming (编程语言)/02-Core Mechanisms (核心机制)/04-Memory Model & Layout (内存模型与布局) ⭐/04a-Stack, Heap, BSS, Text Segments (四区详解)|C++ 内存模型]]
- 进程线程与同步 → [[04-Linux & System (Linux 系统)/02 · 进程与线程/04-Process Fundamentals (进程基础) ⭐/04a-Process Lifecycle： fork, exec, wait (生命周期)|Linux 进程]] · [[03-C++ Programming (编程语言)/05-Concurrency Programming (并发编程)/01-Thread Basics POSIX & std thread (线程基础) ⭐|C++ 线程]]
