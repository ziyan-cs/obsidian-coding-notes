---
tags: [vault/navigation, network/programming]
status: seed
verified: 2026-09-05
---

# Network MOC — 网络编程导航

> 从协议栈到服务器设计模式；核心落点是「用 C++ 写一个可用的高并发网络服务器」。

## 主题分组
- 网络基础：[[05-Network Programming (网络编程)/01-Network Fundamentals (网络基础)/01-TCP⧸IP Stack Overview (协议栈总览)|TCP/IP 总览]] · [[05-Network Programming (网络编程)/01-Network Fundamentals (网络基础)/02-TCP Deep Dive (TCP深入) ⭐/02a-Three-Way Handshake & Four-Way Teardown (三次握手四次挥手)|TCP 深入]] · [[05-Network Programming (网络编程)/01-Network Fundamentals (网络基础)/03-UDP Characteristics & Use Cases (UDP特性与使用场景)|UDP]]
- Socket：[[05-Network Programming (网络编程)/02-Socket Programming (Socket 编程)/04-Socket API (Socket编程接口) ⭐/04a-socket, bind, listen, accept, connect (核心API)|Socket API]] · [[05-Network Programming (网络编程)/02-Socket Programming (Socket 编程)/05-Reactor & Proactor Pattern (事件驱动模型) ⭐/05c-Multi Reactor Multi Thread： one loop per thread (主从reactor)|Reactor/Proactor]]
- 应用层：[[05-Network Programming (网络编程)/03-HTTP & Application Layer (HTTP 与应用层)/06-HTTP⧸1.1 Protocol (HTTP协议详解) ⭐/06a-Request & Response Structure (请求响应结构)|HTTP/1.1]] · [[05-Network Programming (网络编程)/03-HTTP & Application Layer (HTTP 与应用层)/07-HTTPS & TLS Overview (HTTPS原理概览)|TLS]] · [[05-Network Programming (网络编程)/03-HTTP & Application Layer (HTTP 与应用层)/09-gRPC & Protobuf (RPC框架与序列化) ⭐|gRPC]]
- 服务器设计模式：[[05-Network Programming (网络编程)/04-Server Design Patterns (服务器设计模式)/09-Timer Implementation (定时器实现) ⭐/09a-Min-Heap Timer (小根堆定时器)|定时器]] · [[05-Network Programming (网络编程)/04-Server Design Patterns (服务器设计模式)/10-Connection Pool Design (连接池设计) ⭐|连接池]] · [[05-Network Programming (网络编程)/04-Server Design Patterns (服务器设计模式)/11-Buffer Design：Read & Write Buffer (缓冲区设计) ⭐|缓冲区]] · [[05-Network Programming (网络编程)/04-Server Design Patterns (服务器设计模式)/13-Graceful Shutdown (优雅关闭) ⭐|优雅关闭]]

## 学习顺序
1. TCP 三次握手/状态机/拥塞 → 2. Socket API → 3. Reactor 模型 → 4. HTTP 状态机解析 → 5. 定时器/连接池/缓冲区 → 6. 优雅关闭与压测。

## 与 C++ / 项目入口
- 底层 I/O：[[04-Linux & System (Linux 系统)/03-IO Models (输入输出模型)/08-epoll Internals (epoll底层原理) ⭐/08a-epoll_create, epoll_ctl, epoll_wait (API详解)|epoll]]
- C++ 并发：[[03-C++ Programming (编程语言)/05-Concurrency Programming (并发编程)/05-Thread Pool Implementation (线程池手写) ⭐|线程池]]
- 项目：[[12-Backend Projects (后端项目)/02-URL Shortener|URL Shortener]]
