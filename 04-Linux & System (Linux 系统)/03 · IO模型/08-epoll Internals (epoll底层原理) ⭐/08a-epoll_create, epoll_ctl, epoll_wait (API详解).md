---
tags:
  - linux/io
status: 🌱
---

> [!important] **核心考点**
> epoll_create/epoll_ctl/epoll_wait 核心 API、红黑树管理、事件就绪队列

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

> **工程要点**：`epoll_event.data` 是联合体，推荐用 `data.ptr` 指向连接对象（struct），避免再通过 fd 做映射查找。

epoll API 详解见 → [Level Trigger vs Edge Trigger (触发模式)](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/08-epoll%20Internals%20(epoll底层原理)%20⭐/08b-Level%20Trigger%20vs%20Edge%20Trigger：%20LT⧸ET%20(触发模式).md) · [epoll vs select (底层实现对比)](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/08-epoll%20Internals%20(epoll底层原理)%20⭐/08c-epoll%20vs%20select：%20Red-Black%20Tree%20&%20Event%20Queue%20(底层实现对比).md)
