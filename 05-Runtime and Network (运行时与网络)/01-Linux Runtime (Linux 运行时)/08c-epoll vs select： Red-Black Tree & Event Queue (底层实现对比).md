---
tags:
  - linux/io
status: seed
review_due: 2026-09-19
confidence: 1
verified: stable
---

# 08c-epoll vs select： Red-Black Tree & Event Queue (底层实现对比)

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
