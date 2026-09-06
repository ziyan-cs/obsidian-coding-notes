---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 01-Sockets and Event Loop (Socket 与事件循环)

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

## Socket API and Options (Socket API与选项)

> [!note] 本节重点心考点：每个系统调用的作用、参数含义、服务端与客户端各自的调用流程

## 函数调用链

**TCP serve**

socket( ) → bind( ) → listen( ) → accept( ) → recv( ) / send( ) → close( )

**TCP client**

socket( ) → connect( ) → recv( ) / send( ) → close( )

```text
Server                    OS Kernel                Client
  │                         │                        │
  ├── socket() → fd ───────→│                        │
  ├── bind(port) ──────────→│                        │
  ├── listen() → backlog ──→│                        │
  │   (blocking)            │                        │
  │                         │←── socket() → fd ──────┤
  │                         │←── connect(ip:port) ───┤
  │←── three-way handshake ─┤                        │
  │                         ├── connect returns ────→│
  │                         │                        │
  ├── accept() → new_fd ───→│                        │
  │  Data transfer begins   │                        │
  │                         │←───── write(req) ──────┤
  │←─── data arrives ───────┤                        │
  │      → epoll notify     |                        │
  ├── read() → process() ──→│                        │
  ├── write(resp) ─────────→│                        │
  │                         ├── data arrives ───────→│
  │                         │←───── read(resp) ──────┤
  │                         │                        │
  │                         │←────── close() ────────┤
  ├──────── close() ───────→│                        │
```


---

## 接口语义

```cpp
#include <iostream>
#include <cstdio>         // 输入输出：printf, perror
#include <cstdlib>        // 通用：exit, malloc
#include <unistd.h>       // 系统调用：close, read, write
#include <sys/socket.h>   // 核心：socket, bind, listen, accept, connect...
#include <netinet/in.h>   // 结构：sockaddr_in, htons, htonl...
#include <arpa/inet.h>    // 转换：inet_ntoa, inet_addr...
```

## socket

- 创建一个套接字，返回文件描述符（fd）。本质上是在内核中创建了一个网络通信的端点。

```cpp
// int socket(int domain, int type, int protocol);

int fd = socket(AF_INET, SOCK_STREAM, 0); 
if (fd < 0) perror("socket failed");
```

- `domain`：协议族 / 地址族（通信范围和地址格式）
	- `AF_INET`：IPv4 通信
	- `AF_INET6`：IPv6 通信
	- `AF_UNIX`：本地进程间通信

- `type`：套接字类型（数据传输方式和可靠性保证）
	- `SOCK_STREAM`：可靠、有序、双线的字节流（TCP）
	- `SOCK_DGRAM`：不可靠、无序、有数据边界的数据报（UDP）
	- `SOCK_RAW`：原始套接字（直接操作 IP 层）

- `protocol`：具体协议（同一协议族下，进一步指定协议）
	- `0`：自动匹配协议（通常填 0）
	- `IPPROTO_TCP`：TCP 协议
	- `IPPROTO_UDP`：UDP 协议
	- `IPPROTO_IP`：IP 协议

## bind

将套接字绑定到一个本地地址（IP + 端口）。

- 服务端必须调用，指定监听的端口
- 客户端通常不调用，由内核自动分配临时端口

```cpp
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

struct sockaddr_in addr = {
	.sin_family = AF_INET,
	.sin_port = htons(8080),       // 转换为网络字节序
	.sin_addr.s_addr = INADDR_ANY  // 监听所有网卡
};
bind(fd, (struct sockaddr*)&addr, sizeof(addr));
```

> **htons / htonl**：将主机字节序转为网络字节序（大端序整数型）

- `addr`：通用地址结构指针
	- `sockaddr_in`：IPv4 地址结构体
	- `sockaddr_in6`：IPv6 地址结构体
	- `sockaddr_un`：Unix 域使用的结构体

## listen

套接字 “被动” 监听，开始接受连接请求。

- Linux 的半连接/accept 队列行为与溢出处理受内核版本和 sysctl 配置影响；可将其理解为“握手中请求”和“已建立但尚未被应用 accept 的连接”两类积压状态

```cpp
// 函数原型
int listen(int sockfd, int backlog);
```

- `backlog`：**全连接队列**（Accept Queue）的最大长度

```txt
  客户端 SYN 到达
       ↓
┌──────────────────────┐
│  Half-Open Queue     │  SYN_RCVD 状态，等待第三次握手 ACK
│  (SYN Queue)         │  大小由 /proc/sys/net/ipv4/tcp_max_syn_backlog 控制
└─────────┬────────────┘
          │ 三次握手完成
          ↓
┌──────────────────────┐
│  Full Connection Q   │  ESTABLISHED 状态，等待 accept() 取走
│  (Accept Queue)      │  大小 = min(backlog, somaxconn)
└─────────┬────────────┘
          │ accept() 调用
          ↓
        应用程序
```

> 队列满时的丢弃、重传或 RST 等行为依内核配置与状态而变；排查时应同时看 `backlog`、`somaxconn`、SYN 相关参数和应用是否及时 `accept`。

## accept

从全连接队列中取出一个已完成三次握手的连接，返回一个**新的 fd**。

