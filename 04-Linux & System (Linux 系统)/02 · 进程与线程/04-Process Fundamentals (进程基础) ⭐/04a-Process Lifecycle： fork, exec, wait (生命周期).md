---
tags:
  - linux/process
status: 🌱
---

> [!important] **核心考点**：fork 写时拷贝、exec 系列替换进程映像、wait/waitpid 回收子进程、进程生命周期

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
| 打开的文件描述符（共享偏移） | 各自 PCB |
| 信号处理设置 | 各自挂起的信号 |
| 当前工作目录 | 各自文件描述符偏移量（写时拷贝后独立） |

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

## Linux 调度（CFS）

Linux 默认使用 **CFS（Completely Fair Scheduler，完全公平调度器）**：

- 以**虚拟运行时间（vruntime）** 为核心指标，vruntime 最小的进程优先运行
- 用**红黑树**组织所有就绪进程，最左节点（vruntime 最小）即下一个运行的进程
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

进程生命周期详解见 → [Process States & Scheduling (状态与调度)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md) · [Zombie & Orphan Process (僵尸进程与孤儿进程)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04c-Zombie%20&%20Orphan%20Process%20(僵尸进程与孤儿进程).md)
