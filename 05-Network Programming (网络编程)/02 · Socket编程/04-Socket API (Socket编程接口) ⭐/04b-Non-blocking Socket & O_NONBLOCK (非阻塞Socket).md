---
tags:
  - network
  - socket
---

> **核心考点**：阻塞 vs 非阻塞的行为差异、如何设置、如何正确处理 EAGAIN

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

> **为什么要循环读到 EAGAIN？**  
> epoll ET 模式（边缘触发）下，只在状态变化时通知一次。若一次 read() 没读完，后续不会再触发通知，数据就丢了。

---

## EAGAIN vs EWOULDBLOCK

- 两者在 Linux 上值相同（都是 11），可以认为是同一个
- 语义：操作不能立即完成，但不是错误，稍后重试即可