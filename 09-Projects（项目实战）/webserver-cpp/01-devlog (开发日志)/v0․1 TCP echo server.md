
> 完成时间：2026-05-04 
> 对应 tag：v0.1 
> 状态：已完成 ✅

> 版本快照：[webserver-cpp-v0.1](https://github.com/ziyan-cs/webserver-cpp/tree/ecf1d0356dc558bcb81dd8dd4ec19684879ba130)

> 开发环境见 [00-overview (总览)](/09-Projects（项目实战）/webserver-cpp/00-overview%20(总览).md)

## 实现概述

作为项目的奠基版本，核心实现目标是将 TCP Socket 理论落地。

我以《TCP/IP 网络编程》为核心参考，结合资料梳理实现流程，echo 回声功能作为额外功能扩展

CMake 多模块构建思想参考 AI 指导示例，根据多模块分步实现功能，
对于项目整体框架有清晰认知。

当前目标是完善 server 模块，

**完整调用链：**
`socket()` → `setsockopt()` → `bind()` → `listen()` → `accept()` → `recv() / send()` → `close()` 

---

## 模块设计

刻意实践模块化与职责分离，便于后续维护、扩展

- `Config` 类 —— 服务器配置信息
	
	- 集中管理端口、IP、backlog、缓冲区大小等参数
	- 支持默认初始化与自定义覆盖
	- 消除魔法数字散落各处的问题
	
- `Server` 类 —— Socket 生命周期的完整封装
	
	- 分层封装 Socket 生命周期
	- 底层接口（`creatSocket`、`setSocketOpt`、`bindSocket`、`listenSocket`）
	- 整合接口（`init`、`run`、`stop`）分层设计，职责边界清晰
	- 内置统一日志接口 `log()`，便于后续替换为异步日志系统
	
- `main.cpp` —— 仅保留入口逻辑，实现入口与业务解耦

---

## 遇到的问题

### Telnet 逐字符发送，`read()` 每次只收到 1 个字节

**现象：** 客户端输入字符串触发服务器端 `read()` 多次读取，每次仅接收 1 字节

**核心认知：** 
- Telnet 默认以逐字符模式运行，每次按键立即发出一个 TCP segment
- 本质是 **TCP 字节流** 无消息边界，内核自动处理接收，发送与接收次数无对应关系
  发送侧的写入次数与接收侧的读取次数之间没有任何对应关系

**结论：** `read()` 返回 1 字节是完全正确的行为，不是 bug。
消息边界的界定责任在应用层协议（后续 HTTP 解析阶段将正式处理此问题）。

---

## 测试验证

- 宿主机通过 Telnet 连接虚拟机（端口 8080），跨局域网通信
- 输入 `Hello`，服务器端 `read()` 触发 5 次，每次仅收到 1 字节

![终端运行截图](/assets/{date-YYYYMMDDHHmmss}.png)

>此为 TCP 字节流无消息边界的直接体现，详见 → [02-insights/tcp-byte-stream](09-Projects（项目实战）/webserver-cpp/02-insights%20(知识)/tcp-byte-stream.md)

---

## 关键收获

1. 吃透 TCP Socket 全流程调用，明确设计意图和核心作用
2. 理解 `SO_REUSEADDR` 套接字选项，解决 **TIME_WAIT** 端口占用问题
3. 对于 TCP 字节流无边界的认知，从 “理论了解” 到 “实践经历”
4. 掌握 CMake 多模块构建逻辑，理解根目录与子模块的分工
5. 实践类封装、职责分离思想，体会工程化代码的可维护性与可扩展性。

---

## 遗留与展望

单线程阻塞 I/O 模型，仅能同时服务一个客户端，功能十分受限

多线程阻塞 I/O 的处理办法虽然在

**v0.2 目标：** 引入 `epoll` 实现 I/O 多路复用，升级为事件驱动的并发模型，解决单线程阻塞模型的瓶颈