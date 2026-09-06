---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Processes and IPC (进程与进程间通信)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## 30 秒回答

**02-Processes and IPC (进程与进程间通信)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## Process Lifecycle (进程生命周期)

> [!abstract] 核心考点：fork 写时拷贝、exec 系列替换进程映像、wait/waitpid 回收子进程、进程生命周期

## 进程的本质

进程是**程序的一次运行实例**，操作系统资源分配的基本单位。每个进程有独立的：

- 虚拟地址空间（代码段、数据段、堆、栈）
- 文件描述符表
- 信号处理表
- PID、PPID（父进程 ID）

**生命周期：** 创建 → 就绪 → 运行 → 阻塞 → 终止 → 回收

---

## fork() 🔥

```c
#include <unistd.h>
pid_t pid = fork();
```

`fork()` 创建一个与父进程几乎完全相同的子进程。返回值：
- 父进程：返回子进程 PID（> 0）
- 子进程：返回 0
- 失败：返回 -1

```c
pid_t pid = fork();
if (pid < 0) {
    perror("fork failed");
} else if (pid == 0) {
    // 子进程
    printf("Child PID: %d, Parent PID: %d\n", getpid(), getppid());
    exit(0);
} else {
    // 父进程
    printf("Parent, child PID: %d\n", pid);
}
```

**父子进程的区别：**

| 相同点 | 不同点 |
|--------|--------|
| 代码段（共享只读） | PID / PPID |
| 环境变量 | fork() 返回值 |
| 打开的文件描述符（初始指向相同 open file description，因而通常共享文件偏移） | 各自 PCB |
| 信号处理设置 | 各自挂起的信号 |
| 当前工作目录 | 各自的 PID、返回值与地址空间后续写入结果 |

### Copy-On-Write（COW，写时拷贝）

fork 之后，父子进程**共享同一份物理内存页**，并不立即复制。只有当某方尝试**写入**时，才触发缺页中断，内核将该页复制一份。

```
fork 后：
父进程页表 ──┐
            ├──→ 共享物理页（只读标记）
子进程页表 ──┘

某方写入时：
写入方 ──→ 缺页中断 ──→ 内核复制该页 ──→ 写入方使用新副本
另一方 ──→ 继续使用原来的页
```

- ✅ fork 速度快，不用复制全部内存
- ❌ 写入时有复制开销（大数据结构 fork 后立即写代价高）
- 💡 常用优化：fork 后立即 exec（新程序不会触发父进程内存的拷贝）

---

## exec 族函数

`exec` 用一个新程序**替换当前进程的地址空间**，但保留 PID：

```c
#include <unistd.h>

execl("/bin/ls", "ls", "-l", NULL);        // 列出参数
execlp("ls", "ls", "-l", NULL);            // 在 PATH 中查找
execv("/bin/ls", (char *[]){"ls", "-l", NULL});  // 参数用数组
execvp("ls", (char *[]){"ls", "-l", NULL});       // PATH + 数组
execve("/bin/ls", argv, envp);             // 完整版（可指定环境变量）
```

**关键点：**
- exec 成功后，原程序的代码段、数据段、堆栈全部被替换
- exec 失败才会返回（返回 -1）
- 打开的文件描述符默认继承（除非设置 `FD_CLOEXEC`）

**fork + exec = Shell 的工作方式：**

```c
pid_t pid = fork();
if (pid == 0) {
    // 子进程：执行新程序
    execvp("gcc", args);
    perror("exec failed");   // 只有 exec 失败才会执行到这里
    exit(1);
} else {
    wait(NULL);   // 父进程等待子进程结束
}
```

---

## wait / waitpid

父进程必须调用 `wait` 来回收子进程资源，否则子进程变成**僵尸进程**：

```c
#include <sys/wait.h>

pid_t wait(int *status);                          // 等待任意子进程，阻塞
pid_t waitpid(pid_t pid, int *status, int opts);  // 等待指定子进程
```

