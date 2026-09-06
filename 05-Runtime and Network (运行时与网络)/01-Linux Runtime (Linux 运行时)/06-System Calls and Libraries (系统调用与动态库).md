---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 06-System Calls and Libraries (系统调用与动态库)

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

## 30 秒回答

**核心结论**：学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。


## System Calls Overview (常用系统调用)

> [!note] 本节重点心考点：文件/进程/网络/内存管理相关系统调用速查、用户态与内核态切换、系统调用开销

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

系统调用速查见 → [Dynamic Library & Shared Object (动态库原理)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/11-Dynamic%20Library%20&%20Shared%20Object%20(动态库原理)%20⭐.md) · [Debugging & Tracing (调试追踪)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/12-Debugging%20&%20Tracing：%20strace,%20gdb,%20perf%20(调试追踪)%20⭐.md)

---

## Dynamic Libraries and Shared Objects (动态库与共享对象)

> [!note] 本节重点心考点：动态库与静态库的区别、共享对象 (.so) 的加载与链接、PLT/GOT 与位置无关代码

## 静态库 vs 动态库

| 特性 | 静态库 (.a) | 动态库 (.so) |
|------|------------|-------------|
| 链接时机 | 编译期 | 加载期或运行期 |
| 可执行文件大小 | 大（包含库代码） | 小（仅引用） |
| 部署 | 独立运行 | 依赖目标机器有 .so |
| 更新 | 需要重新链接 | 替换 .so 即可 |
| 内存共享 | 不行（每个进程有副本） | 可以（物理内存共享一份代码） |
| 加载速度 | 快（已包含） | 略慢（需动态链接） |

**静态库创建：**
```bash
gcc -c foo.c -o foo.o
ar rcs libfoo.a foo.o
gcc main.c -L. -lfoo -o prog
```

**动态库创建：**
```bash
gcc -fPIC -shared foo.c -o libfoo.so
gcc main.c -L. -lfoo -o prog       # 编译时链接
LD_LIBRARY_PATH=. ./prog            # 运行时指定路径
```

## 位置无关代码（PIC）

动态库加载时基地址不确定（ASLR），因此代码中的地址引用不能是绝对地址。`-fPIC` 编译的代码使用**相对寻址**：

- **全局偏移表（GOT）**：存放全局变量和函数指针的表，加载时填入实际地址
- **过程链接表（PLT）**：延迟绑定，函数首次调用时才解析地址

```
程序调用函数时：
  1. call 跳转到 PLT 表项
  2. PLT 跳转到 GOT 表项
  3. 首次调用时 GOT 指向 PLT 解析函数
  4. 后续调用直接跳转到目标函数地址
```

## 动态库加载流程

```
1. 可执行文件启动
2. ld.so（动态链接器/加载器）被内核加载
3. ld.so 解析 ELF 中的 .dynamic 段
4. 查找并加载所有依赖的 .so 文件
5. 重定位（填充 GOT/PLT 表项）
6. 执行初始化函数（.init）
7. 跳转到程序入口
```

**查看依赖：** `ldd ./prog` | `readelf -d ./prog`

**加载路径搜索顺序：**
1. `LD_LIBRARY_PATH` 环境变量
2. `/etc/ld.so.cache`（由 ldconfig 更新）
3. `/lib`、`/usr/lib` 等系统目录

## 运行时加载（dlopen）

```c
#include <dlfcn.h>

void* handle = dlopen("./libfoo.so", RTLD_LAZY);
if (!handle) { fprintf(stderr, "%s\n", dlerror()); return; }

void (*func)() = dlsym(handle, "foo_func");
if (func) func();  // 调用动态库中的函数

dlclose(handle);
```

**编译：** `gcc main.c -ldl -o prog`

## PLT/GOT 延迟绑定解析

```
首次调用 func()：
  main  →  call func@plt
  func@plt → jmp *GOT[func]        // GOT 初始指向下一条指令
              push reloc_index      // 压入重定位索引
              jmp  resolver         // 跳转到动态链接器
              resolver: 查找 func 地址 → 写入 GOT[func]
                                     
后续调用 func()：
  main → call func@plt
  func@plt → jmp *GOT[func]         // 直接跳转到 func
```

> [!tip]- **工程要点**：`-fPIC` 对性能有轻微影响（多一次间接寻址），但这是动态库和 ASLR 的必要代价。如果不需要共享，静态库性能更优。`LD_PRELOAD` 环境变量可以劫持系统库函数——这是很多调试/监控工具的底层原理。

动态库原理见 → [System Calls Overview (常用系统调用速查)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/10-System%20Calls%20Overview%20(常用系统调用速查).md) · [Debugging & Tracing (调试追踪)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/04-System%20Calls%20&%20Tools%20(系统调用与工具)/12-Debugging%20&%20Tracing：%20strace,%20gdb,%20perf%20(调试追踪)%20⭐.md)



## 零基础阅读路径

先沿一条请求或系统调用的时间顺序阅读，给每一步标出状态、队列和所有者；协议字段与内核实现细节放在第二遍。先能讲清路径，再谈调优。

## 常见误区

- 只记协议或系统调用名，忽略状态变化、阻塞位置、资源释放与异常网络条件。
- 没有抓包、日志、压测或最小 client/server 实验就对性能和正确性下结论。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-System Calls and Libraries (系统调用与动态库)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
