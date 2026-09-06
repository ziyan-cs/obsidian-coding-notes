---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 00-Runtime and Network MOC (运行时与网络导航)

> [!abstract] 一句话结论：先理解 Linux 如何运行一个服务，再理解协议如何传递字节，最后掌握服务器如何把连接、事件与业务组织为可演进的系统。

## 学习顺序

1. **Linux Runtime（Linux 运行时）**：进程、内存、文件描述符、I/O、调度与观测。
2. **Network Protocols（网络协议）**：TCP、HTTP、TLS、DNS 与 RPC 的边界。
3. **Server Networking（服务器网络编程）**：socket、非阻塞 I/O、Reactor、缓冲区与优雅关闭。

## 使用方式

- 每读完一篇，能用“问题—机制—代价—场景”四句复述。
- 遇到服务端故障时，先从 Linux 资源与连接状态定位，再下钻到协议和代码。
- 与 C++ 后端、数据库和分布式主题交叉学习，不把网络当成孤立八股。

## 本模块出口

- 能解释一次请求从监听 socket 到业务处理、再到响应写回的路径。
- 能为一个 C++ 或 Go 服务选择合理的并发模型、超时策略和观测指标。
