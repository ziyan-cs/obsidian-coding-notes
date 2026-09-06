---
tags:
  - network/socket
status: 🌱
---

# Non-blocking Socket & O_NONBLOCK — 非阻塞Socket

> [!important] **核心考点**：阻塞 vs 非阻塞的行为差异、如何设置、如何正确处理 EAGAIN

## 阻塞 vs 非阻塞

|               | 阻塞模式        | 非阻塞模式                       |
| ------------- | ----------- | --------------------------- |
| read( ) 无数据   | 挂起线程，等待数据到来 | 立即返回 -1，errno = EAGAIN      |
| write( ) 缓冲区满 | 挂起线程，等待缓冲区空 | 立即返回 -1，errno = EAGAIN      |
| accept( ) 无连接 | 挂起线程，等待新连接  | 立即返回 -1，errno = EAGAIN      |
| connect( )    | 阻塞直到握手完成    | 立即返回 -1，errno = EINPROGRESS |

---

## 设置非阻塞

```cpp
// 方法一：创建时直接设置（推荐）
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

// 方法二：通过 fcntl 修改已有 fd
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

---

## 非阻塞的正确使用模式

非阻塞 socket 配合 **I/O 多路复用**（select / poll / epoll）使用：

```cpp
while (true) {
    epoll_wait() → 获取就绪的 fd 列表
    for each 就绪的 fd:
        if 可读: read() 直到 EAGAIN（读完所有数据）
        if 可写: write() 直到 EAGAIN 或数据发完
        if 新连接: accept() 直到 EAGAIN（接受所有新连接）
}
```

> **为什么 ET 要循环读到 EAGAIN？**  
> epoll ET 模式只在“未就绪 → 就绪”变化时通知。若一次没有读到 `EAGAIN`，应用可能不再收到新的边沿通知，导致连接长期积压。LT 下通常仍应尽量批量读写以提高吞吐，但不必把“读到 EAGAIN”误解为唯一正确策略。

---

## EAGAIN vs EWOULDBLOCK

- POSIX 允许两者相同；Linux 上通常相同，但可移植代码应同时处理两者
- 语义：操作不能立即完成，但不是错误，稍后重试即可

## 30 秒回答

`O_NONBLOCK` 只改变系统调用在暂时不能推进时的返回方式，不会自动提供事件通知或解决部分读写。它通常配合 epoll 使用：读到 `EAGAIN` 表示当前内核缓冲区已读空，写到 `EAGAIN` 表示要等待可写事件；同时仍要处理 EOF、`EINTR` 和真正错误。

---

Socket API 基础见 → [socket, bind, listen, accept, connect (核心API)](</05-Network%20Programming%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04a-socket,%20bind,%20listen,%20accept,%20connect%20(核心API).md>) · 套接字选项见 → [SO_REUSEADDR & SO_KEEPALIVE (套接字选项)](</05-Network%20Programming%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04c-SO_REUSEADDR%20&%20SO_KEEPALIVE%20(套接字选项).md>)
