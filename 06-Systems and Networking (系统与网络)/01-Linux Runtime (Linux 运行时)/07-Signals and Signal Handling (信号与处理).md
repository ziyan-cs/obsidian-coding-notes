---
study_stage: backlog
---

> [!abstract] 学习目标：理解信号的递送、屏蔽、处理函数限制以及 sigaction 的安全用法。

> [!note] 本节重点：信号的本质、常见信号、信号处理三种方式、可重入函数、信号与多线程

# 信号模型与投递
## 信号的本质

信号是进程/线程收到的**异步通知机制**。进程定向信号可能由任一未屏蔽它的线程接收；由非法内存访问等执行错误产生的信号通常定向到出错线程。信号可处于待决、被屏蔽或已递送状态；被屏蔽不等于丢弃。标准信号多次出现通常不会逐次排队，不能拿它当可靠任务队列。

---

## 常见信号

| 分类       | 信号        | Linux x86-64 编号 | 默认行为      | 常见来源            |
| -------- | --------- | ----- | --------- | --------------- |
| 优雅终止     | SIGINT    | 2     | 终止        | Ctrl+C          |
|          | SIGTERM   | 15    | 终止        | kill 命令默认       |
|          | SIGQUIT   | 3     | Core dump | Ctrl+\          |
| 强制控制     | SIGKILL   | 9     | 强制终止      | 不可捕获            |
|          | SIGSTOP   | 19    | 暂停        | 不可捕获            |
|          | SIGCONT   | 18    | 继续        | 恢复暂停进程          |
| 程序崩溃     | SIGSEGV   | 11    | Core dump | 非法内存访问；定向到出错线程 |
|          | SIGABRT   | 6     | Core dump | `abort()` 调用    |
|          | SIGFPE    | 8     | Core dump | 除零 / 浮点错误       |
| 进程管理     | SIGCHLD   | 17    | 忽略        | 子进程状态改变         |
| 网络 / IO  | SIGPIPE   | 13    | 终止        | 向已关闭管道写入        |
| 定时 / 自定义 | SIGALRM   | 14    | 终止        | `alarm()` 定时器到期 |
|          | SIGUSR1/2 | 10/12 | 终止        | 用户自定义信号         |

---

## 发送信号

```c
kill(pid, SIGTERM);           // 向进程发信号
kill(0, SIGTERM);             // 向当前进程组发
raise(SIGUSR1);               // 向自己发信号
pthread_kill(tid, SIGUSR1);   // 向指定线程发信号
```

`kill(0, ...)` 面向调用者所在**进程组**，不是“本进程”；`kill(-1, ...)` 范围更广，容易误伤，不作为练习命令。示例信号编号只适用于表中平台，程序应使用 `SIGTERM` 等符号名。

```bash
kill -15 1234      # 发 SIGTERM
kill -9 1234       # 发 SIGKILL（强制，不可拦截）
kill -l            # 列出所有信号
killall nginx      # 按名称发信号
```

---

## 信号处理三种方式

```c
#include <signal.h>
#include <unistd.h>

// 方式一：忽略信号（影响整个进程；服务器也可对单次发送使用平台支持的 MSG_NOSIGNAL）
signal(SIGPIPE, SIG_IGN);

// 方式二：恢复默认行为
signal(SIGTERM, SIG_DFL);

// 方式三：自定义处理函数
void handler(int sig) {
    (void)sig;
    // 注意：只能调用异步信号安全函数！
    write(STDOUT_FILENO, "caught!\n", 8);   // write 是信号安全的
    // printf 不是信号安全的！
}
signal(SIGTERM, handler);     // 仅演示三种处置；实际注册推荐 sigaction
```

---

# sigaction（更安全的注册方式）