```cpp
// 函数原型
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- `addr`：输出参数，存储客户端地址信息（可传 `NULL` ）

- `addrlen`：输入输出参数，地址结构体长度（可传 `NULL` ）

## connect

客户端发起连接，触发 TCP 三次握手。

- 阻塞模式下：握手完成（或失败）后才返回
- 非阻塞模式下：立即返回 `EINPROGRESS`，通过 epoll/select 检测连接完成

```cpp
// 函数原型
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- `addr`：指向服务器地址的结构指针

## read / write / recv / send

- `read/write` 是通用文件操作，`recv/send` 是 socket 专用，支持额外 flags
- `read` / `recv` **返回 0**：对端已对该方向 orderly shutdown（通常收到 FIN）；`write` / `send` 的 0 语义不同，仍须按调用与 errno 处理
- **返回值 = -1**：出错，检查 `errno`

```cpp
// 函数原型
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- `sockfd`：`accept()` 返回的已连接套接字（或客户端 `socket()` 返回的套接字）

- `buf`：数据缓冲区指针（发送时为 const，接收时为非 const）

- `len`：要发送 / 接收的数据长度

- `flags`：标志位，常用 `0`（默认阻塞读写）
    - `MSG_NOSIGNAL`：发送失败时不触发 SIGPIPE 信号（常用）

## close / shutdown

```cpp
int close(int fd); 
int shutdown(int sockfd, int how); 
// how: SHUT_RD / SHUT_WR / SHUT_RDWR
```

- 多进程 / 多线程环境下，需要确保所有进程都 close 后，套接字才会真正释放
- 最后一个引用关闭且未提前 `shutdown` 时通常开始该 socket 的关闭流程；具体报文交换取决于连接状态

## 30 秒回答

服务端的监听 fd 只负责接入，`accept` 返回的连接 fd 才负责读写；客户端通过 `connect` 完成建连。非阻塞 I/O 下每个系统调用都要区分成功、`EAGAIN/EWOULDBLOCK`、`EINTR`、EOF 和致命错误。`close` 管理 fd 引用，`shutdown` 管理连接方向，不能混为一谈。

|          | close()               | shutdown( )     |
| -------- | --------------------- | --------------- |
| 作用       | 减少 fd 引用计数，为 0 时才真正关闭 | 立即关闭指定方向        |
| 多进程 fork | close 只减引用计数          | shutdown 直接影响连接 |

---

Socket 编程进阶见 → [Non-blocking Socket & O_NONBLOCK (非阻塞Socket)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04b-Non-blocking%20Socket%20&%20O_NONBLOCK%20(非阻塞Socket).md>) · [SO_REUSEADDR & SO_KEEPALIVE (套接字选项)](</03-Backend%20Systems%20(后端系统)/02-Network%20(网络编程)/02-Socket%20Programming%20(Socket%20编程)/04-Socket%20API%20(Socket编程接口)%20⭐/04c-SO_REUSEADDR%20&%20SO_KEEPALIVE%20(套接字选项).md>)

---

## Nonblocking IO and Event Loop (非阻塞IO与事件循环)

> [!note] 本节重点心考点：阻塞 vs 非阻塞的行为差异、如何设置、如何正确处理 EAGAIN

## 阻塞 vs 非阻塞

|               | 阻塞模式        | 非阻塞模式                       |
| ------------- | ----------- | --------------------------- |
| read( ) 无数据   | 挂起线程，等待数据到来 | 立即返回 -1，errno = EAGAIN      |
| write( ) 缓冲区满 | 挂起线程，等待缓冲区空 | 立即返回 -1，errno = EAGAIN      |
| accept( ) 无连接 | 挂起线程，等待新连接  | 立即返回 -1，errno = EAGAIN      |
| connect( )    | 阻塞直到握手完成    | 立即返回 -1，errno = EINPROGRESS |

---

## 设置非阻塞

```cpp
// 方法一：创建时直接设置（推荐）
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

// 方法二：通过 fcntl 修改已有 fd
int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

---

## 非阻塞的正确使用模式

非阻塞 socket 配合 **I/O 多路复用**（select / poll / epoll）使用：

```cpp
while (true) {
    epoll_wait() → 获取就绪的 fd 列表
    for each 就绪的 fd:
        if 可读: read() 直到 EAGAIN（读完所有数据）
        if 可写: write() 直到 EAGAIN 或数据发完
        if 新连接: accept() 直到 EAGAIN（接受所有新连接）
}
```

> **为什么 ET 要循环读到 EAGAIN？**  
> epoll ET 模式只在“未就绪 → 就绪”变化时通知。若一次没有读到 `EAGAIN`，应用可能不再收到新的边沿通知，导致连接长期积压。LT 下通常仍应尽量批量读写以提高吞吐，但不必把“读到 EAGAIN”误解为唯一正确策略。

---

## EAGAIN vs EWOULDBLOCK

- POSIX 允许两者相同；Linux 上通常相同，但可移植代码应同时处理两者
- 语义：操作不能立即完成，但不是错误，稍后重试即可

## Socket Options and Connection Health (套接字选项与连接健康)

> [!note] 本节重点心考点：套接字选项高度依赖操作系统语义；区分 `SO_REUSEADDR`、`SO_REUSEPORT`、内核 keepalive 与应用层心跳。


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



## 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

## 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Sockets and Event Loop (Socket 与事件循环)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
