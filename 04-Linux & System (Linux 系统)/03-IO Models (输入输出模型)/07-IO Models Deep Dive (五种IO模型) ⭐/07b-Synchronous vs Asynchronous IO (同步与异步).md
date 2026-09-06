---
tags:
  - linux/io
status: 🌱
---

# Synchronous vs Asynchronous IO — 同步与异步

> [!important] **核心考点**：同步 IO 与异步 IO 的本质区别、异步 IO 的实现方式、IO 模型的分类维度

## 同步 vs 异步的本质

区分标准：**数据拷贝（内核→用户）由谁完成、是否需要等待**。

- **同步 IO**：用户线程等待或轮询数据就绪后，自己调用 read 拷贝数据——**拷贝过程阻塞**
- **异步 IO**：用户发起请求立即返回，内核完成数据拷贝后通知用户——**整个过程不阻塞**

## 五种 IO 模型总览

| 模型 | 就绪阶段 | 拷贝阶段 | 归类 |
|------|---------|---------|------|
| 阻塞 IO | 等待阻塞 | read 阻塞 | 同步 |
| 非阻塞 IO | 轮询不阻塞 | read 阻塞 | 同步 |
| IO 多路复用 | select/epoll 阻塞 | read 阻塞 | 同步 |
| 信号驱动 IO | 信号通知 | read 阻塞 | 同步 |
| 异步 IO (AIO) | 内核完成 | 内核完成 | **异步** |

**关键理解：** 前四种模型的数据拷贝都由用户线程完成，因此都是同步 IO。只有 AIO 是内核做完全部工作后才通知。

## 异步 IO（AIO）

```c
#include <aio.h>

struct aiocb cb;
cb.aio_fildes = fd;
cb.aio_buf = buf;
cb.aio_nbytes = sizeof(buf);
aio_read(&cb);   // 发起异步读，立即返回

// 做其他事...

while (aio_error(&cb) == EINPROGRESS) {
    // 继续其他工作
}
ssize_t ret = aio_return(&cb);
```

## 工程选型

实际高并发服务器（Nginx、Redis、Netty）几乎全部使用 **IO 多路复用 + 非阻塞 IO**，而非 AIO。原因：

1. Linux AIO 对网络 IO 支持不如 epoll 成熟
2. 事件循环 + 回调模型已足够高效
3. 同步模型更易理解和调试

> [!tip]- **工程要点**：面试中"阻塞/非阻塞"描述**调用行为**，"同步/异步"描述**拷贝主体**。大部分高性能网络框架是"非阻塞同步 IO + 多路复用"组合。

同步与异步 IO 详解见 → [Blocking vs Non-blocking IO (阻塞与非阻塞)](/04-Linux%20&%20System%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07a-Blocking%20vs%20Non-blocking%20IO%20(阻塞与非阻塞).md) · [IO Multiplexing (多路复用对比)](/04-Linux%20&%20System%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07c-IO%20Multiplexing：%20select,%20poll,%20epoll%20(多路复用对比).md)
