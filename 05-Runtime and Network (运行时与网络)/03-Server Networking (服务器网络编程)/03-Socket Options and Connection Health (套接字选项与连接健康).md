---
tags:
  - network/socket
status: 🌱
---

> [!abstract] 核心考点：套接字选项高度依赖操作系统语义；区分 `SO_REUSEADDR`、`SO_REUSEPORT`、内核 keepalive 与应用层心跳。

# Socket Options — 套接字选项

## setsockopt

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

## SO_REUSEADDR

```c
int opt = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
// 必须在 bind() 之前调用
```

## 作用一：服务端重启后立即端口复用

是否可重绑及冲突规则受操作系统、绑定地址和已有 socket 状态影响。`SO_REUSEADDR` 常用于降低服务重启时的地址占用影响，但它不是跨平台的“无条件端口复用”开关；部署前应按目标系统验证。

## 作用二：允许多个 socket 绑定同一端口（配合 **SO_REUSEPORT**）

> **SO_REUSEADDR vs SO_REUSEPORT**

| |SO_REUSEADDR|SO_REUSEPORT|
|---|---|---|
|主要用途|忽略 TIME_WAIT，快速重启|多进程/线程监听同一端口，内核负载均衡|
|内核分发|否|是（按四元组哈希分发到不同 socket）|
|典型场景|服务重启|Nginx worker 进程、多线程 accept|

---

## SO_KEEPALIVE

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

> 应用层心跳可以定义业务级超时与响应语义；内核 keepalive 只观察 TCP 层连通性。两者可组合使用，具体参数应按网络环境和故障检测目标配置。

## 30 秒回答

**`SO_REUSEADDR` 和 `SO_REUSEPORT` 有何区别？** 前者主要影响地址重用语义，后者在支持的平台上允许多个监听 socket 共享地址并参与内核分发；两者都不能脱离操作系统语义单独背结论。keepalive 是 TCP 层故障探测，不能替代业务心跳。

---

Socket API 基础见 → [socket, bind, listen, accept, connect (核心API)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04a-socket,%20bind,%20listen,%20accept,%20connect%20(核心API).md>) · 非阻塞模式见 → [Non-blocking Socket & O_NONBLOCK (非阻塞Socket)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04b-Non-blocking%20Socket%20&%20O_NONBLOCK%20(非阻塞Socket).md>)
