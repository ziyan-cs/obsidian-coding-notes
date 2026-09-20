---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

# Zero Copy and sendfile (零拷贝与 sendfile)

> [!note] 本节重点：mmap 共享映射、sendfile 零拷贝、DMA 拷贝与 CPU 拷贝、零拷贝对性能的提升

> [!warning] “零拷贝”是相对用户态 CPU copy 的工程术语
> 实际数据路径受内核版本、文件系统、TLS、网卡卸载与硬件能力影响。不要把“0 次 CPU 拷贝”当作任何机器上的保证；应以目标环境的 profile、吞吐和尾延迟决定是否使用。

# 传统 IO 的数据拷贝

在经典“磁盘文件经用户缓冲区发送到网卡”的简化模型里，`read + write` 涉及两次用户/内核之间的 CPU 数据复制；下面的箭头只用于比较路径，**不是每次请求固定发生的物理拷贝次数**。页缓存命中、设备 DMA、TLS 与网络卸载都会改变真实路径。

```
磁盘 → 内核缓冲区（DMA 拷贝）
内核缓冲区 → 用户缓冲区（CPU 拷贝）
用户缓冲区 → socket 缓冲区（CPU 拷贝）
socket 缓冲区 → 网卡（DMA 拷贝）
```

每次 CPU 拷贝都占用 CPU 时间，且用户态/内核态切换（read/write 系统调用）也有开销。

# mmap 减少一次拷贝

```c
// 将文件映射到进程地址空间
void *addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);

// 直接写入 socket（无需先 read 到用户缓冲区）
ssize_t n = write(sockfd, addr, length);

munmap(addr, length);
```

**简化模型：** 映射让应用访问页缓存，省去 `read` 填充用户缓冲区的一次 CPU 复制；`write` 仍可能把数据复制到 socket 路径。
- 磁盘 → 内核缓冲区（DMA）
- 内核缓冲区 → socket 缓冲区（CPU）← 省去了一次用户缓冲区拷贝
- socket 缓冲区 → 网卡（DMA）

# sendfile 零拷贝

```c
#include <sys/sendfile.h>

// 在两个 fd 之间直接传输数据（完全在内核空间完成）
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

**概念模型：** 通常可避免用户态缓冲区相关的 CPU copy；是否还能以 scatter/gather 等方式减少内核缓冲区 copy，取决于内核与设备能力。
- 磁盘 → 内核缓冲区（DMA）
- 内核缓冲区 → 网卡（DMA）

**适用场景：** 静态文件服务器、CDN、代理服务器。**不适用于**需要在传输前修改数据的场景。

# 零拷贝深入：scatter/gather

某些内核、设备与发送路径可以让 `sendfile` 通过页引用或 scatter/gather 进一步减少内核内的数据复制；这是**可能的实现优化**，不是 Linux 2.4+ 在任何文件、socket 和 TLS 配置上的统一保证：

```
磁盘 → 内核缓冲区（DMA 拷贝）
内核缓冲区 → 网卡（SG-DMA，零 CPU 拷贝）
```

# 各方案对比（概念模型）

| 方案 | 用户缓冲区 CPU copy | 主要限制 |
|------|---------------------|----------|
| `read` + `write` | 通常需要 | 可由应用解析、修改数据；必须处理短读写 |
| `mmap` + `write` | 省去文件到用户缓冲区的一次复制 | 映射生命周期、页错误与 `write` 路径仍有成本 |
| `sendfile` | 通常避免 | 输入 fd 类型有限，可能短传输，TLS/设备能力影响路径 |

# splice：管道零拷贝

```c
// splice 在两个 fd 之间移动数据，不经过用户空间
ssize_t splice(int fd_in, loff_t *off_in, int fd_out,
               loff_t *off_out, size_t len, unsigned int flags);
```

两个 fd 中至少一个必须是 pipe；可借助 pipe 在受支持的 fd 之间转移数据，但不是“任意两个 fd 直接互传”。还需处理短传输、`EAGAIN` 和文件系统不支持等失败。

> [!tip]- **工程要点**：零拷贝的核心思路是避免不必要的用户态数据搬运。`sendfile` 常适合静态文件到 socket 的直通路径；一旦需要在应用层查看或修改 body（例如内容转换），就需要不同的数据路径。TLS/协议栈配置也可能改变实际收益，先测量再选择。

> [!summary] 核心摘要
>
> `mmap` 让文件页映射进进程虚拟地址空间，应用仍可读写该映射；`sendfile` 让内核在文件与 socket 之间组织传输，通常避免用户缓冲区 copy。它们解决的是数据搬运成本，不替代缓存策略、网络瓶颈或应用层处理；“是否更快”必须针对真实文件大小、TLS 和网卡环境验证。
>
> 实施前查 [sendfile(2)](https://man7.org/linux/man-pages/man2/sendfile.2.html) 与 [splice(2)](https://man7.org/linux/man-pages/man2/splice.2.html) 的 fd 限制，并在目标环境测量。
