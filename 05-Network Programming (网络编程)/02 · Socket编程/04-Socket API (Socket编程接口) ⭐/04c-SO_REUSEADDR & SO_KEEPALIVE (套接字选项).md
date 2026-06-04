---
tags:
  - network
  - socket
---

> **核心考点**：SO_REUSEADDR 解决 TIME_WAIT 复用、SO_KEEPALIVE 心跳保活、TCP_NODELAY 与 Nagle 算法
# setsockopt

```cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen); 
```

- **level**: 选项所属的协议层
    - `SOL_SOCKET`: 通用套接字选项层（最常用）
    - `IPPROTO_TCP`: TCP 协议
    - `IPPROTO_IP`: IP 协议
    
- **optname**: 要设置的选项名
    - `SO_REUSEADDR`: 允许端口复用
    - `SO_REUSEPORT`: 允许多个进程绑定同一个端口（Linux 3.9+）
    - `SO_KEEPALIVE`: 开启 TCP 保活机制
    - `TCP_NODELAY`: 禁用 Nagle 算法，降低延迟
    - `SO_RCVBUF`/`SO_SNDBUF`: 设置接收 / 发送缓冲区大小
    - `SO_RCVTIMEO`/`SO_SNDTIMEO`: 设置接收 / 发送超时时间
    
- **optval**: 选项值的指针（如 int、struct timeval 等）

# SO_REUSEADDR

```c
int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
// 必须在 bind() 之前调用
```

## 作用一：服务端重启后立即端口复用

服务端正常关闭后，监听端口会进入 **TIME_WAIT（2MSL）**，默认情况下这段时间内无法重新 bind 同一端口。开启 **SO_REUSEADDR** 可以直接绑定。

## 作用二：允许多个 socket 绑定同一端口（配合 **SO_REUSEPORT**）

> **SO_REUSEADDR vs SO_REUSEPORT**

| |SO_REUSEADDR|SO_REUSEPORT|
|---|---|---|
|主要用途|忽略 TIME_WAIT，快速重启|多进程/线程监听同一端口，内核负载均衡|
|内核分发|否|是（按四元组哈希分发到不同 socket）|
|典型场景|服务重启|Nginx worker 进程、多线程 accept|

---

# SO_KEEPALIVE

```c
int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
```

TCP 保活机制：在连接空闲一段时间后，内核自动发送探测包，检测对端是否存活。

## 默认参数（Linux）

```txt
tcp_keepalive_time    = 7200s   （空闲多久后开始探测）
tcp_keepalive_intvl   = 75s     （每次探测间隔）
tcp_keepalive_probes  = 9       （探测失败多少次后断开）
```

默认参数太长（2 小时才开始探测），实际应用通常在**应用层实现心跳**，而不是依赖系统 keepalive。

## 自定义 keepalive 参数

```cpp
int idle = 60, intvl = 10, cnt = 3;
setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,  &idle,  sizeof(idle));
setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,   &cnt,   sizeof(cnt));
```

## SO_KEEPALIVE vs 应用层心跳

|      | SO_KEEPALIVE | 应用层心跳      |
| ---- | ------------ | ---------- |
| 实现   | 内核自动         | 应用自行发送心跳包  |
| 灵活性  | 低（全局参数）      | 高（可按连接定制）  |
| 检测粒度 | TCP 连接存活     | 应用进程是否正常响应 |
| 推荐场景 | 兜底保障         | 生产环境主要机制   |

> 应用层心跳能检测到"进程卡死但 TCP 连接还在"的情况，SO_KEEPALIVE 检测不到。

---

## 关联笔记

- [socket, bind, listen, accept, connect (核心API)](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/04-Socket%20API%20(Socket编程接口)%20⭐/04a-socket,%20bind,%20listen,%20accept,%20connect%20(核心API).md)
- [Non-blocking Socket & O_NONBLOCK (非阻塞Socket)](/05-Network%20Programming%20(网络编程)/02%20·%20Socket编程/04-Socket%20API%20(Socket编程接口)%20⭐/04b-Non-blocking%20Socket%20&%20O_NONBLOCK%20(非阻塞Socket).md)
- [TCP⧸IP Stack Overview (协议栈总览)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/01-TCP⧸IP%20Stack%20Overview%20(协议栈总览).md)
- [Three-Way Handshake & Four-Way Teardown (三次握手四次挥手)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02a-Three-Way%20Handshake%20&%20Four-Way%20Teardown%20(三次握手四次挥手).md)
- [TCP State Machine (状态机全图)](/05-Network%20Programming%20(网络编程)/01%20·%20网络基础/02-TCP%20Deep%20Dive%20(TCP深入)%20⭐/02b-TCP%20State%20Machine%20(状态机全图).md)
