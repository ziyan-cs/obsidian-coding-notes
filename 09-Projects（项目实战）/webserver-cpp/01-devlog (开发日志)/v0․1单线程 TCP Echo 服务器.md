
> 完成时间：2026-05-04 
> 对应 tag：v0.1 
> 状态：已完成 ✅

## 实现概述

基于 POSIX Socket API 构建最小可用的 TCP echo 服务器，
作为后续并发模型演进的基线版本

**调用链：**
`socket()` → `setsockopt()` → `bind()` → `listen()` → `accept()` → `recv() / send()` → `close()` 

---

## 模块设计

- `Config` 类 —— 服务器参数的单一来源（SSOT）
	
	- 集中管理端口、IP、backlog、缓冲区大小等参数
	- 支持默认初始化与自定义覆盖
	- 消除魔法数字散落各处的问题
	
- `Server` 类 —— socket 生命周期的完整封装
	
	- 底层接口（`creatSocket`、`setSocketOpt`、`bindSocket`、`listenSocket`）
	- 整合接口（`init`、`run`、`stop`）分层设计，职责边界清晰
	- 内置统一日志接口 `log()`，便于后续替换为异步日志系统。
	
- `main.cpp` —— 只负责组装与启动，不含任何业务逻辑

---

## 遇到的问题

### Telnet 逐字符发送，`read()` 每次仅接收 1 字节

**现象：** 输入 `Hello`，服务器端 `read()` 被触发 5 次，每次返回 1 字节。

**根因：** 两层原因叠加：
- Telnet 默认以逐字符模式运行，每次按键立即发出一个 TCP segment
- 更本质的是 **TCP 是无边界字节流**——协议层不感知"消息"，
  发送侧的写入次数与接收侧的读取次数之间没有任何对应关系

**结论：** `read()` 返回 1 字节是完全正确的行为，不是 bug。
消息边界的界定责任在应用层协议（后续 HTTP 解析阶段将正式处理此问题）。

>完整展开 → [02-insights/tcp-byte-stream](../02-insights%20(知识)/tcp-byte-stream.md)

---

## 验证方式

- 宿主机通过 Telnet 连接虚拟机，跨局域网通信正常

![终端运行结果](assets/file-20260504151645377.png)

---

## 关键收获

- 掌握 POSIX Socket 编程的完整系统调用链及各调用的语义
- 理解 `SO_REUSEADDR` 的作用：避免服务重启时端口被 TIME_WAIT 状态占用
- TCP 字节流无边界这一本质认知，是理解粘包 / 半包问题的前提
- CMake 子目录构建模式：根目录与模块级 `CMakeLists.txt` 的分工方式

---

## 遗留与展望

当前架构的根本瓶颈：`accept()` 与 `read()` 均为阻塞调用，主线程串行处理，
同一时刻只能服务一个客户端连接。

**v0.2 目标：** 引入 `epoll`，改造为事件驱动的 I/O 多路复用模型。