---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 05-Nginx Proxy and Load Balancing (Nginx 代理与负载均衡)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 30 秒回答

**05-Nginx Proxy and Load Balancing (Nginx 代理与负载均衡)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## Nginx Architecture (Nginx 架构)

> [!abstract] 核心考点：> Nginx Master-Worker 架构、惊群处理、热加载、事件驱动模型

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

Nginx 配置与实践详解见 → [Reverse Proxy & Load Balancing Config (反向代理配置)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md) · [Nginx vs webserver：Why Use Both (与自写server的关系)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02c-Nginx%20vs%20webserver：Why%20Use%20Both%20(与自写server的关系).md)

---

## Reverse Proxy and Load Balancing (反向代理与负载均衡)

> [!abstract] 核心考点：> 反向代理配置、负载均衡策略、location 匹配规则、动静分离、HTTPS 配置

## 反向代理配置

```nginx
server {
    listen 80;
    server_name api.example.com;

    location / {
        proxy_pass http://backend_server;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_connect_timeout 60s;
        proxy_read_timeout 60s;
        proxy_send_timeout 60s;
    }
}
```

---

## 负载均衡策略

```nginx
upstream backend_server {
    # 1. 轮询（默认）
    server 192.168.1.1:8080;
    server 192.168.1.2:8080;

    # 2. 权重
    server 192.168.1.1:8080 weight=3;
    server 192.168.1.2:8080 weight=1;

    # 3. IP 哈希（保持会话）
    ip_hash;

    # 4. 最少连接
    least_conn;
}
```

| 策略 | 说明 | 适用场景 |
|------|------|---------|
| 轮询（RR） | 默认，按顺序分发 | 无状态服务 |
| 加权轮询 | 指定权重比 | 后端性能不同 |
| IP Hash | 同一 IP 始终到同一后端 | 需要 session 保持 |
| Least Connections | 选连接数最少的 | 请求处理时间不均 |

---

## location 匹配规则

```
优先级从高到低：
1. 精确匹配（=）：location = /login
2. 前缀匹配（^~）：location ^~ /static/
3. 正则匹配（~ 大小写敏感，~* 不敏感）
4. 普通前缀匹配：location /api/
5. 默认 /：兜底

匹配规则：
- 先检查普通前缀匹配，记录最长匹配
- 如果有 ^~，直接使用该匹配
- 否则检查正则匹配（按配置顺序），第一个匹配的正则生效
- 如果没正则匹配，使用最长前缀匹配
```

---

## 动静分离

```nginx
server {
    # 静态资源 → Nginx 直接返回
    location ~* \.(css|js|jpg|png|gif|ico|svg)$ {
        root /var/www/static;
        expires 30d;
        add_header Cache-Control "public, immutable";
        access_log off;
    }

    # 动态请求 → 代理到后端
    location /api/ {
        proxy_pass http://backend_servers;
    }

    # SPA 入口
    location / {
        root /var/www/dist;
        index index.html;
        try_files $uri $uri/ /index.html;
    }
}
```

---

## HTTPS 配置

```nginx
server {
    listen 443 ssl http2;
    server_name example.com;

    ssl_certificate     /etc/nginx/certs/fullchain.pem;
    ssl_certificate_key /etc/nginx/certs/privkey.pem;

    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;
}
server {
    listen 80;
    server_name example.com;
    return 301 https://$server_name$request_uri;
}
```

---

## 经典题型速查（补充 2）

| 题型 | 要点 |
|------|------|
| location 匹配顺序 | 精确 > ^~ 前缀 > 正则 > 普通前缀 > / |
| proxy_pass 有无斜杠的区别 | 有斜杠去掉匹配路径，无斜杠全路径传递 |
| upstream 健康检查 | max_fails=3 fail_timeout=30s 自动踢出故障节点 |
| 反向代理 vs 正向代理 | 正向代理代理客户端，反向代理代理服务端 |
| X-Forwarded-For | 传递客户端真实 IP |

