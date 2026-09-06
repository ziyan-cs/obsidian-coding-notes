---
tags:
  - network/http
status: 🌱
---

# Keep-Alive & Connection Management — 长连接

> [!important] **核心考点**：Keep-Alive 长连接复用、Connection 头、管道传输 Pipeline、HTTP 队头阻塞问题

## 短连接 vs 长连接

**短连接（HTTP/1.0 默认）：**
```
客户端 → 打开 TCP → 请求 → 响应 → 关闭 TCP （下一个请求重复此流程）
```
每个请求都新建 TCP 连接，三次握手 + 慢启动开销巨大。

**长连接（HTTP/1.1 默认）：**
```
客户端 → 打开 TCP → 请求 → 响应 → 请求 → 响应 → ... → 关闭
```
复用同一 TCP 连接发送多个请求，避免连接建立开销。

## Connection 头

HTTP/1.1 默认开启 Keep-Alive，通过 Connection 头控制：

```http
# 启用长连接（HTTP/1.1 默认，可省略）
Connection: keep-alive
Keep-Alive: timeout=5, max=100   # 超时 5s，最大 100 个请求

# 关闭长连接
Connection: close
```

**Connection: close 的行为：**
服务端发送完当前响应后关闭连接。常用于处理异常或负载过高的场景。

## Pipeline：请求管道

管道（Pipelining）允许客户端在**收到上一个响应之前**发送下一个请求：

```
非管道模式（串行）：
  C: 请求1 → → → 
  S:          → 响应1 → → → 
  C:                  请求2 → → → 
  S:                            → 响应2

管道模式：
  C: 请求1 → 请求2 → 请求3 → 
  S:          → 响应1 → 响应2 → 响应3 →
```

**Pipelining 的现实问题：**
- **队头阻塞**（HOL Blocking）：如果响应1生成慢，后面的响应都被阻塞
- **实现复杂**：响应顺序必须与请求顺序一致（否则客户端无法匹配）
- **浏览器默认关闭**：实际推广失败，绝大多数客户端不启用
- **代理兼容性差**：很多中间代理不支持 Pipelining

## 队头阻塞（Head-of-Line Blocking）

HTTP/1.1 的核心缺陷：同一连接上的请求必须有序返回。

```
请求 A（大文件 10MB）    → 阻塞
请求 B（小型 JSON）      → 等待 A 完成    ← HOL Blocking!
请求 C（小型 JSON）      → 等待 B 完成
```

**缓解方案：**

| 方案 | 原理 | 缺点 |
|------|------|------|
| 多连接 | 浏览器打开 6-8 个并行 TCP | 连接竞争、服务器压力大 |
| 域名分片 | 将资源分散到不同域名 | DNS 开销、管理复杂 |
| HTTP/2 多路复用 | 二进制分帧，请求交错 | 需要升级协议 |

## Keep-Alive 工程配置

**Nginx 配置：**
```nginx
http {
    keepalive_timeout  65;        # 空闲连接超时
    keepalive_requests 1000;      # 单个连接最大请求数
}
```

**权衡点：**
```
Keep-Alive 时间过短：
  - 连接复用率低，浪费三次握手
  - 适合短突发请求场景

Keep-Alive 时间过长：
  - 占用服务器 fd 资源（C10K 问题）
  - 客户端已消失但连接未释放
  - 适合频繁请求场景（如 API 服务）
```

> [!tip]- **工程要点**：HTTP/1.1 的长期连接复用显著降低了延迟，但队头阻塞是其结构性瓶颈。这也是为什么 HTTP/2 引入多路复用从根本上解决了"一个请求阻塞整条连接"的问题。对于 HTTP/1.1，实践中通过多连接+域名分片来规避 HOL block，但更好的方案是直接升级到 HTTP/2。

---

HTTP 协议详解见 → [Request & Response Structure (请求响应结构)](</05-Network%20Programming%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06a-Request%20&%20Response%20Structure%20(请求响应结构).md>) · [State Machine Parser Implementation (状态机解析实现)](</05-Network%20Programming%20(网络编程)/03-HTTP%20&%20Application%20Layer%20(HTTP%20与应用层)/06-HTTP⧸1.1%20Protocol%20(HTTP协议详解)%20⭐/06d-State%20Machine%20Parser%20Implementation%20(状态机解析实现).md>)
