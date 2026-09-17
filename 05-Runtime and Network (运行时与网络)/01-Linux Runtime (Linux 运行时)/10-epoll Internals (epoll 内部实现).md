---
status: stable
confidence: high
verified: 2026-09-17
---

> [!abstract] 学习目标：理解兴趣集合、就绪列表与事件返回成本，避免 mmap 零拷贝等错误表述。

> [!note] 本节重点：区分 epoll 的 API 语义与 Linux 当前实现；`epoll_wait` 返回 ready list 中至多 `maxevents` 个事件，其成本与返回事件数和内核实现有关，不能简化为 O(1)。

# select 的局限性

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

# epoll 的数据结构优势

epoll 把关注集合保留在内核，并维护 ready list，避免每次等待都重新传入并线性扫描完整 `fd_set`。下图描述 Linux 内核中的典型实现，不属于跨版本稳定的用户态 API 契约：

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

官方语义以 [epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html) 与 [epoll_wait(2)](https://man7.org/linux/man-pages/man2/epoll_wait.2.html) 为准。

# 事件驱动 vs 轮询

| 特性 | select | poll | epoll |
|------|--------|------|-------|
| 数据结构 | 位图 (`fd_set`) | `pollfd` 数组 | API 暴露 interest list + ready list；Linux 内核通常以树结构和链表实现 |
| 最大 fd 数 | glibc `fd_set` 的 `FD_SETSIZE` 为 1024 | 无固定 `FD_SETSIZE` 限制 | 无固定 `FD_SETSIZE` 限制；仍受资源限制 |
| 注册方式 | 每次调用重新传入 | 每次调用重新传入 | epoll_ctl 一次注册 |
| 用户→内核数据传递 | 每次传入集合 | 每次传入数组 | 注册变更经 `epoll_ctl`；`epoll_wait` 仍需把返回事件写入用户缓冲区 |
| 等待与结果获取 | 检查指定范围 | 检查传入数组 | 从 ready list 取至多 `maxevents` 个结果，不能笼统写成 O(1) |
| 就绪获取 | O(n) — 遍历全部 | O(n) — 遍历全部 | O(k) — 仅遍历就绪 fd |
| 触发模式 | 仅水平触发 | 仅水平触发 | LT + ET |

# 回调机制详解

从 Linux 内核实现看，受监视对象状态变化时会触发相应唤醒路径并把引用加入 ready list；“回调驱动”是帮助理解实现的说法，不是用户态可依赖的 API 保证：

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

# 就绪事件如何返回用户态

内核维护关注集合与就绪集合；epoll_wait 将就绪事件返回到用户提供的 events 缓冲区。epoll 不以 mmap 共享用户态/内核态事件区作为其机制——就绪事件经 copy_to_user 拷入用户空间，代价 O(k)（k 为就绪事件数）。

# 性能对比数字

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
- System Administration Basics (系统管理基础)