```c
struct sigaction sa = {0};
sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);           // 清空信号屏蔽集
sigaddset(&sa.sa_mask, SIGINT);     // 处理期间屏蔽 SIGINT
sa.sa_flags = SA_RESTART;           // 仅部分被中断的阻塞调用会自动重启

sigaction(SIGTERM, &sa, NULL);
```

`signal()` 的处理器注册语义在系统之间不够一致，**生产代码优先使用 `sigaction()`**。`SA_RESTART` 不是“所有系统调用都不会返回 EINTR”：调用类型、套接字超时与平台行为均需核对，仍须检查返回值。

---

## 可重入函数（Async-Signal-Safe）

信号处理函数可能在任意时刻打断主程序，若主程序正在执行 `malloc` 而信号处理函数也调用 `malloc`，会导致堆数据结构损坏。

**异步处理器只做极少量工作，所调用函数须在平台的 async-signal-safe 列表中：**

```
可用的常见例子：write、_exit、kill（仍须检查具体平台与 POSIX 版本）
不要在处理器中调用：printf、malloc、free、exit、fopen、C++ iostream
```

`volatile sig_atomic_t` 可用于处理器与正常控制流之间的简单标志，但不能代替线程同步。下面只展示通信机制，实际事件循环不能忙等，也不能天真地用 `while (!flag) pause()`：检查标志与睡眠之间有丢失唤醒的竞态。应选择 `sigsuspend`、self-pipe、`signalfd`（Linux）或下面的 `sigwait` 方案。

```c
volatile sig_atomic_t got_signal = 0;

void handler(int sig) {
    (void)sig;
    got_signal = 1;    // 只做最简单的操作
}
```

---

## 信号屏蔽（Signal Mask）

每个线程有独立的信号屏蔽字，被屏蔽的信号会被挂起，直到解除屏蔽：

```c
sigset_t set;
sigemptyset(&set);
sigaddset(&set, SIGINT);
sigaddset(&set, SIGTERM);

// 屏蔽信号（SIG_BLOCK：添加，SIG_UNBLOCK：移除，SIG_SETMASK：替换）
pthread_sigmask(SIG_BLOCK, &set, NULL);
// 这只影响当前线程；进程定向信号仍可能递送到其他未屏蔽线程
pthread_sigmask(SIG_UNBLOCK, &set, NULL);
```

---

## 信号与多线程

- 进程定向与线程定向信号必须区分；`kill(pid, ...)` 通常生成进程定向信号，`pthread_kill` 生成线程定向信号。
- 对 `SIGTERM`/`SIGINT` 这类正常关闭信号，一个简洁方案是：**创建工作线程前先屏蔽指定信号，再由主线程或专用线程 `sigwait()` 同步接收**。不要对 `SIGSEGV` 等同步错误信号照搬此设计；`SIGKILL` 和 `SIGSTOP` 不可捕获、不可屏蔽。

```c
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);
    int err = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (err != 0) { fprintf(stderr, "pthread_sigmask: %d\n", err); return EXIT_FAILURE; }

    // 此后创建的工作线程继承屏蔽字；生产程序在此启动工作线程。
    int sig = 0;
    err = sigwait(&set, &sig);  // 同步等待，不是在异步信号处理器中
    if (err != 0) { fprintf(stderr, "sigwait: %d\n", err); return EXIT_FAILURE; }
    printf("received signal %d; begin graceful shutdown\n", sig);
    // 停止接入、通知工作线程、等待在途任务，然后退出。
    return EXIT_SUCCESS;
}
```

`sigwait` 不把指定信号交给异步处理器；所有可能接收这些进程定向信号的线程都要先屏蔽它们。不要把示例中的 `SIGTERM` 接收流程误当作 `SIGKILL` 可优雅处理。查阅：[signal(7)](https://man7.org/linux/man-pages/man7/signal.7.html)、[signal-safety(7)](https://man7.org/linux/man-pages/man7/signal-safety.7.html)、[sigwait(3)](https://man7.org/linux/man-pages/man3/sigwait.3.html)。
