---
tags:
  - linux
  - process
---

> **核心考点**：匿名管道 pipe、命名管道 fifo、共享内存 mmap、信号量与消息队列等 IPC 机制对比
> 核心考点：各 IPC 机制的特点、适用场景、使用方式

## IPC 机制对比

|机制|数据方向|有无名字|跨主机|特点|
|---|---|---|---|---|
|匿名管道 pipe|单向|无|❌|只能用于有亲缘关系的进程|
|命名管道 FIFO|单向|有|❌|任意进程可用，以路径标识|
|共享内存 mmap|双向|可有|❌|最快，需自行同步|
|消息队列|双向|有|❌|有结构，按消息读取|
|信号量|—|有|❌|同步原语，不传数据|
|Socket|双向|有|✅|最通用，可跨主机|

---

## 匿名管道（pipe）

```c
int pipefd[2];
pipe(pipefd);        // pipefd[0] = 读端，pipefd[1] = 写端

pid_t pid = fork();
if (pid == 0) {
    // 子进程：写
    close(pipefd[0]);              // 关闭不用的读端
    write(pipefd[1], "hello", 5);
    close(pipefd[1]);
    exit(0);
} else {
    // 父进程：读
    close(pipefd[1]);              // 关闭不用的写端
    char buf[10];
    read(pipefd[0], buf, sizeof(buf));
    printf("Received: %s\n", buf);
    wait(NULL);
}
```

- 内核缓冲区通常 64KB，写满则阻塞
- 写端关闭后，读端 read() 返回 0（EOF）
- Shell 的 `|` 管道就是用 pipe 实现的

---

## 命名管道（FIFO）

```c
mkfifo("/tmp/myfifo", 0666);     // 创建命名管道（也可用 shell: mkfifo /tmp/myfifo）

// 进程 A（写）
int fd = open("/tmp/myfifo", O_WRONLY);
write(fd, "hello", 5);

// 进程 B（读）
int fd = open("/tmp/myfifo", O_RDONLY);
char buf[10];
read(fd, buf, sizeof(buf));
```

- `open()` 时会阻塞，直到另一端也 open（读写端都就绪才能通信）

---

## 共享内存（mmap）

最高效的 IPC 方式，零拷贝，多个进程直接读写同一块物理内存：

```c
// 创建共享内存（基于文件）
int fd = open("/tmp/shm", O_RDWR | O_CREAT, 0666);
ftruncate(fd, 4096);
void *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

// 写入
memcpy(addr, "hello shared memory", 19);

// 另一进程打开同一文件并 mmap，即可读取
// ...

// 释放
munmap(addr, 4096);
close(fd);
```

**也可用匿名共享内存（父子进程间）：**

```c
void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
// fork 后，父子进程共享这块内存
```

> 共享内存本身没有同步机制，必须配合**信号量或互斥锁**使用，防止并发读写冲突。

---

## 关联笔记

- [Process Lifecycle： fork, exec, wait (生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)
- [Process States & Scheduling (状态与调度)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)
- [Zombie & Orphan Process (僵尸进程与孤儿进程)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04c-Zombie%20&%20Orphan%20Process%20(僵尸进程与孤儿进程).md)
- [Shell & Basic Commands (命令行与Shell编程)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md)
- [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md)
