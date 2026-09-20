---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 系统调用通过用户态到内核态的受控入口访问进程、文件和网络资源；库函数可封装系统调用，也可能完全在用户态完成。动态链接还涉及符号解析、装载路径和 ABI 兼容。

# System Calls Overview (常用系统调用)

> [!note] 本节重点：文件/进程/网络/内存管理相关系统调用速查、用户态与内核态切换、系统调用开销

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

> [!tip]- **工程要点**：系统调用次数只是成本之一。`readv/writev` 可减少用户态拼接和调用数，`mmap` 改变文件访问路径，`epoll` 改变大量 fd 的就绪等待方式；这些不是彼此可直接替代的“加速开关”。先测瓶颈，再决定是否优化。


---

# Dynamic Libraries and Shared Objects (动态库与共享对象)

> [!note] 本节重点：动态库与静态库的区别、共享对象 (.so) 的加载与链接、PLT/GOT 与位置无关代码

## 静态库 vs 动态库

| 特性 | 静态库 (.a) | 动态库 (.so) |
|------|------------|-------------|
| 链接与装载 | 构建时把所需目标代码纳入可执行文件 | 构建时记录依赖，装载器在运行时查找并重定位 `.so` |
| 部署 | 减少外部 `.so` 依赖，但仍受内核/运行环境约束 | 需管理库路径、SONAME 与 ABI 兼容 |
| 更新 | 通常重新链接并重新发布可执行文件 | 可单独更新兼容的库；ABI 不兼容时仍需重建/协调发布 |
| 内存共享 | 同一可执行文件的只读映射仍可在进程间共享 | 同一 `.so` 的只读代码页也可共享 |
| 性能 | 大小与启动/运行开销取决于链接选项、重定位和访问模式 | 不能仅凭静态/动态形式断言谁更快 |

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

动态库可能装入不同虚拟地址；`-fPIC` 生成适合共享对象重定位的代码，尽量减少对只读代码页的运行时修改。具体寻址方式与 GOT/PLT 使用取决于架构、编译器和链接选项，不是“所有绝对地址都不允许”：

- **全局偏移表（GOT）**：存放全局变量和函数指针的表，加载时填入实际地址
- **过程链接表（PLT）**：某些外部函数调用路径会经过它；是否延迟绑定取决于 `BIND_NOW` 等配置

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

**查看依赖：** 可信程序可用 `ldd ./prog` 观察实际解析到的库；未知来源二进制不要运行 `ldd`，先用 `readelf -d ./prog` 或 `objdump -p` 看静态 `NEEDED` 条目（不展示完整传递依赖）。

**加载路径搜索：** GNU/Linux 动态加载器还考虑 ELF 的 `DT_RPATH` / `DT_RUNPATH`、`LD_LIBRARY_PATH`、`/etc/ld.so.cache` 与默认目录；它们的优先级及传递依赖行为不同，安全执行模式还会忽略某些环境变量。排查时按 [ld.so(8)](https://man7.org/linux/man-pages/man8/ld.so.8.html) 核对目标系统，不背上面三项的固定顺序。

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

> [!tip]- **工程要点**：`-fPIC`、懒绑定、`LD_PRELOAD` 与库搜索路径都可能影响性能、安全或可复现性，但不能笼统断言静态库总是更快。部署时固定 ABI 与库版本，避免生产进程从可写的未知目录加载 `.so`。


[ld.so(8) Linux manual](https://man7.org/linux/man-pages/man8/ld.so.8.html) · [ldd(1) security note](https://man7.org/linux/man-pages/man1/ldd.1.html)

> [!info]- 延伸阅读
> - 下一步：[13-Debugging and Tracing (调试与追踪)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/13-Debugging%20and%20Tracing%20(调试与追踪).md)

