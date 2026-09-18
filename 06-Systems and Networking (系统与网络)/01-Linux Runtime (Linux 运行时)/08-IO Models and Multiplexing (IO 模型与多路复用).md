---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习目标：区分阻塞、非阻塞、同步、异步与 IO 多路复用，能沿事件循环说明状态变化。

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 阻塞/非阻塞描述一次系统调用是否等待；同步/异步描述完成通知与数据传递方式；I/O multiplexing 则让一个线程等待多个 fd 的就绪事件。epoll 是 Linux 的就绪通知接口，不是 mmap 零拷贝，也不替你读写业务数据。

# 事件循环边界

```text
epoll_wait 返回就绪 fd → 非阻塞 read/write 尽量推进 → 处理 EAGAIN / EOF / error
                         → 不在 I/O 线程执行长 CPU 或阻塞业务
```

## 选择与误区

- LT 更易写对；ET 要求一次读/写到 `EAGAIN`，否则可能错过后续通知。
- 一个 fd 就绪不代表完整业务报文已到达；字节流仍需 buffer 和 framing。
- epoll 的优势与活跃 fd 数相关，不能脱离工作负载宣称固定倍数。

> [!question]- 自测：先回答再展开
> 1. 为什么 ET 模式下必须循环读取到 `EAGAIN`？
> 2. select/poll/epoll 分别如何保存与遍历关注的 fd？
> 3. 哪些工作不应放在 reactor I/O 线程？

# Blocking and Nonblocking IO (阻塞与非阻塞 I O)

> [!note] 本节重点：阻塞 IO 与非阻塞 IO 的核心区别、同步等待 vs 立即返回、系统调用行为差异

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

阻塞与非阻塞 IO 详解见 → Synchronous vs Asynchronous IO (同步与异步) · IO Multiplexing (多路复用对比)

---

# Synchronous and Asynchronous IO (同步与异步 I O)

> [!note] 本节重点：同步 IO 与异步 IO 的本质区别、异步 IO 的实现方式、IO 模型的分类维度

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

同步与异步 IO 详解见 → Blocking vs Non-blocking IO (阻塞与非阻塞) · IO Multiplexing (多路复用对比)

---

# IO Multiplexing (I O 多路复用)

> [!note] 本节重点： select/poll/epoll 多路复用技术对比、文件描述符上限、触发模式与性能差异

## select

```c
fd_set rfds;
FD_ZERO(&rfds);
FD_SET(fd, &rfds);

struct timeval tv = {5, 0};  // 5s 超时
int ret = select(fd + 1, &rfds, NULL, NULL, &tv);
if (FD_ISSET(fd, &rfds)) {
    // 可读
}
```

**缺点：**
- 单个进程监听 fd 上限 1024（FD_SETSIZE）
- 每次调用需将 fd_set 从用户态拷贝到内核态
- 内核遍历所有 fd 检查事件，O(n)
- 修改后的 fd_set 需要重新初始化

## poll

```c
struct pollfd fds[1];
fds[0].fd = fd;
fds[0].events = POLLIN;

int ret = poll(fds, 1, 5000);  // 5s 超时
if (fds[0].revents & POLLIN) {
    // 可读
}
```

**改进：**
- 没有 1024 上限
- 用 pollfd 数组管理，更灵活

**仍存在的问题：**
- 每次调用仍要拷贝所有 fd 到内核
- 内核仍遍历全部 fd，O(n)
- 大量 fd 时性能下降明显
