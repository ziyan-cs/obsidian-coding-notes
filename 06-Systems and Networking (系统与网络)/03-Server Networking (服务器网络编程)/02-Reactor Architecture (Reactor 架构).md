---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# Reactor Pattern (Reactor模式)

> [!note] 本节重点： Reactor 单线程模型、事件循环与回调、适用于 IO 密集型场景
> 代表：Redis 6.0 之前的网络处理部分

## 模型结构

```text
┌───────────────────────────────────────────────┐
│  Single Thread                                │
├───────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────┐  │
│  │  Reactor (epoll_wait event loop)        │  │
│  └──────────────────┬──────────────────────┘  │
│                     │ event arrives           │
│                     ▼                         │
│  ┌─────────────────────────────────────────┐  │
│  │  Event Dispatcher                       │  │
│  └──────┬─────────────┬──────────────┬─────┘  │
│         │ new conn    │ I/O event    │ I/O ev │
│         ▼             ▼              ▼        │
│  ┌──────────┐  ┌──────────────┐ ┌──────────┐  │
│  │ Acceptor │  │ Handler A    │ │ Handler B│  │
│  │accept()  │  │read→proc→wr  │ │read→proc→│  │
│  └────┬─────┘  └──────────────┘ └──────────┘  │
│       │ register new fd                       │
│       └──────────────→ Reactor                │
└───────────────────────────────────────────────┘
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

> 单 Reactor 的优势是连接状态集中、同步简单；但业务处理、慢系统调用或大回复仍可能阻塞事件循环。不能把“内存操作”理解为绝无阻塞；具体产品是否采用多线程 I/O 也随版本与配置变化。

---

# Reactor Threading Models (Reactor线程模型)

> [!note] 本节重点： 单 Reactor 多线程模型、IO 线程与工作线程分离、任务队列与线程安全
> 解决了单线程模型"业务处理阻塞"的问题

## IO 线程与工作线程
```text
┌───────────────────────────────────────────┐
│  Main Thread (Reactor)                    │
├───────────────────────────────────────────┤
│  ┌─────────────────────────────────────┐  │
│  │  Reactor (epoll_wait)               │  │
│  └──────────────────┬──────────────────┘  │
│                     │                     │
│                     ▼                     │
│  ┌─────────────────────────────────────┐  │
│  │  Event Dispatcher                   │  │
│  └──────┬──────────────────┬───────────┘  │
│         │ new conn         │ I/O readable │
│         ▼                  ▼              │
│  ┌──────────┐     ┌───────────────────┐   │
│  │ Acceptor │     │  submit to pool   │   │
│  └──────────┘     └───────────────────┘   │
│       │ register fd                       │
│       └──────────→ Reactor                │
└─────────────────────┬─────────────────────┘
                      │
                      │ tasks
                      ▼
┌───────────────────────────────────────────┐
│  Worker Thread Pool                       │
├──────────┬──────────┬─────────────────────┤
│ Worker 1 │ Worker 2 │  Worker 3           │
│ business │ business │  business           │
│ logic    │ logic    │  logic              │
└──────────┴──────────┴─────────────────────┘
```

## 任务交接与连接生命周期

以下是**流程伪代码**，不是可直接编译的 C++ 类。之前常见的 `read()` 后直接用返回值构造 `std::string(buf, n)` 存在缺陷：`n` 可为 `-1`（错误）或 `0`（对端关闭），且一次读取不一定是一条完整消息；异步 lambda 捕获裸 `this` 还可能在连接关闭后悬空。

```text
I/O thread: read until EAGAIN / EOF / error; feed bytes into frame parser
            only dispatch complete request and immutable data to bounded pool
            if queue full: pause read / reject / apply per-connection backpressure
worker:     process request without touching socket or Connection mutable state
            post result plus connection identity/generation to its owner loop
I/O thread: check connection is still live and request ordering policy
            append response to write buffer; enable writable notification
```

连接对象通常由所属 loop 管理；跨线程结果用可验证的连接 token、generation 或受控 `weak_ptr` 关联，不能假定 fd 数值未被复用。停止时先拒绝新任务，再排空或取消队列，最后等待 worker；队列容量与每连接在途数都必须有限。

## 一次请求的执行路径
1. 主线程 Reactor 监听事件，Acceptor 接受新连接
2. 读事件到来，Handler 在**主线程**完成 `read()`，将数据投递给线程池
3. 工作线程处理业务逻辑
4. 工作线程将结果投递回 Reactor；所属 I/O 线程更新 write buffer 与 `EPOLLOUT`

## 收益
- 业务处理与 I/O 解耦，业务耗时不影响 I/O 响应
- 能利用多核 CPU

## 边界与代价
- **单 Reactor 仍是瓶颈**：所有 I/O 事件都在一个线程处理
- 工作线程写回时需要注意线程安全（共享的 fd → 加锁或排队写）

相比单 Reactor 单线程，此模型把业务计算卸载到 worker，但连接 I/O 与状态仍归 Reactor 线程所有；如果主 loop 饱和，再考虑拆分为多个 I/O loop。

> [!summary] 核心摘要
>
> 单 Reactor 多线程把连接 I/O 与业务计算拆开，但不把一个连接的状态随意交给多个线程。Reactor 线程读请求并拥有 fd/read-write buffer；worker 只处理独立业务数据，完成后通过线程安全队列投递结果回 Reactor。关键风险是任务积压、连接已关闭和响应乱序。
>
# Multi Reactor Architecture (多Reactor架构)

> 本节以主 Reactor 接收、多个子 Reactor 分管连接的**一种设计**为例；Muduo/Netty 可参考，但 Nginx 的 master/worker 多进程不能画成下面这张线程分发图。

```text
┌────────────────────────────────────────────────┐
│  Main Thread (Main Reactor)                    │
├────────────────────────────────────────────────┤
│  Main Reactor                                  │
│  epoll on listening fd                         │
│  Handles accept() only                         │
└──────────┬────────────┬──────────────┬─────────┘
           │ new conn   │ new conn     │ new conn
           ▼            ▼              ▼
