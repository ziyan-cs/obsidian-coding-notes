---
tags:
  - network
  - socket
---

> **核心考点**：主从 Reactor 多线程模型、one loop per thread 设计、Nginx/Netty/Redis 等实际应用
> 代表：Nginx、Netty、Muduo、Node.js cluster 模式  
> 最成熟的高性能网络服务器架构

# 模型结构

```txt
┌─────────────────────────────────────────────────────────────┐
│  主线程（Main Reactor）                                       │
│  ┌──────────────────┐                                       │
│  │  Main Reactor    │  只负责 accept 新连接                   │
│  │  (epoll on       │                                       │
│  │  listening fd)   │                                       │
│  └────────┬─────────┘                                       │
└───────────┼─────────────────────────────────────────────────┘
            │ 将新连接分发给某个 Sub Reactor
     ┌──────┼───────┬──────────────┐
     ↓      ↓       ↓              ↓
┌─────────┐ ┌─────────┐     ┌─────────┐
│Sub      │ │Sub      │ ... │Sub      │   每个 Sub Reactor
│Reactor 1│ │Reactor 2│     │Reactor N│   运行在独立线程
│(epoll)  │ │(epoll)  │     │(epoll)  │   负责所分配连接的
│         │ │         │     │         │   全部 I/O 事件
│Handler  │ │Handler  │     │Handler  │
│(read/   │ │(read/   │     │(read/   │
│process/ │ │process/ │     │process/ │
│write)   │ │write)   │     │write)   │
└─────────┘ └─────────┘     └─────────┘
   线程1       线程2              线程N
                    ↕（可选：业务复杂时再加线程池）
              ┌───────────┐
              │  线程池    │
              │（CPU密集型）│
              └───────────┘
```

# "One Loop Per Thread" 的含义

每个 Sub Reactor 是一个独立的 **event loop**，运行在自己的线程中，负责管理一批连接的所有 I/O 操作。线程之间的连接互不干扰，**天然无锁**。

# 工作流程

1. Main Reactor 只监听 listening fd，`accept()` 新连接
2. 通过负载均衡策略（轮询、最少连接）将新连接的 fd 分配给某个 Sub Reactor
3. 各 Sub Reactor 在自己的线程中独立运行 event loop，处理分配给它的所有连接的读写
4. 业务逻辑若复杂，可再投递给线程池处理

# 核心优势

|特性|说明|
|---|---|
|accept 不成瓶颈|Main Reactor 专职 accept，不处理 I/O|
|I/O 充分并行|N 个 Sub Reactor 并行处理，充分利用多核|
|无锁设计|同一连接的所有操作在同一线程，无需加锁|
|线性扩展|Sub Reactor 数量通常 = CPU 核数|

# 与前两种模型的对比

|模型|线程数|accept|I/O|业务处理|适用场景|
|---|---|---|---|---|---|
|单 Reactor 单线程|1|主线程|主线程|主线程|轻量服务（Redis）|
|单 Reactor 多线程|1 + N|主线程|主线程|线程池|中等并发|
|主从 Reactor 多线程|1 + M + N|主线程|M 个子线程|线程池（可选）|高并发生产环境|

---

# Proactor 模式（补充对比）

Reactor 和 Proactor 的根本区别在于 **I/O 操作由谁来执行**：

|        | Reactor（同步 I/O）           | Proactor（异步 I/O）            |
| ------ | ------------------------- | --------------------------- |
| I/O 操作 | 应用程序自己调用 read/write       | 操作系统完成 I/O，再通知应用            |
| 事件通知时机 | "fd 可读/可写了，你来读"           | "数据已经读好了，放在这里"              |
| 代表实现   | Linux epoll（配合非阻塞 socket） | Windows IOCP、Linux io_uring |
| 编程复杂度  | 较低                        | 较高                          |

> Linux 上 epoll 本质是 Reactor 模型。真正的 Proactor 在 Linux 上由 **io_uring**（内核 5.1+）实现，性能极高，是现代高性能服务器的新趋势。

---

# 实际框架对应

| 框架/项目       | 模型                                                              |
| ----------- | --------------------------------------------------------------- |
| Redis（6.0前） | 单 Reactor 单线程                                                   |
| Nginx       | 主从 Reactor（master + worker，one loop per worker）                 |
| Netty       | 主从 Reactor 多线程（Boss Group + Worker Group）                       |
| Muduo（陈硕）   | 主从 Reactor，one loop per thread                                  |
| Node.js     | 单 Reactor 单线程（libuv event loop）+ 线程池（libuv threadpool，处理文件 I/O） |
| Tokio（Rust） | 主从 Reactor 多线程（work-stealing 调度）                                |
