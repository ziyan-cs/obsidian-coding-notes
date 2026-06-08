---
tags:
  - cs
  - os
---

> **核心考点**：进程与线程的区别、PCB/TCB、进程状态转换、进程创建（fork）、协程

## 进程 vs 线程

| 维度 | 进程（Process） | 线程（Thread） |
|------|----------------|----------------|
| 资源拥有 | 独立地址空间、文件描述符 | 共享进程资源 |
| 切换代价 | 高（切换页表/TLB/地址空间） | 低（仅保存寄存器） |
| 通信方式 | IPC（pipe, shm, socket） | 直接读写共享内存 |
| 健壮性 | 进程间隔离，一个崩溃不影响其他 | 一个线程崩溃可能影响整个进程 |
| 创建开销 | 高（fork 需要复制页表） | 低（pthread_create 轻量） |

### 进程控制块（PCB）

内核为每个进程维护的 PCB（Linux 中为 `task_struct`）：

```c
// Linux task_struct 关键字段（简化）
struct task_struct {
    pid_t pid;                      // 进程 ID
    long state;                     // 进程状态（TASK_RUNNING, TASK_INTERRUPTIBLE...）
    struct mm_struct *mm;           // 地址空间（页表）
    struct files_struct *files;     // 打开的文件描述符表
    struct thread_info *thread;     // 线程信息（寄存器上下文）
    struct list_head children;      // 子进程链表
    unsigned int policy;            // 调度策略
    int prio;                       // 动态优先级
    // ...
};
```

---

## 进程状态

```
     ┌───┐  ┌──────────┐  调度选择  ┌──────────┐
     │创建│→│ 就绪(Ready) │←──────────┤ 运行(Running) │
     └───┘  └──────────┘  时间片用完 └────┬─────┘
                   ↑                      │ 等待 I/O 或事件
                   │                 ┌────▼──────┐
                   └─────────────────┤ 阻塞(Blocked) │
                      事件完成       └───────────┘
```

**Linux 特有状态：**
- `TASK_INTERRUPTIBLE`：可中断睡眠（收到信号可唤醒）
- `TASK_UNINTERRUPTIBLE`：不可中断睡眠（如等待 I/O 完成）
- `TASK_STOPPED`：收到 SIGSTOP 信号
- `TASK_DEAD`：已退出（`exit()`），等待父进程 `wait()`

---

## 进程创建

```cpp
#include <unistd.h>
#include <iostream>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程
        std::cout << "Child: PID=" << getpid()
                  << ", Parent=" << getppid() << std::endl;
        execlp("/bin/ls", "ls", "-l", nullptr);  // 替换进程映像
    } else if (pid > 0) {
        // 父进程
        std::cout << "Parent: child PID=" << pid << std::endl;
        wait(nullptr);  // 等待子进程结束
    } else {
        perror("fork failed");
    }
    return 0;
}
```

**fork 的写时复制（COW）：** fork 时子进程共享父进程的页，仅置为只读。任一进程写入时触发缺页异常，内核复制该页。避免了 fork 时复制整个地址空间的开销。

---

## 线程与协程

```cpp
// POSIX 线程创建
#include <pthread.h>
#include <iostream>

void* thread_func(void* arg) {
    int* id = (int*)arg;
    std::cout << "Thread " << *id << " running" << std::endl;
    return nullptr;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    pthread_create(&t1, nullptr, thread_func, &id1);
    pthread_create(&t2, nullptr, thread_func, &id2);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    return 0;
}
```

**协程（Coroutine）：** 用户态轻量级线程，由程序员显式 yield/schedule，无需内核参与。C++20 引入 `co_await` / `co_yield` / `co_return`。

| 线程 | 协程 |
|------|------|
| 内核调度，抢占式 | 用户调度，协作式 |
| 栈大小 ≈ 8MB（固定） | 栈大小 ≈ KB 级（可动态） |
| 上下文切换 ≈ 1-3μs | 上下文切换 ≈ 0.1-0.3μs |
| 适合 CPU 密集型 | 适合 I/O 密集型 |

---

## 经典题型速查

| 题型 | 要点 |
|------|------|
| fork 返回值 | 父进程返回子 PID，子进程返回 0，错误返回 -1 |
| 孤儿进程 vs 僵尸进程 | 孤儿被 init 收养；僵尸已退出且未 wait，占用 PCB |
| 多线程共享什么 | 堆、全局变量、文件描述符；不共享栈、寄存器 |
| 线程安全 | 用互斥锁 / 原子操作 / TLS（线程本地存储）保证 |
| fork 后子进程获得的资源 | 文件描述符表复制（共享偏移量）、信号处理函数继承 |

> **工程要点**：进程用于隔离，线程用于并行。现代高性能服务器常用**多进程 + 事件驱动**（Nginx）或**多线程 + 异步 I/O**（Redis）。创建线程/进程后必须 join/detach/wait，否则资源泄漏。

---


上下文切换与CPU调度详见 → [Context Switching（上下文切换）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/02-Context%20Switching（上下文切换）.md) · [CPU Scheduling（CPU调度）](/01-CS%20Core%20(计算机核心基础)/03-Operating%20System（操作系统）/03-CPU%20Scheduling（CPU调度）.md)
