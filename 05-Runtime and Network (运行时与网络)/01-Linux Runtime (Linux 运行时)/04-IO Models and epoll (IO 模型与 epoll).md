---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 04-IO Models and epoll (IO 模型与 epoll)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 14-Blocking and Nonblocking IO (阻塞与非阻塞 I O)

> [!abstract] 核心考点：阻塞 IO 与非阻塞 IO 的核心区别、同步等待 vs 立即返回、系统调用行为差异

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

---

## 15-Synchronous and Asynchronous IO (同步与异步 I O)

> [!abstract] 核心考点：同步 IO 与异步 IO 的本质区别、异步 IO 的实现方式、IO 模型的分类维度

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

同步与异步 IO 详解见 → [Blocking vs Non-blocking IO (阻塞与非阻塞)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07a-Blocking%20vs%20Non-blocking%20IO%20(阻塞与非阻塞).md) · [IO Multiplexing (多路复用对比)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/07-IO%20Models%20Deep%20Dive%20(五种IO模型)%20⭐/07c-IO%20Multiplexing：%20select,%20poll,%20epoll%20(多路复用对比).md)

---

## 16-IO Multiplexing (I O 多路复用)

> [!abstract] 核心考点：> select/poll/epoll 多路复用技术对比、文件描述符上限、触发模式与性能差异

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

## epoll（Linux 专属）

```c
// 创建 epoll 实例
int epfd = epoll_create1(0);

// 添加 fd 到监听集合
struct epoll_event ev;
ev.events = EPOLLIN | EPOLLET;  // 边缘触发
ev.data.fd = fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

// 等待事件
struct epoll_event events[128];
int n = epoll_wait(epfd, events, 128, -1);
for (int i = 0; i < n; i++) {
    // 只处理就绪的 fd
}
```

**核心优势：**
- **O(1) 事件通知**：只返回就绪的 fd，不遍历所有
- **关注集合 + 就绪集合**：内核维护关注集合与就绪集合；epoll_wait 将就绪事件返回到用户提供的 events 缓冲区（epoll 不以 mmap 共享事件区作为其机制）
- **红黑树管理**：epoll_ctl 用红黑树维护 fd 集合，增删改 O(log n)
- **就绪链表**：内核把就绪的 fd 链入就绪链表，epoll_wait 直接读取
- **无上限**：受系统最大文件数限制（cat /proc/sys/fs/file-max）

## 三者的详细对比

| 特性 | select | poll | epoll |
|------|--------|------|-------|
| 底层结构 | 位数组 | pollfd 数组 | 红黑树 + 就绪链表 |
| 最大 fd 数 | 1024 | 无上限 | 无上限 |
| 遍历方式 | 全部遍历 O(n) | 全部遍历 O(n) | 直接返回就绪 O(k) |
| 数据拷贝 | 每次拷贝全部 | 每次拷贝全部 | 仅就绪事件拷入用户 events 缓冲区 |
| 触发模式 | LT | LT | LT + ET |
| 平台 | 几乎所有平台 | 几乎所有平台 | Linux 2.6+ |
| 修改 fd | 重设 fd_set | 重设 pollfd | epoll_ctl 增量更新 |

## 选型建议

- **select**：仅用于兼容性要求极高的场景
- **poll**：fd 数量少（几百以内）且追求可移植性时可用
- **epoll**：Linux 高并发服务器的首选，支持百万级 fd

> [!tip]- **工程要点**
> epoll 的高效不在于"快"，而在于"不浪费"——它只通知你真正有事件的 fd，避免了遍历全部 fd 的开销。当连接数少且活跃度高时，select/poll 和 epoll 性能差异不大；连接数多但活跃度低时（如 C10K 问题），epoll 优势明显。

---

epoll 底层原理详解 → [epoll API详解](../08-epoll%20Internals%20(epoll底层原理)%20⭐/08a-epoll_create,%20epoll_ctl,%20epoll_wait%20(API详解).md) · [LT vs ET](../08-epoll%20Internals%20(epoll底层原理)%20⭐/08b-Level%20Trigger%20vs%20Edge%20Trigger：%20LT⧸ET%20(触发模式).md)
- [System Administration Basics (系统管理基础)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/03-System%20Administration%20Basics%20(系统管理基础).md)

---

## 17-epoll API (epoll API)

