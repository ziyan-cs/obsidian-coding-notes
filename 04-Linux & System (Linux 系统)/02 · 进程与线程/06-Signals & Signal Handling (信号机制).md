---
tags:
  - linux
  - process
---

> **核心考点**：信号的本质、常见信号、信号处理三种方式、可重入函数、信号与多线程

## 信号的本质

信号是 Linux 的**异步通知机制**，内核或进程可以向另一个进程发送信号，目标进程在下次从内核态返回用户态时处理信号。

---

## 常见信号

| 分类       | 信号        | 编号    | 默认行为      | 常见来源            |
| -------- | --------- | ----- | --------- | --------------- |
| 优雅终止     | SIGINT    | 2     | 终止        | Ctrl+C          |
|          | SIGTERM   | 15    | 终止        | kill 命令默认       |
|          | SIGQUIT   | 3     | Core dump | Ctrl+\          |
| 强制控制     | SIGKILL   | 9     | 强制终止      | 不可捕获            |
|          | SIGSTOP   | 19    | 暂停        | 不可捕获            |
|          | SIGCONT   | 18    | 继续        | 恢复暂停进程          |
| 程序崩溃     | SIGSEGV   | 11    | Core dump | 非法内存访问          |
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
kill(-1, SIGTERM);            // 向所有有权限的进程发（慎用）
raise(SIGUSR1);               // 向自己发信号
pthread_kill(tid, SIGUSR1);   // 向指定线程发信号
```

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

// 方式一：忽略信号
signal(SIGPIPE, SIG_IGN);    // 常用：服务器忽略 SIGPIPE

// 方式二：恢复默认行为
signal(SIGTERM, SIG_DFL);

// 方式三：自定义处理函数
void handler(int sig) {
    // 注意：只能调用异步信号安全函数！
    write(STDOUT_FILENO, "caught!\n", 8);   // write 是信号安全的
    // printf 不是信号安全的！
}
signal(SIGTERM, handler);
```

---

## sigaction（更安全的注册方式）

```c
struct sigaction sa;
sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);           // 清空信号屏蔽集
sigaddset(&sa.sa_mask, SIGINT);     // 处理期间屏蔽 SIGINT
sa.sa_flags = SA_RESTART;           // 被信号中断的系统调用自动重启

sigaction(SIGTERM, &sa, NULL);
```

`signal()` 的行为在不同系统上不一致，**生产代码应使用 `sigaction()`**。

---

## 可重入函数（Async-Signal-Safe）

信号处理函数可能在任意时刻打断主程序，若主程序正在执行 `malloc` 而信号处理函数也调用 `malloc`，会导致堆数据结构损坏。

**信号处理函数中只能调用异步信号安全函数：**

```
安全：write, read, _exit, kill, signal, sem_post, 
      abort, clock_gettime, strlen...

不安全：printf, malloc, free, exit, fopen, 
        任何使用全局锁的函数...
```

**常用模式：用 volatile 标志位传递信号到主循环：**

```c
volatile sig_atomic_t got_signal = 0;

void handler(int sig) {
    got_signal = 1;    // 只做最简单的操作
}

int main() {
    signal(SIGTERM, handler);
    while (!got_signal) {
        // 正常工作...
    }
    // 在主循环中处理清理工作
    cleanup();
    return 0;
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
// 临界区操作（不会被 SIGINT/SIGTERM 打断）
pthread_sigmask(SIG_UNBLOCK, &set, NULL);
```

---

## 信号与多线程

- 信号是发给**进程**的，内核选择一个没有屏蔽该信号的线程处理
- 最佳实践：**主线程屏蔽所有信号，专门用一个线程用 `sigwait()` 同步处理信号**：

```c
void *signal_thread(void *arg) {
    sigset_t *set = arg;
    int sig;
    while (1) {
        sigwait(set, &sig);    // 同步等待信号（不是异步处理函数，可以调用 printf 等）
        if (sig == SIGTERM) {
            printf("Received SIGTERM, shutting down...\n");
            // 优雅关闭...
        }
    }
    return NULL;
}

int main() {
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);   // 主线程屏蔽所有信号

    pthread_t tid;
    pthread_create(&tid, NULL, signal_thread, &set);
    // ... 其他工作线程
}
```

---

## 关联笔记

- [Shell & Basic Commands (命令行与Shell编程)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md)
- [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md)
- [System Administration Basics (系统管理基础)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/03-System%20Administration%20Basics%20(系统管理基础).md)
- [Process Lifecycle： fork, exec, wait (生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)
- [Process States & Scheduling (状态与调度)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)
