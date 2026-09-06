---
tags:
  - network/socket
status: 🌱
---

# Socket API Lifecycle — 核心 API

> [!abstract] 核心考点：每个系统调用的作用、参数含义、服务端与客户端各自的调用流程

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
