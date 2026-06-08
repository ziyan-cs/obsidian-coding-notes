---
tags:
  - network
  - socket
status: 🌱
---

> [!important] **核心考点**
> Reactor 单线程模型、事件循环与回调、适用于 IO 密集型场景
> 代表：Redis 6.0 之前的网络处理部分

## 模型结构

```
┌─────────────────────────────────────────────┐
│              单线程                         │
│                                             │
│  ┌──────────┐     ┌───────────────────┐     │
│  │          │ 事件 │   Dispatch        │    │
│  │  Reactor │───> │                   │     │
│  │ (select/ │     │  ┌─────────────┐  │     │
│  │  epoll)  │     │  │ Acceptor    │  │     │
│  │          │     │  │（处理新连接） │  │   │
│  └──────────┘     │  └─────────────┘  │     │
│                   │  ┌─────────────┐  │     │
│                   │  │ Handler     │  │     │
│                   │  │（读/处理/写） │  │   │
│                   │  └─────────────┘  │     │
│                   └───────────────────┘     │
└─────────────────────────────────────────────┘
```

## 核心代码结构

```cpp
// Reactor 单线程事件循环核心框架
class Reactor {
    int epfd_;
    std::unordered_map<int, Handler*> handlers_;

public:
    Reactor() { epfd_ = epoll_create(1); }

    void register_handler(int fd, Handler* h, uint32_t events) {
        struct epoll_event ev = {events, {.fd = fd}};
        epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
        handlers_[fd] = h;
    }

    void loop() {
        struct epoll_event events[1024];
        while (true) {
            // 1. 等待事件（阻塞，可设超时）
            int n = epoll_wait(epfd_, events, 1024, -1);
            for (int i = 0; i < n; i++) {
                int fd = events[i].data.fd;
                // 2. 分发事件给对应 Handler
                if (events[i].events & (EPOLLIN | EPOLLOUT))
                    handlers_[fd]->handle(events[i].events);
            }
            // 3. 回到等待 —— 全部在一个线程中完成
        }
    }
};
```

## 工作流程

1. Reactor 调用 `epoll_wait()` 等待事件
2. 新连接事件 → Acceptor 调用 `accept()`，注册新 fd 到 Reactor
3. 读写事件 → Handler 负责 `read()` → 业务处理 → `write()`
4. 回到步骤 1

## 优点

- 模型简单，无锁，无线程切换开销
- 适合 I/O 密集、业务逻辑极轻的场景

## 缺点

- **业务处理阻塞 = 所有连接阻塞**：单线程中一旦某个 Handler 业务处理耗时，整个 Reactor 卡住
- 无法利用多核 CPU
- 不适合有计算密集型业务的场景

> Redis 用单线程 Reactor 的原因：Redis 的操作都是内存操作，极快，不存在阻塞问题；且避免了多线程的锁竞争。

---

## 关联笔记

- [Single Reactor Multi Thread (单reactor多线程)](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05b-Single%20Reactor%20Multi%20Thread%20(单reactor多线程).md)
- [Multi Reactor Multi Thread： one loop per thread (主从reactor)](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05c-Multi%20Reactor%20Multi%20Thread：%20one%20loop%20per%20thread%20(主从reactor).md)
- [TCP⧸IP Stack Overview (协议栈总览)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/01-TCP⧸IP%20Stack%20Overview%20(协议栈总览).md)
- [Three-Way Handshake & Four-Way Teardown (三次握手四次挥手)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02a-Three-Way%20Handshake%20&%20Four-Way%20Teardown%20(三次握手四次挥手).md)
- [TCP State Machine (状态机全图)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02b-TCP%20State%20Machine%20(状态机全图).md)