> [!abstract] 核心考点：> epoll_create/epoll_ctl/epoll_wait 核心 API、红黑树管理、事件就绪队列

> [!warning] 示例循环省略了生产级错误处理
> `accept`、`read`、`write` 都可能返回 `EAGAIN`、`EINTR` 或错误。ET 模式还必须循环读/accept 到 `EAGAIN`；不要把下面的最小骨架直接当成完整服务器。

## epoll_create

```c
#include <sys/epoll.h>

int epfd = epoll_create(int size);   // size > 0，2.6.8 后忽略
int epfd = epoll_create1(int flags); // 推荐：EPOLL_CLOEXEC
```

**底层结构：** 内核创建 `eventpoll` 实例，包含：
- **红黑树（rbr）**：存储所有注册的 fd 及事件
- **就绪链表（rdllist）**：存储有事件发生的 fd

## epoll_ctl

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// op：EPOLL_CTL_ADD / EPOLL_CTL_MOD / EPOLL_CTL_DEL

struct epoll_event {
    uint32_t     events;  // EPOLLIN / EPOLLOUT / EPOLLET 等
    epoll_data_t data;    // 用户数据
};

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;
```

**常用事件：** EPOLLIN（可读）、EPOLLOUT（可写）、EPOLLERR（错误）、EPOLLET（边缘触发）、EPOLLONESHOT（一次性）。

## epoll_wait

```c
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
// timeout: -1 阻塞, 0 立即返回, >0 超时毫秒

// 使用示例
struct epoll_event events[128];
int nfds = epoll_wait(epfd, events, 128, -1);

for (int i = 0; i < nfds; i++) {
    if (events[i].events & EPOLLIN) {
        // 处理可读事件
    }
}
```

## 典型事件循环

```c
int epfd = epoll_create1(EPOLL_CLOEXEC);

struct epoll_event ev;
ev.events = EPOLLIN;
ev.data.fd = listen_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

while (1) {
    int n = epoll_wait(epfd, events, 128, -1);
    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == listen_fd) {
            int conn = accept(listen_fd, NULL, NULL);
            ev.data.fd = conn;
            epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &ev);
        } else {
            // 处理数据
        }
    }
}
close(epfd);
```

> [!tip]- **工程要点**：`epoll_event.data` 是联合体，推荐用 `data.ptr` 指向连接对象（struct），避免再通过 fd 做映射查找。

## 30 秒回答 / 自测

- **30 秒回答**：`epoll_create1(EPOLL_CLOEXEC)` 建实例（内核红黑树存 fd + 就绪链表）；`epoll_ctl` 增删改 fd 与事件；`epoll_wait` 阻塞取就绪事件，从 `events[i].data` 取用户数据。
- **常见误区**：用 `data.fd` 存 fd 后还要回查连接对象（应直接用 `data.ptr`）；多线程共享 epfd 时漏设 `EPOLLONESHOT`，导致同一事件被多线程重复处理。
- **自测**：1) `epoll_event.data` 为什么是 union？ 2) `epoll_create` 的 size 参数为什么被忽略？

epoll API 详解见 → [Level Trigger vs Edge Trigger (触发模式)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08b-Level%20Trigger%20vs%20Edge%20Trigger：%20LT⧸ET%20(触发模式).md) · [epoll vs select (底层实现对比)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08c-epoll%20vs%20select：%20Red-Black%20Tree%20&%20Event%20Queue%20(底层实现对比).md)

---

## 18-Level and Edge Triggering (水平与边缘触发)

> [!abstract] 核心考点：水平触发 LT 与边缘触发 ET 的区别、ET 模式需循环读取、epoll 高效根源

## 水平触发 LT（Level-Triggered）

**默认模式。** 只要 fd 还有数据可读，每次 `epoll_wait` 都会返回该 fd。

```c
ev.events = EPOLLIN;  // 默认为 LT
```

**行为：** 缓冲区有 100 字节，read 只读了 50 → 下次 epoll_wait 仍然通知可读。

**优点：**
- 编程简单，不容易漏事件
- 不必一次读完所有数据
- 兼容性好，select/poll 都是 LT

**缺点：**
- 同一个 fd 可能被重复通知，多线程下需注意

## 边缘触发 ET（Edge-Triggered）

**状态变化时触发一次。** 只有当 fd 从"无数据可读"变为"有数据可读"时才通知。如果一次没读完，剩余数据不会再触发通知（除非有新的数据到达）。

```c
ev.events = EPOLLIN | EPOLLET;  // 边缘触发
```

**行为：** 缓冲区有 100 字节，read 只读了 50 → 剩余 50 字节不会再触发 epoll_wait，直到有新数据写入触发新的事件。

**ET 模式的使用要求：**

```c
// 1. fd 必须设置为非阻塞
fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

