---
tags:
  - network/core
status: 🌱
---

> **核心考点**：为什么存在、带来什么问题、如何正确处理

## 为什么存在 TIME_WAIT？

TIME_WAIT 是主动关闭方在发送最后一个 ACK 后进入的等待状态，持续 **2MSL**（通常 60~120 秒）。

**两个核心原因：**

### 原因一：保证最后一个 ACK 被对端收到

```txt
主动关闭方 ──── ACK ────> 被动关闭方
                          （如果 ACK 丢失）
主动关闭方 <─── FIN ───── 被动关闭方（重传）
主动关闭方 ──── ACK ────> 被动关闭方（再次确认）
```

若没有 TIME_WAIT，主动关闭方收到重传的 FIN 时连接已不存在，会回 RST，导致被动关闭方异常终止。

### 原因二：让旧连接的所有报文消亡

若立即用相同的四元组（src IP, src port, dst IP, dst port）建立新连接，网络中残留的旧报文可能被新连接误收，造成数据混乱。2MSL 足以让所有旧报文过期消失。

---

## TIME_WAIT 带来的问题

在**高并发短连接**场景下（如 HTTP/1.0、频繁建连的微服务），TIME_WAIT 会大量堆积：

- 每个 TIME_WAIT 状态的连接占用一个本地端口
- Linux 默认端口范围约 28000 个（`/proc/sys/net/ipv4/ip_local_port_range`）
- 端口耗尽 → 新连接无法建立 → 服务不可用

---

## 处理方案

### 方案一：开启 tcp_tw_reuse（推荐）

bash

```bash
# 允许将 TIME_WAIT 状态的连接重用于新的出站连接
net.ipv4.tcp_tw_reuse = 1
# 必须同时开启时间戳，才能安全区分新旧报文
net.ipv4.tcp_timestamps = 1
```

- 只对**客户端（主动发起连接方）**有效
- 依赖时间戳选项确保安全

### 方案二：缩短 FIN_TIMEOUT

bash

```bash
net.ipv4.tcp_fin_timeout = 30   # 默认 60s，可适当减小
```

### 方案三：使用长连接 / 连接池（根本方案）

- HTTP/1.1 Keep-Alive、HTTP/2 多路复用
- 数据库连接池、Redis 连接池
- 减少连接建立/断开频率，从根本上避免大量 TIME_WAIT

### 方案四：SO_REUSEADDR（服务端重启用）

c

```c
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

- 允许服务端在 TIME_WAIT 期间重新绑定同一端口
- 主要用于服务重启后快速恢复监听，不用等 2MSL

### ❌ 不推荐：tcp_tw_recycle

- 在 NAT 环境下会导致连接被错误丢弃（同一 NAT 后的多个客户端时间戳不一致）
- Linux 4.12 已彻底移除该参数

---

## 小结

|问题|原因|解法|
|---|---|---|
|端口耗尽|大量短连接主动关闭|tcp_tw_reuse + 长连接|
|服务重启失败|端口被 TIME_WAIT 占用|SO_REUSEADDR|
|残留报文干扰|TIME_WAIT 本身的作用|不应跳过，依赖时间戳保护|

---

TIME_WAIT 在 TCP 状态机中的位置见 → [TCP State Machine (状态机全图)](</05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02b-TCP%20State%20Machine%20(状态机全图).md>) · 流量控制机制见 → [Flow Control & Congestion Control (流量控制与拥塞控制)](</05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02d-Flow%20Control%20&%20Congestion%20Control%20(流量控制与拥塞控制).md>)
