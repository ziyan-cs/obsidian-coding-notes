---
study_stage: backlog
---

> [!abstract] 学习定位：沿着一次事件或请求的完整路径学习协议、内核与服务器模型，重点是状态变化、阻塞点和释放时机。

> [!summary] 核心摘要
>
> 进程拥有独立地址空间和内核资源，IPC 用管道、共享内存、消息队列或 socket 跨越隔离边界。选型要同时考虑数据量、同步方式、复制成本和故障隔离。

# Process Lifecycle (进程生命周期)

> [!note] 本节重点：fork 写时拷贝、exec 系列替换进程映像、wait/waitpid 回收子进程、进程生命周期

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

# Linux 调度（调度器概念）

普通任务的公平调度以权重分配 CPU 时间为基本心智模型；具体挑选下一任务的实现随内核版本演进。旧版 CFS 偏重 `vruntime` 最小者，Linux 6.6 起逐步转向 EEVDF 的“符合资格的最早虚拟截止时间”选择，不能把红黑树最左节点当成所有版本的规则。

- `nice` 值（-20 到 19）改变普通任务的调度权重；值更低通常分得更多 CPU，但不保证立刻运行。
- CFS 的 `vruntime` 与红黑树是理解旧实现的模型；EEVDF 同时考虑 lag、资格和虚拟 deadline。
- 实时调度策略、CPU affinity、cgroup 配额和系统负载也影响观测结果；不要把 `nice` 等同于内核所有调度类的统一优先级。

```bash
nice -n -10 ./myapp     # 以高优先级启动
renice 5 -p 1234        # 修改运行中进程的 nice 值
```

查看 `ps -o pid,stat,ni,pri,comm -p <PID>` 时，`NI` 是 nice 值；`PRI`/`PR` 的显示和解释依工具及调度策略而异，不能用 `PR = NI + 20` 当作通用公式。调高优先级（负 nice）通常需要相应权限。

[Linux EEVDF scheduler](https://docs.kernel.org/scheduler/sched-eevdf.html) · [CFS scheduler history](https://docs.kernel.org/scheduler/sched-design-CFS.html)

---

# Zombie and Orphan Processes (僵尸进程与孤儿进程)

> [!note] 本节重点：僵尸进程产生原因与危害、孤儿进程的 init 收养、SIGCHLD 信号处理

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


---

# Interprocess Communication (进程间通信)

> [!note] 本节重点：匿名管道 pipe、命名管道 fifo、共享内存 mmap、信号量与消息队列等 IPC 机制对比
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

共享映射让多个进程访问同一组物理页，避免为了共享数据反复在进程间复制；但同步、缓存一致性可见性、页错误和安全边界仍需自行设计。它不意味着“任何 IPC 工作负载里绝对最快”或没有内存复制成本：

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


> [!info]- 延伸阅读
> - 下一步：[05-POSIX Thread Lifecycle (POSIX 线程生命周期)](/06-Systems%20and%20Networking%20(系统与网络)/01-Linux%20Runtime%20(Linux%20运行时)/05-POSIX%20Thread%20Lifecycle%20(POSIX%20线程生命周期).md)
