---
tags:
  - distributed/nginx
status: 🌱
---

> [!important] **核心考点**
> Nginx Master-Worker 架构、惊群处理、热加载、事件驱动模型

## Nginx 进程模型

```text
	              ┌───────────────────────────────────────┐
	              │  Master Process                       │
	              │  (root)                               │
	              │  Read config → fork Workers           │
	              │                                       │
	              │  Responsibilities:                    │
	              │  - Hot reload configuration (reload)  │
	              │  - Smooth binary upgrade (upgrade)    │
	              |  - Restart crashed workers            │
	              └──────────────────┬────────────────────┘
                                     │ fork
                 ┌───────────────────┼─────────────────────┐
                 ▼                   ▼                     ▼
    ┌───────────────────┐  ┌───────────────────┐  ┌───────────────────┐
    │  Worker 1         │  │  Worker 2         │  │  Worker 3         │
    │  (nobody user)    │  │  (nobody user)    │  │  (nobody user)    │
    │  epoll event loop │  │  epoll event loop │  │  epoll event loop │
    └──────┬────────────┘  └─────────┬─────────┘  └────────┬──────────┘
           │                         │                     │
           ├─────────────────────────┼─────────────────────┤
         [ Shared Memory / Cache (shared across all workers) ]
           │                         │                     │
           │                         │                     │
    ┌──────┴─────────────────────────┴─────────────────────┴──────────┐
    │  Client requests ──→ accept competition across all workers      │
    └─────────────────────────────────────────────────────────────────┘
```

### 各进程职责

| 进程 | 职责 | 权限 |
|------|------|------|
| **Master** | 读取配置、管理 worker 生命周期、热加载 | root |
| **Worker** | 处理客户端请求、反向代理、负载均衡 | nobody |
| **Cache Loader** | 启动时加载缓存元数据到共享内存 | nobody |
| **Cache Manager** | 管理缓存过期和清理 | nobody |

---

## 为什么 Nginx 性能好

### 1. 事件驱动 + 非阻塞

Nginx 事件循环（与 Redis 类似），每个 worker 独立的事件循环，使用 epoll（Linux）收集就绪事件：

```c
for (;;) {
    events = epoll_wait(epfd, ready_events, max_events, timeout);
    for (i = 0; i < events; i++) {
        process_event(ready_events[i]);  // 读、解析、处理、写回
    }
}
```

### 2. Worker 进程数量 = CPU 核数

```nginx
worker_processes auto;  # = CPU 核心数
```

### 3. 无阻塞调用

```
- 文件读取 → 异步 I/O（aio）
- 上游连接 → epoll 事件驱动
- 磁盘日志 → 缓冲区异步写
```

---

## 惊群问题（Thundering Herd）

多进程同时 accept 同一个 socket，内核唤醒所有等待的 worker，但只有一个能成功。

**Nginx 解决方案：accept_mutex 锁**

```nginx
accept_mutex on;             # 默认开启
accept_mutex_delay 500ms;    # 拿锁失败后等待时间
```

**EPOLLEXCLUSIVE（Linux 4.5+）：** 内核级解决方案，只唤醒一个等待进程。

---

## 热加载（Hot Reload）

```bash
nginx -s reload
# 1. Master 读取新配置
# 2. Master fork 新 worker（新配置）
# 3. 旧 worker 不再接受新连接
# 4. 旧 worker 处理完请求后优雅退出
```

---

## 架构对比

| 特性 | Nginx | Apache |
|------|-------|--------|
| 进程模型 | Master + Worker (event-driven) | Prefork / Worker / Event |
| 并发处理 | epoll 事件驱动 | 进程/线程 per 连接 |
| 静态文件 | 高效（异步 I/O + sendfile） | 中 |
| 内存占用 | 低 | 高 |
| 配置灵活性 | 中 | 高（.htaccess） |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| worker 数量设置 | 通常 = CPU 核数 |
| 惊群问题 | accept_mutex 或 EPOLLEXCLUSIVE 解决 |
| 热加载原理 | 旧进程优雅退出，新进程逐步接管 |
| sendfile 优化 | 零拷贝：文件 → 网卡，不经用户态 |
| 一个 worker 能处理多少连接 | 理论无上限，取决于内存 |

> [!tip]- **工程要点**
> `worker_connections 10240` + `worker_processes auto` 是常见配置。大并发时注意修改 `ulimit -n`。Nginx 架构是"少量进程 + 异步非阻塞"的典范。

---

Nginx 配置与实践详解见 → [Reverse Proxy & Load Balancing Config (反向代理配置)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md) · [Nginx vs webserver：Why Use Both (与自写server的关系)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02c-Nginx%20vs%20webserver：Why%20Use%20Both%20(与自写server的关系).md)
