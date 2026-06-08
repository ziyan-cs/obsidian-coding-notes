---
tags:
  - linux/io
status: 🌱
---

> **核心考点**：mmap 共享映射、sendfile 零拷贝、DMA 拷贝与 CPU 拷贝、零拷贝对性能的提升

## 传统 IO 的数据拷贝

传统 `read + write` 传输文件涉及 **4 次上下文切换 + 4 次数据拷贝（其中 2 次 DMA、2 次 CPU）**：

```
磁盘 → 内核缓冲区（DMA 拷贝）
内核缓冲区 → 用户缓冲区（CPU 拷贝）
用户缓冲区 → socket 缓冲区（CPU 拷贝）
socket 缓冲区 → 网卡（DMA 拷贝）
```

每次 CPU 拷贝都占用 CPU 时间，且用户态/内核态切换（read/write 系统调用）也有开销。

## mmap 减少一次拷贝

```c
// 将文件映射到进程地址空间
void *addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);

// 直接写入 socket（无需先 read 到用户缓冲区）
ssize_t n = write(sockfd, addr, length);

munmap(addr, length);
```

**拷贝次数：** 3 次（2 次 DMA + 1 次 CPU）
- 磁盘 → 内核缓冲区（DMA）
- 内核缓冲区 → socket 缓冲区（CPU）← 省去了一次用户缓冲区拷贝
- socket 缓冲区 → 网卡（DMA）

## sendfile 零拷贝

```c
#include <sys/sendfile.h>

// 在两个 fd 之间直接传输数据（完全在内核空间完成）
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

**拷贝次数：** 2 次（2 次 DMA，**0 次 CPU 拷贝**）→ 真正的零拷贝
- 磁盘 → 内核缓冲区（DMA）
- 内核缓冲区 → 网卡（DMA）

**适用场景：** 静态文件服务器、CDN、代理服务器。**不适用于**需要在传输前修改数据的场景。

## 零拷贝深入：scatter/gather

Linux 2.4+ 的 sendfile 进一步优化：内核缓冲区无需真正将数据拷贝到 socket 缓冲区，而是通过**描述符**直接告诉网卡数据位置（SG-DMA）：

```
磁盘 → 内核缓冲区（DMA 拷贝）
内核缓冲区 → 网卡（SG-DMA，零 CPU 拷贝）
```

## 各方案对比

| 方案 | CPU 拷贝 | DMA 拷贝 | 上下文切换 | 系统调用 |
|------|---------|---------|-----------|---------|
| read + write | 2 | 2 | 4 | 2 |
| mmap + write | 1 | 2 | 2 | 2 |
| sendfile | 0 | 2 | 2 | 1 |
| 带 scatter/gather 的 sendfile | 0 | 2 | 2 | 1 |

## splice：管道零拷贝

```c
// splice 在两个 fd 之间移动数据，不经过用户空间
int splice(int fd_in, loff_t *off_in, int fd_out,
           loff_t *off_out, size_t len, unsigned int flags);
```

用于任意两个 fd 之间的零拷贝数据传输（不限于文件到 socket）。

> **工程要点**：零拷贝的核心思路是避免数据在内核态和用户态之间来回拷贝。sendfile 对静态文件传输最有效，如果数据需要计算/修改（如压缩、加密），仍需要传统方式。Nginx 的静态文件服务正是利用 sendfile 实现高性能。

零拷贝与 mmap 详解见 → [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md) · [Process Lifecycle (生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)