**检查退出状态：**

```c
int status;
pid_t child = waitpid(pid, &status, 0);

if (WIFEXITED(status))                       // 正常退出
    printf("exit code: %d\n", WEXITSTATUS(status));

if (WIFSIGNALED(status))                     // 被信号杀死
    printf("signal: %d\n", WTERMSIG(status));

if (WIFSTOPPED(status))                      // 被暂停
    printf("stop signal: %d\n", WSTOPSIG(status));
```

**非阻塞等待：**

```c
pid_t ret = waitpid(-1, &status, WNOHANG);   // 非阻塞，没有子进程退出返回 0
```

**循环回收所有子进程：**

```c
while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    printf("Child %d reaped\n", pid);
}
```

---

## 进程状态

```
              fork()
CREATED ───────────────────> READY
                               │
  Scheduler selects process    │ Wait for I/O / Signal
              ↓                ↓
           RUNNING ─────────> BLOCKED
              │                    │
              │ Time slice expires | Condition satisfied
              ↓                    ↓
            READY <────────────────┘
              │
              │ exit()
              ↓
           ZOMBIE ── wait() ──> Terminated
```

| 状态 | ps 显示 | 含义 |
|------|---------|------|
| 运行中 | R | 正在 CPU 执行，或在运行队列中等待调度 |
| 可中断睡眠 | S | 等待事件（I/O、信号），可被信号唤醒 |
| 不可中断睡眠 | D | 等待内核 I/O（如磁盘），不能被信号中断 ⚠️ |
| 暂停 | T | 被 SIGSTOP/SIGTSTP 暂停 |
| 僵尸 | Z | 已退出但父进程未 wait 回收 |

> **D 状态** 很危险：进程无法被 `kill`，通常意味着磁盘 I/O 卡住或 NFS 挂载问题，只能等待或重启。
> **Z 状态** 需要父进程调用 `wait()` 回收，否则占用 PID 资源。

---

## Linux 调度（调度器概念）

Linux 的公平调度实现会随内核版本演进（例如 CFS/EEVDF 等），这里保留“按权重分配 CPU 时间”的心智模型；当前系统行为需查所用内核文档（NEEDS_VERIFY）：

- 以**虚拟运行时间（vruntime）** 为核心指标，vruntime 最小的进程优先运行
- 旧版 CFS 以红黑树和虚拟运行时间组织可运行任务；不要把这一实现细节当作所有内核版本的固定事实
- **nice 值**（-20 ~ 19）影响 vruntime 增长速度：nice 越低，获得更多 CPU 时间

```bash
nice -n -10 ./myapp     # 以高优先级启动
renice 5 -p 1234        # 修改运行中进程的 nice 值
```

| 概念 | 范围 | 说明 |
|------|------|------|
| PR（Priority） | 0~39 | 内核实际调度优先级（值越低优先级越高） |
| NI（Nice） | -20~19 | 用户设置的优先级偏移量 |
| 最终优先级 | PR = NI + 20 | CFS 将其映射到 vruntime 权重 |

进程生命周期详解见 → [Process States & Scheduling (状态与调度)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md) · [Zombie & Orphan Process (僵尸进程与孤儿进程)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04c-Zombie%20&%20Orphan%20Process%20(僵尸进程与孤儿进程).md)

---

## Process States and Scheduling (进程状态与调度)

> [!abstract] 核心考点：进程三态/五态模型、就绪/运行/阻塞状态切换、Linux 调度策略与优先级

## 进程状态（补充 2）

```
              fork()
CREATED ───────────────────> READY
                               │
  Scheduler selects process    │ Wait for I/O / Signal
              ↓                ↓
           RUNNING ─────────> BLOCKED
              │                    │
              │ Time slice expires | Condition satisfied
              ↓                    ↓
            READY <────────────────┘
              │
              │ exit()
              ↓
           ZOMBIE ── wait() ──> Terminated
```

