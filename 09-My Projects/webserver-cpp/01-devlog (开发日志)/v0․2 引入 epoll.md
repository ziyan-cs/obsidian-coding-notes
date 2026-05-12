

> 完成时间：2026-05-04 
> 对应 tag：v0.2 
> 状态：已完成 ✅

> 版本快照：

> 开发环境见 [00-overview (总览)](/09-My%20Projects/webserver-cpp/00-overview%20(总览).md)

## 实现概述

本版本针对 v0.1 单线程阻塞 I/O 的瓶颈，引入 epoll 将服务器升级为事件驱动的 I/O 多路复用模型，实现同时处理多个客户端连接。

**核心改动：**

1. 新增 net 模块，剥离事件驱动逻辑
2. Server 模块重构，适配 net 接口
3. 监听 socket 设置非阻塞

---

## net 模块设计

- `Channel` 类：
	- 把每个 fd 封装成频道对象，持有回调函数指针和 arg 参数，事件触发时自行调用

- `EventLoop` 类：
	- 封装 epoll 主循环，管理 Channel 的注册与事件分发

---

## 遇到的问题

---

## 测试验证

- 宿主机打开多终端通过 Telnet 连接虚拟机（端口 8080）
- 挨个窗口输入 `Hello`，服务端 `recv()` 逐字节读取，同时客户端逐字节回显

<img src="assets/file-20260512204508239.png" alt="终端运行截图" width="680">

---

## 关键收获

---

## 口述整理

v0.2针对上版本瓶颈引入 epoll，把阻塞模型升级成事件驱动。为什么选 epoll 而不是 select 或 poll —— select 和 poll 每次都要全量遍历 fd 列表，fd 数量越大开销越高，而且每次都要把所有 fd 从用户态拷贝到内核态，select 还有 1024 的连接上限。epoll 是被动收集有事件的 fd 到就绪数组，只遍历真正活跃的连接，效率随连接数增长不会显著下降。

触发模式我用的是 LT 条件触发，这是 epoll 默认模式，只要缓冲区有数据就持续通知，编程相对简单不容易出bug。ET 边缘触发只在状态变化时通知一次，必须配合非阻塞 socket 一次读完，实现复杂度更高。

代码结构上新增了 net 模块。Channel 类把每个fd封装成频道对象，持有回调函数，通过函数指针加 this 指针的方式注册 Server::onEvent，事件触发时 Channel 直接调用。EventLoop 封装 epoll 主循环，Channel 通过 enableReading 把自己注册进 epoll 监听表。监听 socket 在 run() 时设置了 O_NONBLOCK 非阻塞，accept_channel 负责处理新连接，每个新连接同样创建 Channel 注册进 loop。

回调注册用的是 C 风格函数指针，定义了 EventCallback 类型，Channel 内部存储函数指针和 arg 参数。注册时把 Server::onEvent 和 this 传进去，事件触发时 Channel 用自己的 fd 和事件类型直接调用，不需要知道调用的是谁的方法。

---

## 遗留与展望

**根本瓶颈：** 单线程事件循环，CPU 利用率受限

**v0.3 目标**：解析 HTTP/1.1 请求，响应静态页面
