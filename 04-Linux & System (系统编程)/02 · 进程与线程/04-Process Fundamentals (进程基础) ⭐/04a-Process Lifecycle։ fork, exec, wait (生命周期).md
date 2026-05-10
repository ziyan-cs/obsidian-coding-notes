

# 0. 核心（进程模型）

- 本质

>项目.cpp → 程序.exe → 进程

- 生命周期

>创建 → 运行 → 阻塞 → 终止 → 回收

# 1. 进程创建
### fork（🔥）

- `fork()`：创建子进程
  - 复制父进程的地址空间（写时复制）
  - 返回值：父进程返回子进程PID，子进程返回0，失败返回-1
- 父子进程的区别：PID、PPID、PCB资源、文件描述符偏移量

### exec

### wait / waitpid

# 2. 进程查看与控制（🔥）

### ps

- `ps`：静态查看进程
	- `[PID]`：（指定进程）
	- `-aux`：BSD 风格，列名更友好
	- `-ef`：System V 风格，`PPID` 列更清晰
	- `-efL`：查看线程（LWP 轻量级进程ID）
- 参数
	- `USER`-`PID`-`PPID`-`%CPU`/`%MEM`-`VSZ`/`RSS`
	- `STAT`：进程状态
	- `TTY`：完整终端名称
	- `COMMAND (CMD)`：启动程序所用命令和参数
- 常用
	- `ps -aux | grep`
	- `ps -efL`

- `pstree`：静态查看进程树
	- `-p [PID]`：（指定进程）
	- `-p`：（显示 `PID`）
	- `-u`：（显示进程数）

### top / htop

- `top`：动态监控进程
	- `-p [PID]`：（指定进程）
	- `-d [num]`：（指定刷新间隔，默认3秒）
	- `-i`：（不显示空闲进程）
- 参数
	- `VIRT`/`RES`/`SHR`
	- `PR`：进程优先级
		- 内核调度的优先级
		- （-20 最高，39 最低）
	- `NI`：进程谦让值
		- 用户设置的优先级偏移量
		- （-20 最高，19 最低）
- 快捷键
	- `P`：（按CPU使用率排序，默认）
	- `M`：（按内存占用率排序）
	- `N`：（按 `PID` 排序）
	- `k`：（终止指定进程）
	- `q`：（退出）

- `htop`：增强版特点与安装

### netstat / ss

- `netstat`
	- `-t`：（仅 TCP 协议）
	- `-u`：（仅 UDP 协议）
	- `-l`：（仅监听状态）
	- `-n`：（显示IP）
	- `-p`：（显示 PID / 进程名）
	- `-a`：（所有）

- `ss -s`：全局连接状态的参数
- `ss -tulnp`：正在监听的 TCP/UDP 端口
- `ss -antp`：所有状态的 TCP 端口
	- `state [listening|established|time-wait]`
		- （指定状态）
	- `src [端口号]`：（发起的连接）
	- `dst [端口号]`：（连接到的连接）
	- `sport = [端口号]`：（指定端口）
	- `| grep :[端口号]`（指定端口）
	- `pid [PID]`：（指定进程）

### kill

- `kill [PID]`：发送 `SIGTERM(15)` 信号
	- `-9`：发送 `SIGKILL(9)` 信号
- `pidof [process_name]` 获取进程名
- `killall [process_name]`：批量终止
- `pkill [process_name]`：模糊匹配，终止进程

# 3. 进程状态（🔥）

- **STAT**
	- `R` 运行中/可运行
	- `S` 可中断睡眠（等待事件）
	- `D` 不可中断睡眠（通常是IO等待，杀不掉）
	- `Z` 僵尸态（父进程没回收）
	- `T` 停止/被跟踪

- `kill -l`：列出系统支持的所有信号
- 常用信号与进程行为
	- `SIGINT(2)`
	- `SIGTERM(15)`
	- `SIGKILL(9)`
	- `SIGCHLD(17)`


> 核心考点：fork 的行为、COW 机制、exec 族函数、wait 的必要性

## 进程的本质

进程是**程序的一次运行实例**，是操作系统资源分配的基本单位。每个进程有独立的：

- 虚拟地址空间（代码段、数据段、堆、栈）
- 文件描述符表
- 信号处理表
- PID、PPID（父进程 ID）

---

## fork()

```c
#include <unistd.h>
pid_t pid = fork();
```

`fork()` 创建一个与父进程几乎完全相同的子进程：

```
父进程
  │
  ├─ fork() ────────────────┐
  │                         │
  ↓ 父进程继续               ↓ 子进程（从 fork() 返回处开始执行）
  pid > 0（子进程 PID）      pid == 0
```

```c
pid_t pid = fork();
if (pid < 0) {
    perror("fork failed");
} else if (pid == 0) {
    // 子进程
    printf("Child PID: %d\n", getpid());
    exit(0);
} else {
    // 父进程
    printf("Parent, child PID: %d\n", pid);
}
```

### Copy-On-Write（COW，写时复制）

fork 之后，父子进程**共享同一份物理内存页**，并不立即复制。只有当某方尝试**写入**时，才触发缺页中断，内核将该页复制一份给写入方。

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

---

## exec 族函数

`exec` 用一个新程序**替换当前进程的地址空间**，但保留 PID：

```c
#include <unistd.h>

execl("/bin/ls", "ls", "-l", NULL);     // 列出参数
execlp("ls", "ls", "-l", NULL);         // 在 PATH 中查找
execv("/bin/ls", argv);                  // 参数用数组
execvp("ls", argv);                      // PATH + 数组
execve("/bin/ls", argv, envp);           // 完整版（可指定环境变量）
```

- exec 成功后，原来的代码段、数据段、堆栈全部被替换
- exec 失败才会返回（返回 -1）
- 打开的文件描述符默认继承（除非设置 `FD_CLOEXEC`）

**fork + exec = 创建新进程执行新程序（Shell 的工作方式）：**

```c
pid_t pid = fork();
if (pid == 0) {
    execvp("gcc", args);    // 子进程执行 gcc
    perror("exec failed");
    exit(1);
} else {
    wait(NULL);             // 父进程等待子进程结束
}
```

---

## wait / waitpid

父进程必须调用 `wait` 来回收子进程资源，否则子进程变成**僵尸进程**：

```c
#include <sys/wait.h>

pid_t wait(int *status);                          // 等待任意子进程
pid_t waitpid(pid_t pid, int *status, int opts);  // 等待指定子进程

// 检查退出状态
int status;
pid_t child = waitpid(pid, &status, 0);

if (WIFEXITED(status))                    // 正常退出？
    printf("exit code: %d\n", WEXITSTATUS(status));
if (WIFSIGNALED(status))                  // 被信号杀死？
    printf("signal: %d\n", WTERMSIG(status));
```

**非阻塞等待（轮询）：**

```c
waitpid(pid, &status, WNOHANG);   // 不阻塞，若子进程未结束返回 0
```