---
tags:
  - linux/io
status: seed
review_due: 2026-09-19
confidence: 1
verified: stable
---

> [!important] **核心考点**：水平触发 LT 与边缘触发 ET 的区别、ET 模式需循环读取、epoll 高效根源

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

epoll 触发模式见 → [epoll_create, epoll_ctl, epoll_wait (API详解)](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/08-epoll%20Internals%20(epoll底层原理)%20⭐/08a-epoll_create,%20epoll_ctl,%20epoll_wait%20(API详解).md) · [epoll vs select (底层实现对比)](/04-Linux%20&%20System%20(Linux%20系统)/03%20·%20IO模型/08-epoll%20Internals%20(epoll底层原理)%20⭐/08c-epoll%20vs%20select：%20Red-Black%20Tree%20&%20Event%20Queue%20(底层实现对比).md)
