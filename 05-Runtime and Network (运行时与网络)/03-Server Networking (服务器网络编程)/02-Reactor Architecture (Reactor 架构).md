---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Reactor Architecture (Reactor 架构)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## Reactor Pattern (Reactor模式)

> [!abstract] 核心考点：> Reactor 单线程模型、事件循环与回调、适用于 IO 密集型场景
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

> Redis 用单线程 Reactor 的原因：Redis 的操作都是内存操作，极快，不存在阻塞问题；且避免了多线程的锁竞争。

---

Reactor 模型进阶见 → [Single Reactor Multi Thread (单reactor多线程)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05b-Single%20Reactor%20Multi%20Thread%20(单reactor多线程).md>) · [Multi Reactor Multi Thread： one loop per thread (主从reactor)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05c-Multi%20Reactor%20Multi%20Thread：%20one%20loop%20per%20thread%20(主从reactor).md>)

---

## Reactor Threading Models (Reactor线程模型)

> [!abstract] 核心考点：> 单 Reactor 多线程模型、IO 线程与工作线程分离、任务队列与线程安全
> 解决了单线程模型"业务处理阻塞"的问题

## 模型结构（补充 2）

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

## 核心代码结构（补充 2）

```cpp
// 主线程 Reactor + 工作线程池（简化）
class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;

public:
    ThreadPool(int n) {
        for (int i = 0; i < n; i++)
            workers_.emplace_back([this] {
                while (true) {
                    Task t;
                    {
                        std::unique_lock lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        t = std::move(tasks_.front()); tasks_.pop();
                    }
                    t();  // 工作线程执行业务逻辑
                }
            });
    }
    void submit(Task t) {
        std::lock_guard lock(mtx_);
        tasks_.push(std::move(t));
        cv_.notify_one();
    }
};

// 使用方式：Handler 中收到读事件后
void Handler::on_readable() {
    char buf[4096]; int n = read(fd_, buf, sizeof(buf));
    // 将业务处理提交到线程池，不阻塞主线程
    thread_pool_.submit([this, data = std::string(buf, n)] {
        std::string resp = process(data);  // 业务处理（在 worker 线程）
        // 将响应投递回所属 Reactor；由 I/O 线程写入连接的 write buffer。
        reactor_.queue_in_loop([this, resp = std::move(resp)] {
            append_and_enable_write(resp);
        });
    });
}
```

## 工作流程（补充 2）

1. 主线程 Reactor 监听事件，Acceptor 接受新连接
2. 读事件到来，Handler 在**主线程**完成 `read()`，将数据投递给线程池
3. 工作线程处理业务逻辑
4. 工作线程将结果投递回 Reactor；所属 I/O 线程更新 write buffer 与 `EPOLLOUT`

## 优点（补充 2）

- 业务处理与 I/O 解耦，业务耗时不影响 I/O 响应
- 能利用多核 CPU

## 缺点（补充 2）

- **单 Reactor 仍是瓶颈**：所有 I/O 事件都在一个线程处理
- 工作线程写回时需要注意线程安全（共享的 fd → 加锁或排队写）

> **与 05a 的区别：** 05a 所有工作在单线程串行；05b 将业务逻辑卸载到工作线程，I/O 读写与连接状态仍归 Reactor 线程所有。高并发下主线程仍可能成为瓶颈——进一步优化见 05c 主从 Reactor 模型。

## 30 秒回答

单 Reactor 多线程把连接 I/O 与业务计算拆开，但不把一个连接的状态随意交给多个线程。Reactor 线程读请求并拥有 fd/read-write buffer；worker 只处理独立业务数据，完成后通过线程安全队列投递结果回 Reactor。关键风险是任务积压、连接已关闭和响应乱序。

---

Reactor 模型演进见 → [Single Reactor Single Thread (单reactor单线程)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05a-Single%20Reactor%20Single%20Thread%20(单reactor单线程).md>) · [Multi Reactor Multi Thread： one loop per thread (主从reactor)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/05-Reactor%20&%20Proactor%20Pattern%20(事件驱动模型)%20⭐/05c-Multi%20Reactor%20Multi%20Thread：%20one%20loop%20per%20thread%20(主从reactor).md>)

---

## Multi Reactor Architecture (多Reactor架构)

> [!abstract] 核心考点：> 主从 Reactor 多线程模型、one loop per thread 设计、Nginx/Netty/Redis 等实际应用
> 代表：Nginx、Netty、Muduo、Node.js cluster 模式  
> 最成熟的高性能网络服务器架构


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

每个 Sub Reactor 是一个独立的 **event loop**，运行在自己的线程中，负责管理一批连接的所有 I/O 操作。线程之间的连接互不干扰，**天然无锁**。

## 工作流程（补充 3）

1. Main Reactor 只监听 listening fd，`accept()` 新连接
2. 通过负载均衡策略（轮询、最少连接）将新连接的 fd 分配给某个 Sub Reactor
3. 各 Sub Reactor 在自己的线程中独立运行 event loop，处理分配给它的所有连接的读写
4. 业务逻辑若复杂，可再投递给线程池处理

## 核心优势

|特性|说明|
|---|---|
|accept 不成瓶颈|Main Reactor 专职 accept，不处理 I/O|
|I/O 充分并行|N 个 Sub Reactor 并行处理，充分利用多核|
|无锁设计|同一连接的所有操作在同一线程，无需加锁|
|线性扩展|Sub Reactor 数量通常 = CPU 核数|

## 与前两种模型的对比

|模型|线程数|accept|I/O|业务处理|适用场景|
|---|---|---|---|---|---|
|单 Reactor 单线程|1|主线程|主线程|主线程|轻量服务（Redis）|
|单 Reactor 多线程|1 + N|主线程|主线程|线程池|中等并发|
|主从 Reactor 多线程|1 + M + N|主线程|M 个子线程|线程池（可选）|高并发生产环境|

---

## Proactor 模式（补充对比）

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
> 主从 Reactor 是目前高并发服务器最成熟的架构模式，其核心优势在于"无锁"——同一连接的所有操作在同一线程中，天然避免了锁竞争。但需注意 Sub Reactor 之间的负载均衡（轮询可能导致热点），实际生产中常结合连接预估或最少连接策略来分配。
>

另两种变体见 → [Single Reactor Single Thread](05a-Single%20Reactor%20Single%20Thread%20(单reactor单线程).md) · [Single Reactor Multi Thread](05b-Single%20Reactor%20Multi%20Thread%20(单reactor多线程).md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 02-Reactor Architecture (Reactor 架构) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
