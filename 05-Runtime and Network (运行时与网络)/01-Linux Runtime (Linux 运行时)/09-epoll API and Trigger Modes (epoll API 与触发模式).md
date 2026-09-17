---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习目标：掌握 epoll_ctl、epoll_wait、LT、ET 与非阻塞读写循环的契约。

# epoll（Linux 专属）

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
- **按就绪结果返回**：`epoll_wait` 从 ready list 向用户缓冲区返回至多 `maxevents` 个事件，调用成本至少与本次返回数量有关，不应概括为“事件通知 O(1)”
- **关注集合 + 就绪集合**：内核维护关注集合与就绪集合；epoll_wait 将就绪事件返回到用户提供的 events 缓冲区（epoll 不以 mmap 共享事件区作为其机制）
- **兴趣集合管理**：`epoll_ctl` 维护注册项；红黑树是 Linux 内核实现细节，不是 epoll API 契约
- **就绪链表**：内核把就绪的 fd 链入就绪链表，epoll_wait 直接读取
- **没有 `select` 的 `FD_SETSIZE` 固定集合限制**：实际容量仍受进程 `RLIMIT_NOFILE`、系统文件表、内存和应用资源预算约束

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
- System Administration Basics (系统管理基础)

---

# epoll API (epoll API)

> [!note] 本节重点： epoll_create/epoll_ctl/epoll_wait 核心 API、红黑树管理、事件就绪队列

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

> [!summary]- 复述与自测：学完后再展开
>
> - **常见误区**：用 `data.fd` 存 fd 后还要回查连接对象（应直接用 `data.ptr`）；多线程共享 epfd 时漏设 `EPOLLONESHOT`，导致同一事件被多线程重复处理。
> - **自测**：1) `epoll_event.data` 为什么是 union？ 2) `epoll_create` 的 size 参数为什么被忽略？
>
> epoll API 详解见 → Level Trigger vs Edge Trigger (触发模式) · epoll vs select (底层实现对比)
>
> ---

# Level and Edge Triggering (水平与边缘触发)

> [!note] 本节重点：水平触发 LT 与边缘触发 ET 的区别、ET 模式需循环读取、epoll 高效根源

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

epoll 触发模式见 → epoll_create, epoll_ctl, epoll_wait (API详解) · epoll vs select (底层实现对比)

---
