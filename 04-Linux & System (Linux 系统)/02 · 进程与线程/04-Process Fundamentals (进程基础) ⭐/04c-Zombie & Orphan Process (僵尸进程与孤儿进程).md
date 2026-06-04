---
tags:
  - linux
  - process
---

> **核心考点**：僵尸进程产生原因与危害、孤儿进程的 init 收养、SIGCHLD 信号处理

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

## 关联笔记

- [Process Lifecycle： fork, exec, wait (生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md)
- [Process States & Scheduling (状态与调度)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)
- [Inter-Process Communication： pipe, fifo, mmap (IPC)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/04-Process%20Fundamentals%20(进程基础)%20⭐/04d-Inter-Process%20Communication：%20pipe,%20fifo,%20mmap%20(IPC).md)
- [Shell & Basic Commands (命令行与Shell编程)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/01-Shell%20&%20Basic%20Commands%20(命令行与Shell编程).md)
- [File System & Permissions (文件系统与权限)](/04-Linux%20&%20System%20(Linux%20系统)/01%20·%20Linux基础/02-File%20System%20&%20Permissions%20(文件系统与权限).md)
