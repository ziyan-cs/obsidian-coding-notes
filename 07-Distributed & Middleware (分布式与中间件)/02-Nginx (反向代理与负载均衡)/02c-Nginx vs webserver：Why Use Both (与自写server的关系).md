---
tags:
  - distributed/nginx
status: 🌱
---

> [!important] **核心考点**
> Nginx 与自写 Web Server 的职责边界、为什么用 Nginx 做反向代理、部署架构

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
# Nginx 使用 sendfile：文件 → 网卡，不经用户态
# 应用服务器：文件 → 内核 → 用户态 → 内核 → 网卡
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

## 经典题型速查

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

Nginx 架构与配置详解见 → [Nginx Architecture：Master & Worker Process (架构模型)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02a-Nginx%20Architecture：Master%20&%20Worker%20Process%20(架构模型).md) · [Reverse Proxy & Load Balancing Config (反向代理配置)](/07-Distributed%20&%20Middleware%20(分布式与中间件)/02-Nginx%20(反向代理与负载均衡)/02b-Reverse%20Proxy%20&%20Load%20Balancing%20Config%20(反向代理配置).md)
