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