// 2. 读操作必须循环到 EAGAIN（确保全部读完）
while (1) {
    char buf[4096];
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n > 0) {
        // 处理数据
    } else if (n == 0) {
        close(fd);  // 对端关闭
        break;
    } else if (errno == EAGAIN) {
        break;  // 数据已全部读尽
    } else {
        // 真正出错
        break;
    }
}
```

## LT vs ET 对比

| 特性 | LT | ET |
|------|-----|-----|
| 通知条件 | 只要有数据就通知 | 只有状态变化时通知 |
| 必须一次读完 | 否 | 是（否则漏数据） |
| fd 必须非阻塞 | 推荐 | **必须** |
| 编程复杂度 | 低 | 高（需循环 + EAGAIN 判断） |
| epoll_wait 调用次数 | 可能更多（重复通知） | 更少（变化才通知） |
| 性能 | 略低 | 略高 |

## ET 为什么高效

- 避免**同 fd 被反复唤醒**：LT 模式下，大量数据分批读取时，epoll_wait 每次都会返回同一个 fd，造成重复的事件循环
- 减少**用户态/内核态切换**：ET 一次事件驱动用户读完所有数据，事件通知次数更少
- 配合非阻塞 IO，单次系统调用批量处理数据

## 实际选型

- **Nginx**：使用 ET 模式（追求极致性能）
- **Redis**：使用 LT 模式（追求简单、事件驱动明确）
- **一般项目**：推荐 LT 模式，编程简单不易出错，性能差异通常可接受

> [!tip]- **工程要点**：ET 模式的核心问题是"怎么知道数据读完了"——只能通过 read 返回 EAGAIN 判断。这就要求 fd 必须是非阻塞的，否则最后一次无数据可读时 read 会阻塞线程。如果你发现 ET 模式下某些事件没触发，大概率是循环读逻辑不对或忘记设置非阻塞。

epoll 触发模式见 → [epoll_create, epoll_ctl, epoll_wait (API详解)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08a-epoll_create,%20epoll_ctl,%20epoll_wait%20(API详解).md) · [epoll vs select (底层实现对比)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/03-IO%20Models%20(输入输出模型)/08-epoll%20Internals%20(epoll底层原理)%20⭐/08c-epoll%20vs%20select：%20Red-Black%20Tree%20&%20Event%20Queue%20(底层实现对比).md)

---

## 19-epoll Internals (epoll 底层实现)

> [!abstract] 核心考点：> epoll 红黑树+就绪队列 vs select 轮询、O(1) 事件通知 vs O(n) 扫描

## select 的局限性

select 是最早的 IO 多路复用接口，核心缺陷源于其数据结构：

```
select 的 fd_set 是位图（bitmap），最多 1024 个 fd
每次调用需从用户态拷贝全部 fd_set 到内核态
内核线性扫描所有 fd 检查就绪状态
返回时内核改写 fd_set，用户态需重新遍历所有 fd
```

**select 的 O(n) 三要素：**
- 每次调用 O(n) 拷贝 fd_set 到内核
- 内核 O(n) 扫描 fd 状态
- 返回后用户 O(n) 遍历 fd_set 检查就绪位

```c
// select 典型模式 — 每次调用重建 fd_set
FD_ZERO(&readfds);
FD_SET(fd1, &readfds);
FD_SET(fd2, &readfds);
// ...每次 select 调用都做一次 O(n) 拷贝

int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
// 返回后遍历 0..max_fd — O(n) 扫描全部
for (int i = 0; i <= max_fd; i++) {
    if (FD_ISSET(i, &readfds)) {
        // 处理 fd i
    }
}
```

## epoll 的数据结构优势

epoll 通过内核内建数据结构消除了 select 的 O(n) 瓶颈：

```text
epoll Instance (struct eventpoll):

