---
tags:
  - linux/io
status: 🌱
---

> [!important] **核心考点**
> select/poll/epoll 多路复用技术对比、文件描述符上限、触发模式与性能差异

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
- **mmap 共享内存**：避免用户态/内核态数据拷贝
- **红黑树管理**：epoll_ctl 用红黑树维护 fd 集合，增删改 O(log n)
- **就绪链表**：内核把就绪的 fd 链入就绪链表，epoll_wait 直接读取
- **无上限**：受系统最大文件数限制（cat /proc/sys/fs/file-max）

## 三者的详细对比

| 特性 | select | poll | epoll |
|------|--------|------|-------|
| 底层结构 | 位数组 | pollfd 数组 | 红黑树 + 就绪链表 |
| 最大 fd 数 | 1024 | 无上限 | 无上限 |
| 遍历方式 | 全部遍历 O(n) | 全部遍历 O(n) | 直接返回就绪 O(k) |
| 数据拷贝 | 每次拷贝全部 | 每次拷贝全部 | mmap 共享内存，零拷贝 |
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
- [System Administration Basics (系统管理基础)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/03-System%20Administration%20Basics%20(系统管理基础).md)