|状态|ps 显示|含义|
|---|---|---|
|Running|R|正在 CPU 上执行，或在运行队列中|
|Sleeping（可中断）|S|等待事件（I/O、信号），可被信号唤醒|
|Sleeping（不可中断）|D|等待内核 I/O（如磁盘），**不能被信号中断**|
|Stopped|T|被 SIGSTOP / SIGTSTP 暂停|
|Zombie|Z|已退出但父进程未 wait|

> **D 状态（不可中断睡眠）** 很危险：进程无法被 kill，通常意味着磁盘 I/O 卡住或 NFS 挂载问题，只能等待或重启。

## Linux 调度器（CFS）

Linux 默认使用 **CFS（Completely Fair Scheduler，完全公平调度器）**：

- 以**虚拟运行时间（vruntime）** 为核心指标，vruntime 最小的进程优先运行
- 用**红黑树**组织所有就绪进程，最左节点（vruntime 最小）即下一个运行的进程
- nice 值（-20 ~ 19）影响 vruntime 增长速度：nice 越低（优先级越高），vruntime 增长越慢，获得更多 CPU 时间

```bash
nice -n -10 ./myapp    # 以高优先级启动
renice 5 -p 1234       # 修改运行中进程的 nice 值
```

进程状态与调度详解见 → [Process Lifecycle (生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md) · [Zombie & Orphan Process (僵尸进程与孤儿进程)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04c-Zombie%20&%20Orphan%20Process%20(僵尸进程与孤儿进程).md)

---

## Zombie and Orphan Processes (僵尸进程与孤儿进程)

> [!abstract] 核心考点：僵尸进程产生原因与危害、孤儿进程的 init 收养、SIGCHLD 信号处理

## 僵尸进程（Zombie）

子进程已经退出，但父进程没有调用 `wait()` 回收其资源，子进程的 PCB（进程控制块）仍留在内核中，状态显示为 `Z`。

**危害：** 占用 PID，PID 耗尽后无法创建新进程。

**产生原因：**

```c
// 父进程没有 wait，直接去做其他事
pid_t pid = fork();
if (pid == 0) {
    exit(0);      // 子进程退出
}
// 父进程没有调用 wait()，子进程变成僵尸
while (1) sleep(1);
```

**解决方法：**

```c
// 方法一：父进程正确调用 wait/waitpid
waitpid(-1, NULL, WNOHANG);   // 非阻塞回收所有已退出子进程

// 方法二：忽略 SIGCHLD 信号（让内核自动回收）
signal(SIGCHLD, SIG_IGN);

// 方法三：注册 SIGCHLD 处理函数
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);  // 循环回收，处理多个子进程同时退出
}
signal(SIGCHLD, sigchld_handler);
```

---

## 孤儿进程（Orphan）

父进程先于子进程退出，子进程成为孤儿，被 **init 进程（PID=1，现代系统为 systemd）** 收养。

- 孤儿进程由 init 负责 wait 回收，**不会产生僵尸**，相对无害
- 守护进程（Daemon）本质上就是主动制造的孤儿进程

**守护进程化的标准步骤：**

```c
// 1. fork，父进程退出（子进程被 init 收养，脱离终端控制）
if (fork() > 0) exit(0);

// 2. setsid：创建新会话，脱离控制终端
setsid();

// 3. 再次 fork（防止重新获取控制终端）
if (fork() > 0) exit(0);

// 4. 修改工作目录、重定向标准输入输出
chdir("/");
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
```

僵尸与孤儿进程详解见 → [Process Lifecycle (生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md) · [Process States & Scheduling (状态与调度)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)

---

## Interprocess Communication (进程间通信)

> [!abstract] 核心考点：匿名管道 pipe、命名管道 fifo、共享内存 mmap、信号量与消息队列等 IPC 机制对比
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

IPC 机制详解见 → [Process Lifecycle (生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md) · [Process States & Scheduling (状态与调度)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 02-Processes and IPC (进程与进程间通信) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
