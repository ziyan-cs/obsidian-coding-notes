#linux #socket #system-programming #network #tcp #udp

## ⚡ TL;DR（快速决策）

- 套接字编程的核心是：**通过操作系统提供的 Socket 接口建立进程与网络之间的通信通道**
- 它是 Linux 网络系统编程的基础入口
- 若只抓主线，应优先记住：
    - 服务端：`socket -> bind -> listen -> accept`
    - 客户端：`socket -> connect`
    - 收发数据：`send/recv` 或 `read/write`

## 🧩 Core Idea（核心本质）

- Socket 编程不是直接操作网络协议报文，而是通过内核网络栈进行通信
- 一句话理解：
    - **Socket 是用户程序访问网络能力的系统调用接口。**

## 🏗️ 核心流程

1. 创建 Socket

- `socket`

1. 绑定地址

- `bind`

1. 建立监听或主动连接

- 服务端：`listen`、`accept`
- 客户端：`connect`

1. 数据收发

- `send/recv`、`read/write`

1. 关闭连接

- `close`

## 🔧 Usage Patterns（可复用理解框架）

1. 区分 TCP 与 UDP Socket

- TCP：面向连接
- UDP：面向报文

1. 用“文件描述符”理解 Socket

- 在 Linux 中，Socket 也是一种可读写的 fd

1. 用“阻塞与非阻塞”理解通信行为

- 系统调用可能等待连接建立或数据就绪

## ⚠️ Pitfalls（高频错误）

- 不理解监听 Socket 和已连接 Socket 的区别
- 忽略错误处理和返回值检查
- 忽略粘包、半包、阻塞、EAGAIN 等实际问题

## 🚀 Performance / Tips（理解深化）

- Socket 编程是 Linux 系统编程和网络协议知识的连接点
- 真正写高并发服务时，还需要结合 I/O 多路复用、线程池、缓冲区管理等机制

## 🧪 Common Scenarios（常见使用场景）

- 客户端 / 服务端程序
- 网络代理
- 聊天程序
- HTTP/TCP 服务开发

## 🧾 Minimal Template（最小理解模板）

```
服务端：socket -> bind -> listen -> accept -> send/recv
客户端：socket -> connect -> send/recv
```

## 📌 One-liner Summary（一句话总结）

- **套接字编程的核心，是通过 Linux 提供的 Socket 接口在进程中建立和使用网络通信通道。**