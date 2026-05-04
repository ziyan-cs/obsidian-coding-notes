
| 完成时间   | 2026-05-04 |
| ------ | ---------- |
| 对应 tag | v0.1       |
| 状态     | 已完成 ✅      |

# 做了什么

### 实现逻辑：

- 基于 POSIX Socket API 实现最基础的 TCP 回声功能
- 采用模块化设计，为向多线程并发的演进做基础管理

### 代码模块：

- `Config` 配置类：封装服务器基础配置
	
	- 包括端口、缓冲区大小、backlog、服务器 IP
	- 提供完整的 `get`/`set`/`print` 方法，支持默认初始化与自定义初始化
	
- `Server` 核心类：封装服务器生命周期
	
	- 提供统一的日志接口 `log()`，便于调试与后续扩展
	- 底层接口：`creatSocket()`、`setSocketOpt()`、`bindSocket()`、`listenSocket()`
	- 整合接口：`init()` 初始化、`run()` 启动、`stop()` 停止、
	
- `main.cpp`：只负责调用 `init()` 和 `run()` 的接口，业务与入口完全解耦

### 实现流程：

- 底层接口
	- `createSocket()`：创建 TCP socket 文件描述符
	- `setSocketOpt()`：设置地址复用等选项，避免端口占用问题
	- `bindSocket()`：绑定配置中的 IP 与端口
	- `listenSocket()`：开启监听，并设置 backlog 队列长度

- 整合接口
	- `init()`：按顺序调用上述底层接口，完成服务器初始化
	- `run()`：循环 `accept()` 客户端连接，调用 `handleClient()` 处理业务

## 遇到的问题

- **Telnet 逐字符发送，`read()` 每次接收 1 字节**
	
	- **现象：** 用 Telnet 输入 `Hello`，服务器端 `read()` 调用了 5 次，每次只读到一个字符
	- **原因：** **TCP 是字节流协议**，不存在 ”数据边界"，每输入一个字符发送一个 TCP 包
	- **结论：** `read()` 返回 1 字节不是 bug，是 TCP 的正确行为
	
	此问题的完整展开见 → [02-insights/tcp-byte-stream](../02-insights%20(知识)/tcp-byte-stream.md)

## 验证方式

- 实体机 **Telnet** 连接虚拟机，同一局域网内通信正常
- 服务器终端可以看见收发日志

## 该阶段所学

- POSIX Socket 编程的完整流程和每个系统调用的作用
- 第一次深化实践类封装
- TCP 字节流本质：接收方必须自己处理消息边界问题
- CMake 多模块构建：根目录 + 子目录 `CMakeLists.txt` 的组织

## 下一阶段

- 当前是单线程阻塞，`accept()` 和 `read()` 都会阻塞主线程，同一时刻只能服务一个客户端
- v0.2 目标：引入 epoll，改为事件驱动的非阻塞 I/O