┌─────────────────────────────────────────────────────────────┐
│  ┌──────────────────────────┐  ┌──────────────────────────┐ │
│  │  Red-Black Tree (rbr)    │  │  Ready List (rdllist)    │ │
│  │  stores all registered   │  │  stores fds with pending │ │
│  │  fd → epitem             │  │  events                  │ │
│  │                          │  │                          │ │
│  │  Node: fd + event type   │  │  Kernel driver (NIC      │ │
│  │                          │  │  interrupt) callback     │ │
│  │  Insert   O(log n)       │  │  → ep_poll_callback      │ │
│  │  Delete   O(log n)       │  │       ↓                  │ │
│  │  Modify   O(log n)       │  │  Enqueue ready epitem    │ │
│  │                          │  │  into rdllist            │ │
│  │                          │  │  epoll_wait directly     │ │
│  │                          │  │  retrieves from list     │ │
│  │                          │  │  → O(k) where k = ready  │ │
│  └──────────────────────────┘  └──────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘

Key Advantage:
  fd_set is registered only once (epoll_ctl ADD).
  Subsequent epoll_wait calls incur no fd_set copy overhead.
```

## 事件驱动 vs 轮询

| 特性 | select | poll | epoll |
|------|--------|------|-------|
| 数据结构 | 位图 (fd_set) | pollfd 数组 | 红黑树 + 就绪链表 |
| 最大 fd 数 | 1024 (FD_SETSIZE) | 无限制 | 无限制 |
| 注册方式 | 每次调用重新传入 | 每次调用重新传入 | epoll_ctl 一次注册 |
| 用户→内核拷贝 | O(n) — 每次 select | O(n) — 每次 poll | 仅 epoll_ctl（常数次） |
| 内核扫描 | O(n) — 全部 fd | O(n) — 全部 fd | O(1) — 直接返回就绪列表 |
| 就绪获取 | O(n) — 遍历全部 | O(n) — 遍历全部 | O(k) — 仅遍历就绪 fd |
| 触发模式 | 仅水平触发 | 仅水平触发 | LT + ET |

## 回调机制详解

epoll 的核心设计是**回调驱动**而非**轮询扫描**：

```
1. epoll_ctl(EPOLL_CTL_ADD, fd) 时：
   - 内核为 fd 在其驱动上注册回调函数 ep_poll_callback
   - 回调挂在 fd 的等待队列（wait queue）上

2. 当 fd 就绪（例如网卡收到数据）：
   - 驱动中断处理 → 数据到达 socket → socket 状态变为可读
   - 触发 ep_poll_callback:
     a. 将 epitem 加入 eventpoll.rdllist（就绪链表）
     b. 如果有进程阻塞在 epoll_wait，唤醒该进程

3. epoll_wait 返回：
   - 直接将 rdllist 中的事件拷贝到用户态 events 数组
   - 时间复杂度 O(k)，k = 就绪事件数
```

## 就绪事件如何返回用户态

内核维护关注集合与就绪集合；epoll_wait 将就绪事件返回到用户提供的 events 缓冲区。epoll 不以 mmap 共享用户态/内核态事件区作为其机制——就绪事件经 copy_to_user 拷入用户空间，代价 O(k)（k 为就绪事件数）。

## 性能对比数字

```
场景：100 万个连接，只有 1 个活跃连接

select: 遍历 100 万位 → O(n)  → 100 万次检查
poll:   遍历 100 万 pollfd → O(n) → 100 万次检查
epoll:  仅返回 1 个就绪事件 → O(1) → 1 次取数

场景：100 个连接，50 个活跃

select/poll: 仍遍历 100 → O(n)
epoll:       返回 50 → O(k)，k=50
```

> [!tip]- **工程要点**
> epoll 的优势在连接数大（>1000）时尤其明显。对于少量长连接，select 或 poll 的简单性足够。epoll 的红黑树维护本身也有开销——适合"大并发、稀疏活跃"的场景。Redis 单线程用 epoll 处理数万连接正是利用了 O(1) 就绪通知的优势。

---

epoll 触发模式详解 → [LT vs ET](08b-Level%20Trigger%20vs%20Edge%20Trigger：%20LT⧸ET%20(触发模式).md)
- [System Administration Basics (系统管理基础)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/01-Linux%20Fundamentals%20(Linux%20基础)/03-System%20Administration%20Basics%20(系统管理基础).md)