---
tags:
  - linux/syscall
status: 🌱
---

# System Calls Overview — 常用系统调用速查

> [!important] **核心考点**：文件/进程/网络/内存管理相关系统调用速查、用户态与内核态切换、系统调用开销

## 系统调用的成本

系统调用涉及**用户态→内核态→用户态**的上下文切换，开销约几十到几百纳秒。主要成本：
1. 保存/恢复寄存器
2. 内核态栈切换
3. TLB / Cache 刷新
4. 权限检查

## 文件 IO

| 系统调用 | 作用 | 关键参数/标志 |
|---------|------|-------------|
| `open(path, flags, mode)` | 打开/创建文件 | O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND |
| `close(fd)` | 关闭文件描述符 | — |
| `read(fd, buf, count)` | 从文件读取数据 | 返回实际读取字节数 |
| `write(fd, buf, count)` | 写入文件 | 返回实际写入字节数 |
| `lseek(fd, offset, whence)` | 移动文件偏移 | SEEK_SET, SEEK_CUR, SEEK_END |
| `fsync(fd)` | 刷新到磁盘 | 确保数据持久化 |
| `fcntl(fd, cmd, ...)` | 控制 fd 属性 | F_SETFL（设置非阻塞）, F_GETFL |
| `stat(path, buf)` | 获取文件元信息 | 大小、权限、时间戳 |

## 进程管理

| 系统调用 | 作用 | 说明 |
|---------|------|------|
| `fork()` | 创建子进程 | 写时拷贝，返回子进程 PID |
| `execvp(file, argv)` | 替换进程映像 | 不创建新进程 |
| `exit(status)` | 终止进程 | 返回退出码 |
| `wait(&status)` | 等待子进程结束 | 阻塞等待 |
| `waitpid(pid, &status, options)` | 等特定子进程 | WNOHANG 非阻塞 |
| `getpid()` | 获取进程 PID | — |
| `kill(pid, sig)` | 发送信号 | — |

## 内存管理

| 系统调用 | 作用 | 说明 |
|---------|------|------|
| `brk(addr)` / `sbrk(increment)` | 调整堆内存 | malloc 底层使用 |
| `mmap(addr, length, prot, flags, fd, offset)` | 映射内存 | MAP_SHARED, MAP_PRIVATE, MAP_ANONYMOUS |
| `munmap(addr, length)` | 解除映射 | — |
| `mprotect(addr, len, prot)` | 设置内存权限 | PROT_READ, PROT_WRITE, PROT_EXEC |
| `mlock(addr, len)` | 锁定内存（不换页） | 实时程序使用 |

## 网络

| 系统调用 | 作用 | 说明 |
|---------|------|------|
| `socket(domain, type, protocol)` | 创建套接字 | AF_INET, SOCK_STREAM / SOCK_DGRAM |
| `bind(sockfd, addr, addrlen)` | 绑定地址 | 服务器端 |
| `listen(sockfd, backlog)` | 监听连接 | backlog 是连接队列长度 |
| `accept(sockfd, addr, addrlen)` | 接受连接 | 返回新 fd |
| `connect(sockfd, addr, addrlen)` | 发起连接 | 客户端 |
| `send(sockfd, buf, len, flags)` / `recv` | 发送/接收数据 | — |
| `setsockopt(sockfd, level, opt, val, len)` | 设置选项 | SO_REUSEADDR, TCP_NODELAY |
| `ioctl(fd, request, ...)` | 设备控制 | 获取网络接口信息 |

## 文件描述符操作

| 系统调用 | 作用 |
|---------|------|
| `dup(oldfd)` | 复制 fd |
| `dup2(oldfd, newfd)` | 复制到指定 fd |
| `pipe(pipefd[2])` | 创建管道 |
| `select(nfds, readfds, writefds, exceptfds, timeout)` | IO 多路复用（可移植） |
| `poll(fds, nfds, timeout)` | IO 多路复用（无上限） |
| `epoll_create1(flags)` / epoll_ctl / epoll_wait | epoll 系列（Linux） |

## 信号与定时

| 系统调用 | 作用 |
|---------|------|
| `signal(sig, handler)` | 注册信号处理函数 |
| `sigaction(sig, act, old)` | 注册信号（更可控） |
| `alarm(seconds)` | 设置定时信号 |
| `setitimer(which, new, old)` | 更精确的定时器 |
| `nanosleep(req, rem)` | 高精度睡眠 |

> [!tip]- **工程要点**：频繁系统调用是性能瓶颈之一。减少系统调用的技术包括：用户态缓冲区（stdio 的 fread/fwrite）、批量处理（readv/writev 聚集 IO）、mmap 减少 read/write、epoll 替代 select/poll。

系统调用速查见 → [Dynamic Library & Shared Object (动态库原理)](/04-Linux%20&%20System%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/11-Dynamic%20Library%20&%20Shared%20Object%20(动态库原理)%20⭐.md) · [Debugging & Tracing (调试追踪)](/04-Linux%20&%20System%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/12-Debugging%20&%20Tracing：%20strace,%20gdb,%20perf%20(调试追踪)%20⭐.md)
