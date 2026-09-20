---
study_stage: backlog
---

> [!abstract] 学习目标：掌握 epoll_ctl、epoll_wait、LT、ET 与非阻塞读写循环的契约。

# 模型与选型

`epoll` 是 Linux 的就绪通知接口：应用用 `epoll_ctl` 维护关注集合，再由 `epoll_wait` 取回一批就绪事件。它告诉应用“此刻可能可以读/写”，**不替应用完成读写**；真正调用 `accept`、`read`、`write` 后仍须检查结果。`epoll_wait` 把事件写进用户提供的数组，不依赖“mmap 零拷贝事件区”。

|接口|每轮传入关注集合|返回就绪结果|主要边界|
|---|---|---|---|
|`select`|调用者每次重建 `fd_set`|检查返回后的集合|`fd_set` 大小受 `FD_SETSIZE` 限制，许多环境默认为 1024|
|`poll`|每次传入 `pollfd` 数组|扫描数组中的 `revents`|没有固定的 `FD_SETSIZE` 上限，但仍受资源与扫描成本约束|
|`epoll`|注册后增量管理|数组中仅返回本批就绪事件|Linux 专有；仍受 fd 限额、内存及应用预算约束|

高连接数、低活跃比例的 Linux 服务通常受益于 epoll；小规模场景应以可维护性、移植性和实测为准。红黑树、就绪链表等属于 Linux 内核实现细节，不能把 `epoll_wait` 概括成无条件 O(1)：至少要处理返回的事件数。

# 三个核心 API

> [!warning] 示例循环省略了生产级错误处理
> `accept`、`read`、`write` 都可能返回 `EAGAIN`、`EINTR` 或错误。ET 模式还必须循环读/accept 到 `EAGAIN`；不要把下面的最小骨架直接当成完整服务器。

## epoll_create

```c
#include <sys/epoll.h>

int epfd = epoll_create1(EPOLL_CLOEXEC); // 返回 -1 时检查 errno
```

旧接口 `epoll_create(size)` 的 `size` 在现代 Linux 内核中已忽略，但仍须大于零；新代码通常选择 `epoll_create1(EPOLL_CLOEXEC)`，避免 exec 时泄漏 epoll fd。

## epoll_ctl

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

// op：EPOLL_CTL_ADD / EPOLL_CTL_MOD / EPOLL_CTL_DEL

struct epoll_event ev = {0};
ev.events = EPOLLIN;      // 默认 LT；需要 ET 时再加 EPOLLET
ev.data.fd = fd;          // data 是 union，也可存连接对象指针
if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    // 记录 errno；按当前连接的所有权规则清理 fd
}
```

`EPOLL_CTL_MOD` 更新关注事件，`EPOLL_CTL_DEL` 删除。常见标志还有 `EPOLLOUT`、`EPOLLERR`、`EPOLLHUP`、`EPOLLRDHUP`、`EPOLLET`、`EPOLLONESHOT`。不要长期无条件订阅 `EPOLLOUT`：可写 socket 可能持续就绪，形成忙循环；有待发送数据时再订阅。

## epoll_wait

```c
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
// timeout: -1 阻塞, 0 立即返回, >0 超时毫秒

// n > 0：处理 n 个事件；n == 0：超时；n == -1：检查 errno
```

## 事件循环的最小骨架

```text
create1(EPOLL_CLOEXEC) → 注册非阻塞监听 fd
while running:
    epoll_wait；EINTR 时继续，其他错误按策略处理
    for event in returned_events:
        listen fd 可读 → accept 循环；EAGAIN 时停止
        连接 fd 有 ERR/HUP/RDHUP → 检查状态，决定读剩余数据或关闭
        连接 fd 可读 → 累计读取到 EAGAIN/预算上限，交给分帧器
        连接 fd 可写 → 发送待写缓冲，到 EAGAIN 或写完
    处理超时、背压和关闭中的连接
```

这是流程伪代码，不是完整服务器。`data.fd` 与 `data.ptr` 都可以，后者要保证连接对象在事件队列/回调期间仍存活；多线程共用 epoll 实例时，应明确连接所有权，按需使用 `EPOLLONESHOT` 并在处理后重新 `MOD` 激活。

# LT 与 ET：同一就绪条件的两种通知策略

LT 和 ET 改变的是**重复通知策略**，不改变 TCP 字节流、socket 缓冲区或消息边界。先能写对 LT，再决定是否需要 ET。

## 水平触发 LT（Level-Triggered）

**默认模式。** 只要 fd 仍满足可读条件，后续 `epoll_wait` 仍可返回该 fd；一次调用是否立刻返回还受其他事件、超时等条件影响。

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

**只通知就绪状态的变化，而非持续提醒。** 如果一次没把已有可读数据处理完，不能依赖下一次 `epoll_wait` 必然再次报告同一 fd。内核可能因后续数据到达再次报告，但应用正确性不能寄托于此。

```c
ev.events = EPOLLIN | EPOLLET;  // 边缘触发
```

**行为：** 缓冲区已有 100 字节，只读 50 字节后就回到 `epoll_wait`，剩余 50 字节可能一直无人读取；这是 ET 常见的“卡住”症状。

**ET 模式的使用要求：**

```text
先以 fcntl(F_GETFL) 取现有状态标志，检查错误，再 OR O_NONBLOCK 后 F_SETFL
每次可读通知：
    循环 read；n > 0 → 追加到有界接收缓冲并尝试分帧
    n == 0 → 对端关闭；处理已经读到的完整帧，再按连接状态清理
    n < 0 且 errno == EINTR → 重试
    n < 0 且 errno == EAGAIN/EWOULDBLOCK → 本轮读尽，返回事件循环
    其他错误 → 记录并关闭连接
```

这里是流程伪代码。若为公平性设置单连接读预算，在尚未读到 `EAGAIN` 时主动停下，必须把该连接放入应用自己的待处理队列或重新激活，不能只等待下一次边缘通知。写端同样要保存未写完的偏移量，遇 `EAGAIN` 后按需关注 `EPOLLOUT`。

## LT vs ET 对比

| 特性 | LT | ET |
|------|-----|-----|
| 通知条件 | 就绪条件持续成立可重复通知 | 不持续重复提醒，需自行维护处理进度 |
| 未读尽时可否直接回到等待 | 可以（仍须保证公平性） | 不应只依赖下一次通知 |
| fd 非阻塞 | 服务器通常推荐 | 实践中应使用，避免排空时阻塞 |
| 编程复杂度 | 低 | 高（需循环 + EAGAIN 判断） |
| 性能 | 取决于负载与处理策略 | 可能减少重复通知，也增加实现复杂度 |

## 实际选型

- **初学与常规服务**：优先 LT，减少漏通知风险；仍需限制每连接的处理预算并做好背压。
- **已经有成熟非阻塞状态机的服务**：可对 ET 做基准测试，再决定是否采用。不要把某个产品的某一版本实现当作普遍性能定律。

遇到“ET 连接卡住”，按顺序查：fd 是否非阻塞、读写是否处理到 `EAGAIN`、预算提前结束后是否已自行重新调度、`EPOLLONESHOT` 是否重新激活。详见 [epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html) 与 [epoll Internals (epoll 内部实现)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/10-epoll%20Internals%20(epoll%20内部实现).md)。


---