┌─────────────────────────────────────────────────┐
│  Sub Reactors (one event loop per thread)       │
├──────────────┬────────────────┬─────────────────┤
│ Sub Rctr 1   │  Sub Reactor 2 │  Sub Reactor N  │
│ ┌──────────┐ │  ┌──────────┐  │  ┌──────────┐   │
│ │  epoll   │ │  │  epoll   │  │  │  epoll   │   │
│ │  Handler │ │  │  Handler │  │  │  Handler │   │
│ │ rd/pr/wr │ │  │ rd/pr/wr │  │  │ rd/pr/wr │   │
│ │ Thread 1 │ │  │ Thread 2 │  │  │ Thread N │   │
│ └──────────┘ │  └──────────┘  │  └──────────┘   │
└───────┬──────┴────────┬───────┴────────┬────────┘
        │               │                │
        └───────┬───────┴────────┬───────┘
                │   (optional)   │
                ▼                ▼
      ┌─────────────────────────────────────┐
      │     Thread Pool (CPU-intensive)     │
      └─────────────────────────────────────┘
```

## "One Loop Per Thread" 的含义

每个 Sub Reactor 是一个独立的 **event loop**，运行在自己的线程中，负责一批连接的 I/O。若连接状态只由所属线程修改，这部分状态可避免跨线程锁；工作队列、共享缓存、统计与连接迁移仍需同步。

## 连接分配与处理流程
1. Main Reactor 只监听 listening fd，`accept()` 新连接
2. 通过负载均衡策略（轮询、最少连接）将新连接的 fd 分配给某个 Sub Reactor
3. 各 Sub Reactor 在自己的线程中独立运行 event loop，处理分配给它的所有连接的读写
4. 业务逻辑若复杂，可再投递给线程池处理

## 核心优势

|特性|说明|
|---|---|
|accept 不成瓶颈|Main Reactor 专职 accept，不处理 I/O|
|I/O 充分并行|N 个 Sub Reactor 并行处理，充分利用多核|
|减少连接状态锁|同一连接的可变状态归所属 loop，跨线程共享部分仍需同步|
|线性扩展|Sub Reactor 数量通常 = CPU 核数|

## 与前两种模型的对比

|模型|线程数|accept|I/O|业务处理|适用场景|
|---|---|---|---|---|---|
|单 Reactor 单线程|1|主线程|主线程|主线程|轻量服务（Redis）|
|单 Reactor 多线程|1 + N|主线程|主线程|线程池|中等并发|
|主从 Reactor 多线程|1 + M + N|主线程|M 个子线程|线程池（可选）|高并发生产环境|

---

# Proactor 模式（补充对比）

Reactor 和 Proactor 的根本区别在于 **I/O 操作由谁来执行**：

|        | Reactor（同步 I/O）           | Proactor（异步 I/O）            |
| ------ | ------------------------- | --------------------------- |
| I/O 操作 | 应用程序自己调用 read/write       | 操作系统完成 I/O，再通知应用            |
| 事件通知时机 | "fd 可读/可写了，你来读"           | "数据已经读好了，放在这里"              |
| 代表实现   | Linux epoll（配合非阻塞 socket） | Windows IOCP、Linux io_uring |
| 编程复杂度  | 较低                        | 较高                          |

> Linux 上 epoll 本质是 Reactor 模型。真正的 Proactor 在 Linux 上由 **io_uring**（内核 5.1+）实现，性能极高，是现代高性能服务器的新趋势。

---

## 实际框架对应

| 框架/项目       | 模型                                                              |
| ----------- | --------------------------------------------------------------- |
| Redis（6.0前） | 单 Reactor 单线程                                                   |
| Nginx       | master/worker 多进程，各 worker 独立事件循环                          |
| Netty       | 主从 Reactor 多线程（Boss Group + Worker Group）                       |
| Muduo（陈硕）   | 主从 Reactor，one loop per thread                                  |
| Node.js     | 单 Reactor 单线程（libuv event loop）+ 线程池（libuv threadpool，处理文件 I/O） |
| Tokio（Rust） | 主从 Reactor 多线程（work-stealing 调度）                                |

> **Nginx 说明**：Nginx 采用 master/worker 多进程架构，worker 各自运行事件循环；它**不是**主 Reactor accept 后分发给子 Reactor 的 Muduo/Netty 式主从模型。历史上 worker 通过 accept_mutex（或新版 EPOLLEXCLUSIVE / SO_REUSEPORT）竞争共享监听 socket，具体机制随版本与配置不同，实现时按所用版本核对官方文档。

---

> [!tip]- **工程要点**
> 主从 Reactor 是一种常见的连接分片方案，不是所有高并发服务的唯一标准架构。它减少连接内部的跨线程共享，但任务队列、全局资源与观测仍可能需要锁；还要验证子 loop 负载、热点连接和关闭时跨线程回调的生命周期。
>



> [!info]- 延伸阅读
> - 下一步：[03-Timers (定时器)](/06-Systems%20and%20Networking%20(系统与网络)/03-Server%20Networking%20(服务器网络编程)/03-Timers%20(定时器).md)
