

> 完成时间：2026-05-04 
> 对应 tag：v0.1 
> 状态：已完成 ✅

> 版本快照：[webserver-cpp-v0.1](https://github.com/ziyan-cs/webserver-cpp/blob/f2a2f3f022a5ad0c1020350cb0f1a23c1d0f1b55/README.md)

> 开发环境见 [00-overview (总览)](/09-My%20Projects/webserver-cpp/00-overview%20(总览).md)

## 实现概述

这是项目奠基版本，目标是用 C++ 将 TCP Socket 理论落地，同时搭好后续模块化扩展的框架。

1. 以 [《TCP/IP 网络编程》](/09-My%20Projects/webserver-cpp/02-refs%20(参考资料).md) 为主要参考，结合 [个人笔记](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/04-Socket%20API%20(Socket编程接口)%20⭐/04a-socket,%20bind,%20listen,%20accept,%20connect%20(核心API).md) 手动实现核心逻辑
2. `CMakeLists.txt` 参考成熟工程示例，按多模块构建可扩展框架
3. 参照后端服务器通用设计范式，为并发模型、日志模块等后续演进预留接口

`main` 仅作逻辑入口，与业务代码解耦；当前阶段聚焦 `server` 模块，echo 回声功能作为内置逻辑验证通路。

**完整调用链：**
`socket()` → `setsockopt()` → `bind()` → `listen()` → `accept()` → `recv() / send()` → `close()` 

---

## server 模块设计

刻意模块化与职责分离，便于后续维护、扩展。

- `Config` 类：
	- 集中管理端口、IP、backlog、缓冲区大小，支持默认初始化与自定义覆盖
	- 消除魔法数字散落各处的问题

- `Server` 类：
	- 封装 Socket 完整生命周期，分层设计
	- 底层接口：`creatSocket`、`setSocketOpt`、`bindSocket`、`listenSocket`
	- 整合接口：`init`、`run`

- `main.cpp`：
	- 只调用初始化与启动接口，不含任何业务代码。

---

## 遇到的问题

### Telnet 逐字符输入时，`recv()` 每次只收到 1 个字节

**现象：** 输入 `Hello`，服务端 `recv()` 触发 5 次，每次仅接收 1 字节。

**排查过程**：起初怀疑是 `recv()` 调用逻辑有误，反复检查无果后，重新回看 TCP 协议性质才定位原因。

**根因：** 
- Telnet 默认以逐字符模式运行：：每输入 1 个字符，就将其对应的 1 个字节立即封装为 TCP 段发送。
- 本质是 **TCP 字节流无消息边界**：内核只负责传输字节流，发送端写入次数与接收端读取次数没有任何对应的关系。

**结论：** 是正确行为，不是 bug。

>延伸阅读 → [02e-粘包问题与解决](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02e-Sticky%20Packet%20Problem%20&%20Solutions%20(粘包问题与解决).md)

---

## 测试验证

- 宿主机通过 Telnet 连接虚拟机（端口 8080），跨局域网通信正常
- 输入 `Hello`，服务端 `recv()` 逐字节读取，同时客户端逐字节回显

<img src="/assets/file-20260506161744859.png" alt="终端运行截图" width="700">

---
- [a] n 
## 关键收获

1. 吃透 TCP Socket 完整调用链，明确每个系统调用的设计意图与边界
2. 理解 `SO_REUSEADDR` 的作用：避免服务重启时端口被 **TIME_WAIT** 状态占用
3. 对于 TCP 字节流无边界的认知，从 “看过理论” 到 “踩坑验证”
4. 掌握 CMake 多模块构建逻辑，理解根目录与子模块 `CMakeLists.txt` 的分工
5. 实践类封装与职责分离，体会分层设计对调试效率的实际影响

---

## 遗留与展望

**根本瓶颈：单线程阻塞 I/O**

`accept()`、`recv()`、`send()`  均为阻塞调用，同一时刻只能服务一个客户端， 后续连接必须等待当前客户端断开。

**为什么跳过 thread-per-client**

虽然来一个连接开一个线程符合直觉，但线程创建与销毁的开销随并发量线性增长，连接一多就会演变为资源消耗问题。跳过这一过渡阶段，直接演进到 I/O 多路复用更有工程价值。

**v0.2 目标**：引入 `epoll` ，实现 I/O 多路复用，从 阻塞单线程升级为事件驱动并发模型。