> [!tip]- **工程要点**
> 动静分离可显著提升性能。HTTPS 建议 Let's Encrypt 自动续期。调试用 `nginx -T` 查看合并配置。

---

Nginx 架构与实践详解见 → [Nginx Architecture：Master & Worker Process (架构模型)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02a-Nginx%20Architecture：Master%20&%20Worker%20Process%20(架构模型).md) · [Nginx vs webserver：Why Use Both (与自写server的关系)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02c-Nginx%20vs%20webserver：Why%20Use%20Both%20(与自写server的关系).md)

---

## Nginx and Application Server (Nginx 与应用服务器)

> [!abstract] 核心考点：> Nginx 与自写 Web Server 的职责边界、为什么用 Nginx 做反向代理、部署架构

## 为什么用 Nginx + 自写 Server

纯自写 Web Server（Go/Java/C++）也可以处理 HTTP，但生产环境通常在前面放一层 Nginx。

### 各自职责

```
客户端 → Nginx（基础设施）→ 自写 Server（业务逻辑）
```

| 功能         | Nginx           | 自写 Server |
| ---------- | --------------- | --------- |
| TLS/SSL 终止 | ✓（OpenSSL 优化）   | 卸载到 Nginx |
| 静态文件       | ✓（sendfile 零拷贝） | 不处理       |
| 负载均衡       | ✓（多种策略）         | 不处理       |
| 限流/限速      | ✓（limit_req）    | 不处理       |
| 访问控制       | ✓（IP 黑名单）       | 不处理       |
| 业务逻辑       | ✗               | ✓         |
| 认证/鉴权      | 仅 Basic Auth    | ✓         |
| 数据库操作      | ✗               | ✓         |

---

## Nginx 的优势

### 零拷贝静态文件

```nginx
```

### 连接管理

```
Nginx：单一进程处理数万并发（epoll 事件驱动）
自写 Server：连接越多开销越大
```

### 安全隔离

```
Nginx 做限流、IP 黑名单、过滤恶意请求
→ 即使 Nginx 被打满，自写 Server 不会直接暴露
```

---

## 常见部署架构

### 单层
```
Nginx → App Server → Database
```

### 多层（高并发）
```
负载均衡器（LVS）
    ↓
Nginx 集群（限流 + SSL + 静态）
    ↓
App Server 集群（业务）
    ↓
Redis → MySQL
```

### 微服务
```
Nginx（路由）
 ├── /api/user → User Service
 ├── /api/order → Order Service
 └── /static → 直接返回
```

---

## 什么时候不需要 Nginx

- 纯内网微服务（gRPC 直连）
- 非 HTTP 协议服务（TCP/UDP）
- 简单单体服务（开发/测试环境）

---

## 经典题型速查（补充 3）

| 题型 | 要点 |
|------|------|
| 为什么用 Nginx 而不是全用自写 Server | Nginx 处理静态/TLS/限流/负载均衡更擅长 |
| 自写 Server 的优势 | 完全掌控业务逻辑 |
| 什么时候不需要 Nginx | 纯内网 gRPC、非 HTTP 协议 |
| sendfile 零拷贝 | 避免用户态参与，直接 DMA 到网卡 |
| Nginx + 自写 Server 是职责分离 | 各司其职，不互相替代 |

> [!tip]- **工程要点**
> Nginx 在前面做限流、SSL、静态文件，自写 Server 专注业务——这是后端最佳实践。即使 Go 自带 net/http，生产环境也建议前面挂 Nginx。

---

Nginx 架构与配置详解见 → [Nginx Architecture：Master & Worker Process (架构模型)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02a-Nginx%20Architecture：Master%20&%20Worker%20Process%20(架构模型).md) · [Reverse Proxy & Load Balancing Config (反向代理配置)](/03-Backend%20Systems%20(后端系统)/04-Distributed%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 05-Nginx Proxy and Load Balancing (Nginx 代理与负载均衡) 的问题、核心机制与边界。

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
