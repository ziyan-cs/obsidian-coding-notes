


# 1. 整体流程（🔥最重要）  

### TCP server 生命周期  

>socket( ) → bind( ) → listen( ) → accept( ) →
>read( ) / write( ) → close( )

### TCP client 生命周期

>socket( ) → connect( ) →
>write( ) / read( ) → close( )
  
# 2. 通信模型  

### 一次请求的完整流转

>建立连接 →
>     客户端发送请求 → 服务器接收请求 →
>     服务器处理并响应 → 客户端接收响应 →
>断开连接

 1. 客户端调用 `connect()`，与服务器完成三次握手，建立 TCP 连接

2. 客户端调用 `write()`，数据通过内核协议栈层层封装后发送给服务器

3. 服务器从 `accept()` 返回的套接字中 `read()` 数据

4. 服务器处理请求，调用 `write()` 发送响应数据

5. 客户端调用 `read()` 获取服务器响应

6. 双方完成数据交互后，调用 `close()`，通过四次挥手断开连接

### client → server 的数据传输过程

>客户端应用层 → 传输层 → 网络层 → 数据链路层 → 
>     物理层 → 服务器物理层 →
> 数据链路层 → 网络层 → 传输层 → 服务器应用层

1. 客户端应用层：调用 `write()` 写入数据，数据进入内核发送缓冲区

2. 传输层：加上 TCP 头部（端口、序列号、校验和）

3. 网络层：加上 IP 头部（IP 地址），选择路由下一跳

4. 数据链路层：加上 MAC 头部，通过 ARP 找到下一跳 MAC 地址

5. 物理层：转为比特流，通过网络设备传输

6. 服务器端：从物理层到应用层逐层解封装，最终数据被拷贝到服务器 read() 的缓冲区

# 3. 接口语义（🔥）

```c
#include <stdio.h>        // 输入输出：printf, perror
#include <stdlib.h>       // 通用：exit, malloc
#include <unistd.h>       // 系统调用：close, read, write
#include <sys/socket.h>   // 🔥 核心：socket, bind, listen, accept, connect...
#include <netinet/in.h>   // 🔥 结构：sockaddr_in, htons, htonl...
#include <arpa/inet.h>    // 🔥 转换：inet_ntoa, inet_addr...
```

### socket

- 作用：创建一个服务端 / 客户端套接字
	- 返回文件描述符 ( `Linux` ) / 的句柄 ( `Windows` )

```c
// 函数原型
int socket(int domain, int type, int protocol);

// 创建 TCP 套接字
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) {
    perror("socket error");
    exit(EXIT_FAILURE);
}
```

- `domain`：通信域（协议族 / 地址族 ）
	- 规定通信范围和地址格式
	- `AF_INET`：IPv4 互联网通信
	- `AF_INET6`：IPv6 互联网通信
	- `AF_UNIX`：本地进程间通信
		- 地址格式：文件路径 `/tmp/socket`
	- 告诉内核：“使用什么规则来识别通信双方”

- `type`：套接字类型
	- 规定数据传输方式和可靠性保证
	- `SOCK_STREAM`：可靠、有序、双线的字节流（默认 TCP）
	- `SOCK_DGRAM`：不可靠、无序、有数据边界的数据报（默认 UDP）
	- `SOCK_RAW`：原始套接字（直接操作 IP 层）
	- 告诉内核：“需要什么服务来传输数据”

- `protoal`：具体协议
	- 同一 `domain` 同一 `type`，进一步指定协议
	- `0`：选择默认协议（大部分情况选择 `0` ）
	- `IPPROTO_TCP`：明确指定 TCP
	- `IPPROTO_UDP`：明确指定 UDP
	- `IPPROTO_IP`：原始 IP 协议
	- 当 `type` 不够精确，用 `protocol` 细化

### bind

- 作用：将套接字与服务器的特定 IP 地址和端口号绑定。
	- 客户端系统自动分配一个临时端口，一般不需要 `bind()`

```c
// 函数原型
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// 绑定 IPv4 地址和端口
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(8080); // 主机字节序转网络字节序
addr.sin_addr.s_addr = INADDR_ANY; // 绑定本机所有网卡(0.0.0.0)
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    close(sockfd);
    exit(EXIT_FAILURE);
}
```

- `sockfd`：`socket()` 函数返回的文件描述符

- `addr`：通用地址结构指针
	- `sockaddr_in`：IPv4 使用的结构替
	- `sockaddr_in6`：IPv6 使用的结构体
	- `sockaddr_un`：Unix 域使用的结构体

- `addrlen`：地址结构体的实际长度

### listen

- 作用：将套接字状态从 `CLOSED` 变为 `LISTEN`
	- 内核为该套接字维护两个队列，处理 TCP 三次握手
	- 收到的 `SYN` 包由内核自动处理握手，完成的连接放入队列

```c
// 函数原型
int listen(int sockfd; int backlog);

// 开启监听
if (listen(sockfd, 128) == -1) { // backlog 设为 128 (常用值)
    perror("listen error");
    close(sockfd);
    exit(EXIT_FAILURE);
}
```

- `sockfd`：`socket` 函数返回的文件描述符

- `backlog`：未完成队列 + 已完成队列的最大长度
	- 未完成队列：`SYN` 已接收，`SYN+ACK` 未发送
	- 已完成队列：三次握手完成，等待 `accept()` 取走

### accept 

- 作用：从已完成连接队列中取出一个连接，创建新的套接字用于通信

```c
// 函数原型
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 接受客户端连接
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
if (connfd == -1) {
    perror("accept error");
    continue;
}
printf("new connection from %s:%d\n",
       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
```

- `addr`：输出参数，存储客户端地址信息（可传 `NULL` ）

- `addrlen`：输入输出参数，地址结构体长度（可传 `NULL` ）

### connect


### read / recv


### write / send


# 4. 代码模板（🔥）  
### TCP server 模板  
### TCP client 模板  
  
# 5. 问题分析（🔥）  
### 阻塞分析（accept/recv/connect）
### 单客户端限制问题（串行模型缺陷）