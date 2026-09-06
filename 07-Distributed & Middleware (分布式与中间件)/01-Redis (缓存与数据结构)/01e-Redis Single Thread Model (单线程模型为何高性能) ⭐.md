---
tags:
  - redis/core
status: seed
review_due: 2026-10-10
confidence: 1
verified: stable
---

# Redis Single Thread Model — 单线程模型为何高性能

> [!important] **核心考点**
> Redis 单线程模型、I/O 多路复用、为何单线程还快、瓶颈在哪里

## Redis 单线程模型

Redis 的多数命令执行路径以单线程事件循环为核心；网络 I/O、持久化和后台任务的线程/进程模型随版本与配置而变：

```text
┌──────────────┐
│  Client      │
│  Connections │
│  (N clients) │
└──────┬───────┘
       │
       ▼
┌──────────────────────┐
│  epoll/kqueue        │
│  Event Notification  │
└──────┬───────────────┘
       │
       ▼
┌──────────────────────────┐
│  Main Thread             │
│  Event Loop (aeMain)     │
└──────┬───────────────────┘
       │
       ▼
┌──────────────────┐
│  Read Event →    │
│  Parse Command   │
└──────┬───────────┘
       │
       ▼
┌──────────────────┐
│  Execute Command │
└──────┬───────────┘
       │
       ▼
┌──────────────────┐
│  Write Result    │
│  Back to Client  │
└──────┬───────────┘
       │
       └──────→ (next round → epoll/kqueue)
```

**注意：** Redis 6.0+ 的 I/O 线程池只在**读写 socket** 阶段多线程化，**命令执行**仍是单线程。

---

## 为什么单线程还这么快

| 原因 | 说明 |
|------|------|
| **纯内存操作** | 避免普通磁盘访问，但延迟仍受数据结构、CPU、缓存未命中与网络影响 |
| **I/O 多路复用** | 单线程处理大量并发连接（基于 epoll） |
| **非阻塞 I/O** | 不等待就绪事件，事件循环高效轮询 |
| **无锁竞争** | 单线程不存在锁竞争和上下文切换 |
| **数据结构优化** | SDS、ziplist 等针对内存效率优化 |

### 延迟对比

```
内存访问（L1/L2/L3）:  ~1-10 ns
内存访问（RAM）:     ~100 ns
SSD 随机读:         ~10-50 μs  ← 100× slower
网络 RTT（同机房）:  ~0.5 ms  ← 5000× slower
磁盘寻道:           ~10 ms   ← 100000× slower

Redis 瓶颈通常在网络 I/O，而非 CPU
```

> 上表为**数量级参考**（经典 "Latency Numbers Every Programmer Should Know"），具体数值随硬件与网络环境变化，非精确测量值（NEEDS_VERIFY）。Redis 实际延迟应通过 `redis-benchmark` / `redis-cli --latency` 在本机测量。

---

## I/O 多路复用

```c
// Redis 事件循环核心（ae.c）
// 基于 epoll（Linux）/ kqueue（macOS）/ select（兜底）

void aeMain(aeEventLoop *eventLoop) {
    while (!eventLoop->stop) {
        // 阻塞等待事件（最多等待设定的时间）
        aeProcessEvents(eventLoop, AE_ALL_EVENTS);
    }
}

int aeProcessEvents(aeEventLoop *el, int flags) {
    // 调用 epoll_wait 获取就绪事件
    int numevents = epoll_wait(el->epfd, events, AE_SETSIZE, tvp);
    
    for (int j = 0; j < numevents; j++) {
        aeFileEvent *fe = &el->events[events[j].data.fd];
        
        // 处理读事件（客户端发请求）
        if (events[j].events & EPOLLIN)
            fe->rfileProc(el, fd, fe->clientData, mask);
        
        // 处理写事件（发结果给客户端）
        if (events[j].events & EPOLLOUT)
            fe->wfileProc(el, fd, fe->clientData, mask);
    }
}
```

**epoll 的优势：** 应用可等待就绪事件而无需用户态线性扫描所有连接；其内部复杂度与实际成本不能只用一个 O(1) 概括。

---

## 单线程的问题

| 问题 | 影响 | 解决方案 |
|------|------|---------|
| 单个慢命令阻塞所有 | `KEYS *`、`HGETALL` 大 hash | 用 `SCAN`、`SSCAN` 替代 |
| CPU 密集型操作 | 计算耗时操作的 QPS 下降 | 分布式部署多个实例 |
| 大 key 操作 | 阻塞时间与 key 大小成正比 | `UNLINK`（异步删除）、拆分大 key |
| Lua 脚本超时 | 脚本内循环或死循环 | 脚本设执行时间上限 |

### 什么命令慢

```bash
# 查看慢查询（执行时间超过设定值的命令）
slowlog-log-slower-than 10000  # 单位微秒（默认 10ms）
slowlog-max-len 128
slowlog get 10                 # 获取最近 10 条慢查询

# 典型慢命令
KEYS *         # 遍历所有 key（O(N)），禁止生产使用
SMEMBERS       # 取大集合全部成员，O(N)
HGETALL        # 取大 hash 全部字段
SORT           # 排序 O(N+M*logM)
LTRIM / LREM   # 列表操作可能 O(N)
```

---

## Redis 6.0 多线程 I/O

```ini
# redis.conf
io-threads 4          # I/O 线程数（默认 4）
io-threads-do-reads yes  # 启用多线程读取
```

**多线程 I/O 模型：**

```text
Main Thread                       I/O Threads
    │                                  │
    ├── epoll_wait to get ready events │
    │  (running)                       │
    │                                  │
    ├── Distribute read tasks ────────→│
    │                                  ├── Parallel read from
    │                                  │   socket connections
    │◄──── Return results ─────────────┤
    │                                  │
    ├── Execute commands sequentially  │
    │  (still single-threaded)         │
    │                                  │
    ├── Distribute write tasks ───────→│
    │                                  ├── Parallel write back
    │                                  │   to socket connections
    │◄──── Completion ─────────────────┤
    │                                  │
    ├── Next round of event loop       │
```

**命令执行仍是单线程**，所以无需修改数据结构，无需考虑并发安全问题。

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| Redis 为什么快 | 纯内存 + epoll + 单线程无锁 + 数据结构优化 |
| 单线程的瓶颈 | 网络 I/O 而非 CPU（除非有大 key 或慢命令） |
| 什么场景 Redis 不够 | 多核 CPU 无法充分利用 → 开多个实例 |
| KEYS 代替方案 | `SCAN 0 MATCH * COUNT 1000` 游标迭代 |
| 多线程 I/O 做了什么 | 读请求/写响应多线程，命令执行仍然是单线程 |

> [!tip]- **工程要点**
> 生产环境避免 `KEYS *`，对大 key 使用 `SCAN`、拆分与异步删除等策略。通过 `redis-cli --bigkeys` 与慢日志定位问题；延迟目标必须由部署拓扑、SLO 和本机测量确定，不能背固定 1ms/5ms 阈值。

---

Redis 的底层数据结构详解见 → [SDS](01a1-SDS：Simple%20Dynamic%20String%20(简单动态字符串).md) · [ziplist](01a2-ziplist%20&%20listpack%20(压缩列表).md) · [跳表](01a3-skiplist：Sorted%20Set%20Internals%20(跳表)%20⭐.md)
