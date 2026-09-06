---
tags:
  - linux/io
status: 🌱
---

# Blocking vs Non-blocking IO — 阻塞与非阻塞

> [!important] **核心考点**：阻塞 IO 与非阻塞 IO 的核心区别、同步等待 vs 立即返回、系统调用行为差异

## 阻塞 IO

**行为：** 系统调用直到操作完成才返回，调用线程在此期间挂起等待。

```c
ssize_t n = read(fd, buf, sizeof(buf));
// 线程在此等待，直到有数据可读或出错
```

**典型场景：** 默认的 socket read/write、普通文件 read/write。

**问题：** 一个线程只能处理一个 IO 操作。多线程处理多连接时，线程数随连接数增长，上下文切换开销大。

## 非阻塞 IO

**行为：** 系统调用立即返回，操作无法完成时返回 `EAGAIN` 或 `EWOULDBLOCK`。

```c
fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

ssize_t n = read(fd, buf, sizeof(buf));
if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    // 当前无数据，稍后重试
}
```

**典型场景：** 配合 epoll 使用，单线程管理大量 fd。

## 核心对比

| 特性 | 阻塞 IO | 非阻塞 IO |
|------|--------|----------|
| 调用返回时机 | 操作完成后 | 立即返回 |
| 线程状态 | 挂起等待 | 继续执行 |
| CPU 利用率 | 等待时不占 CPU | 轮询消耗 CPU |
| 编程复杂度 | 简单 | 需处理 EAGAIN |

## 本质理解

阻塞 vs 非阻塞描述的是**数据未就绪时系统调用的行为**：
- 阻塞：**等**数据就绪才返回
- 非阻塞：**立即返回**，告诉调用者"还没好"

两者都是**同步 IO**——数据从内核到用户缓冲区的拷贝由用户线程自己完成，需要等待拷贝结束。

> [!tip]- **工程要点**：非阻塞 IO 本身不提升性能，它的价值是和 IO 多路复用配合时让单个线程管理大量 fd。纯轮询非阻塞 IO（一直 read 检查 EAGAIN）反而更浪费 CPU。

阻塞与非阻塞 IO 详解见 → [Synchronous vs Asynchronous IO (同步与异步)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07b-Synchronous%20vs%20Asynchronous%20IO%20(同步与异步).md) · [IO Multiplexing (多路复用对比)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07c-IO%20Multiplexing：%20select,%20poll,%20epoll%20(多路复用对比).md)
