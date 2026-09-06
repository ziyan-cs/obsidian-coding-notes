---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Threads Synchronization and Signals (线程同步与信号)

> [!abstract] 阅读定位
>
> 本专题整合同类机制、边界与实践内容，作为一次完整学习单元。

## POSIX Thread Lifecycle (POSIX 线程生命周期)

> [!abstract] 核心考点：POSIX 线程 pthread_create/join/detach API、线程属性设置、线程生命周期管理

## 线程的本质

线程是进程内的**执行流**，共享进程的地址空间、文件描述符、信号处理，但拥有独立的：

- 栈（Stack）
- 寄存器（包括 PC、SP）
- 线程本地存储（TLS）
- errno

---

## 创建与等待

```c
#include <pthread.h>
// 编译：gcc -o prog prog.c -lpthread

void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d running\n", id);
    return (void *)(long)id;    // 返回值
}

int main() {
    pthread_t tid;
    int arg = 42;

    // 创建线程
    pthread_create(&tid, NULL, worker, &arg);

    // 等待线程结束，获取返回值
    void *retval;
    pthread_join(tid, &retval);
    printf("Thread returned: %ld\n", (long)retval);
    return 0;
}
```

---

## 线程属性

```c
pthread_attr_t attr;
pthread_attr_init(&attr);

// 设置栈大小（默认通常 8MB）
pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);   // 2MB

// 分离状态（detached：线程结束自动回收，不需要 join）
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

pthread_create(&tid, &attr, worker, NULL);
pthread_attr_destroy(&attr);
```

---

## 分离（detach）与合并（join）

```c
// join：主线程等待子线程，获取返回值
pthread_join(tid, &retval);

// detach：线程结束后自动释放资源，无法 join
pthread_detach(tid);
// 或在线程内部自我分离：
pthread_detach(pthread_self());
```

> 若线程既没有 join 也没有 detach，线程结束后资源泄漏（类似僵尸进程）。

---

## 线程本地存储（TLS）

每个线程有独立的副本，互不干扰：

```c
// C++11
thread_local int counter = 0;    // 每个线程有自己的 counter

// POSIX
pthread_key_t key;
pthread_key_create(&key, free);         // 创建 key，free 是析构函数
pthread_setspecific(key, malloc(100));  // 设置当前线程的值
void *val = pthread_getspecific(key);   // 获取当前线程的值
```

POSIX 线程详解见 → [Mutex & Condition Variable (互斥锁与条件变量)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md) · [Deadlock (死锁原理与预防)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05c-Deadlock：%20Causes%20&%20Prevention%20(死锁原理与预防).md)

---

## Mutex and Condition Variable (互斥锁与条件变量)

> [!abstract] 核心考点：pthread_mutex 互斥锁、pthread_cond 条件变量、生产者-消费者模型

## 互斥锁（Mutex）

保护临界区，防止多线程同时访问共享资源：

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 或动态初始化
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

// 使用
pthread_mutex_lock(&mutex);
// --- 临界区 ---
shared_counter++;
// --- 临界区结束 ---
pthread_mutex_unlock(&mutex);

pthread_mutex_destroy(&mutex);
```

**trylock（非阻塞）：**

```c
if (pthread_mutex_trylock(&mutex) == 0) {
    // 加锁成功
    pthread_mutex_unlock(&mutex);
} else {
    // 锁被占用，做其他事
}
```

**C++11 RAII 方式（推荐）：**

```cpp
#include <mutex>
std::mutex mtx;

void func() {
    std::lock_guard<std::mutex> lock(mtx);  // 构造时加锁，析构时自动解锁
    shared_counter++;
}   // 离开作用域自动解锁，异常安全
```

---

## 条件变量（Condition Variable）

用于线程间的**等待/通知**机制，解决"等待某个条件成立"的问题：

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
int ready = 0;

// 消费者线程：等待条件
void *consumer(void *arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {                          // 必须用 while，防止虚假唤醒
        pthread_cond_wait(&cond, &mutex);    // 原子地：释放锁 + 进入等待
    }                                        // 被唤醒后重新持有锁
    // 处理数据...
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 生产者线程：发出通知
void *producer(void *arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_signal(&cond);              // 唤醒一个等待者
    // pthread_cond_broadcast(&cond);        // 唤醒所有等待者
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

### 为什么 while 而不是 if？

**虚假唤醒与竞争唤醒**：线程可能在没有收到 signal 的情况下醒来；即使收到通知，其他线程也可能先拿锁并改变条件。用 `while` 在持锁状态重新检查 predicate，确保条件真的满足。

### pthread_cond_wait 的原子性

`pthread_cond_wait` 内部做了三件事，且保证原子：

1. 释放 mutex
2. 进入睡眠等待
3. 被唤醒后重新持有 mutex

---

## 读写锁（rwlock）

允许多个读者并发，写者独占：

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// 读（可并发）
pthread_rwlock_rdlock(&rwlock);
// 读操作...
pthread_rwlock_unlock(&rwlock);

// 写（独占）
pthread_rwlock_wrlock(&rwlock);
// 写操作...
pthread_rwlock_unlock(&rwlock);
```

适合：**读多写少**的场景（如配置、缓存）。写多时写者频繁等待，反而不如 mutex。

互斥锁与条件变量详解见 → [POSIX Thread (线程生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md) · [Deadlock (死锁原理与预防)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05c-Deadlock：%20Causes%20&%20Prevention%20(死锁原理与预防).md)

## 30 秒回答

mutex 保护共享不变量；condition variable 不保存条件本身，只负责等待/通知，所以必须配合受同一 mutex 保护的 predicate，并在 `while` 中等待。通知不是“事件不会丢”的保证，正确性来自“修改 predicate 与检查 predicate 都在锁下”。

---

## Deadlock Causes and Prevention (死锁原因与预防)

> [!abstract] 核心考点：死锁四个必要条件（Coffman 条件）、锁顺序约定预防、死锁检测与恢复

## 死锁的四个必要条件（Coffman 条件）

同时满足以下四个条件才会发生死锁：

|条件|含义|
|---|---|
|互斥（Mutual Exclusion）|资源一次只能被一个线程持有|
|占有并等待（Hold and Wait）|线程持有资源的同时等待其他资源|
|不可剥夺（No Preemption）|资源只能由持有者主动释放|
|循环等待（Circular Wait）|线程之间形成资源等待的环|

**破坏任意一个条件即可预防死锁。**

---

## 经典死锁场景

```c
// 线程 A                    // 线程 B
lock(mutex1);                lock(mutex2);
lock(mutex2);  ←─等待─→     lock(mutex1);
// 循环等待，死锁！
```

---

## 预防策略

### 策略一：固定加锁顺序（破坏循环等待）

所有线程按相同顺序申请锁：

```c
// 统一规定：先锁 mutex1，再锁 mutex2
// 线程 A 和 B 都遵守这个顺序 → 不会形成环
```

### 策略二：一次性申请所有锁（破坏占有并等待）

```cpp
// C++17 std::scoped_lock（自动避免死锁顺序问题）
std::mutex m1, m2;
std::scoped_lock lock(m1, m2);   // 使用避免死锁的多锁获取算法
```

`std::scoped_lock`（多锁构造）使用避免死锁的获取算法；它不表示“硬件原子地同时拿到所有锁”，而是保证调用按标准语义避免相互等待。

### 策略三：使用 trylock + 超时（破坏不可剥夺）

```c
while (true) {
    pthread_mutex_lock(&mutex1);
    if (pthread_mutex_trylock(&mutex2) == 0) {
        break;       // 成功拿到两个锁
    }
    pthread_mutex_unlock(&mutex1);   // 拿不到就放弃，重试
    usleep(rand() % 1000);           // 随机退避，避免活锁
}
```

### 策略四：锁层次（Lock Hierarchy）

给每个锁分配层级编号，只允许从高层级向低层级申请锁，不允许反向申请。

---

## 检测死锁

```bash
thread apply all bt

valgrind --tool=helgrind ./myapp
```

死锁原理与预防见 → [Mutex & Condition Variable (互斥锁与条件变量)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md) · [POSIX Thread (线程生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md)

---

## Threads and Processes (线程与进程)

> [!abstract] 核心考点：线程 vs 进程的创建/切换/通信开销、Linux clone 系统调用、共享资源差异

## 创建开销

|操作|典型耗时|原因|
|---|---|---|
|fork()|~100μs|需要复制页表、文件描述符表等（COW 优化后好很多）|
|pthread_create()|~10μs|只分配栈和 TCB，共享进程地址空间|
|协程切换|~100ns|用户态切换，只保存少量寄存器|

## 核心对比

|维度|进程|线程|
|---|---|---|
|地址空间|独立|共享（同一进程内）|
|通信|IPC（复杂）|共享内存（简单，但需同步）|
|隔离性|强（崩溃不影响其他进程）|弱（一个线程崩溃可能拖垮整个进程）|
|创建开销|大|小|
|上下文切换|大（需切换页表、刷 TLB）|小（同一地址空间，只换栈和寄存器）|
|适用场景|需要强隔离（浏览器多进程）|需要高效通信（Web 服务器工作线程）|

## 上下文切换的代价

线程切换：

1. 保存当前线程寄存器（通用寄存器、PC、SP）到 TCB
2. 恢复目标线程寄存器
3. 切换栈指针

进程切换额外还需： 4. 切换 CR3（页表基址寄存器）→ TLB 全部失效（代价最大） 5. 切换文件描述符表等内核资源

> **TLB 失效**是进程切换比线程切换慢的核心原因。现代 CPU 用 ASID（地址空间标识符）标记 TLB 条目，可部分缓解这个问题。

线程进程对比见 → [POSIX Thread (线程生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md) · [Mutex & Condition Variable (互斥锁与条件变量)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md)

---

## Semaphores (信号量)

> [!abstract] 核心考点：POSIX 与 System V 信号量 API、二值/计数信号量、PV 操作与生产者-消费者同步

## 信号量的本质

信号量是一个非负整数计数器，支持两个原子操作：

- **P 操作（wait/down）**：仅当计数大于 0 时原子地减 1；否则等待
- **V 操作（signal/up/post）**：计数器加 1，唤醒等待者

**二值信号量（0/1）** 可用于互斥，但没有 mutex 的所有者语义与优先级等特性；**计数信号量** 更自然地表达可用资源数量。

---

## POSIX 信号量（推荐）

```c
#include <semaphore.h>

// 无名信号量（线程间）
sem_t sem;
sem_init(&sem, 0, 1);      // 参数：信号量, pshared(0=线程间), 初始值

sem_wait(&sem);            // P 操作（阻塞直到 > 0，然后减 1）
// --- 临界区 ---
sem_post(&sem);            // V 操作（加 1，唤醒等待者）

sem_destroy(&sem);

// 有名信号量（进程间）
sem_t *sem = sem_open("/mysem", O_CREAT, 0666, 1);
sem_wait(sem);
sem_post(sem);
sem_close(sem);
sem_unlink("/mysem");       // 删除
```

### 用信号量实现生产者-消费者

```c
sem_t empty;   // 空槽数量（初始 = 缓冲区大小）
sem_t full;    // 已填槽数量（初始 = 0）
pthread_mutex_t mutex;

// 生产者
sem_wait(&empty);           // 等待有空槽
pthread_mutex_lock(&mutex);
// 放入数据...
pthread_mutex_unlock(&mutex);
sem_post(&full);            // 通知消费者

// 消费者
sem_wait(&full);            // 等待有数据
pthread_mutex_lock(&mutex);
// 取出数据...
pthread_mutex_unlock(&mutex);
sem_post(&empty);           // 通知生产者
```

---

## 信号量 vs 互斥锁 vs 条件变量

| |互斥锁|条件变量|信号量|
|---|---|---|---|
|核心用途|互斥访问临界区|等待某个条件|控制资源数量|
|是否有值|无（只有锁定/解锁）|无|有（非负整数）|
|跨进程|需特殊配置|需特殊配置|POSIX 有名信号量支持|
|锁的所有者|只有加锁者能解锁|—|任何线程可 post|

信号量详解见 → [Mutex & Condition Variable (互斥锁与条件变量)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md) · [Deadlock (死锁原理与预防)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05c-Deadlock：%20Causes%20&%20Prevention%20(死锁原理与预防).md)

---

## Signals and Signal Handling (信号与处理)

> [!abstract] 核心考点：信号的本质、常见信号、信号处理三种方式、可重入函数、信号与多线程

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

信号机制详解见 → [Process Lifecycle (生命周期)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04a-Process%20Lifecycle：%20fork,%20exec,%20wait%20(生命周期).md) · [Process States & Scheduling (状态与调度)](/03-Backend%20Systems%20(后端系统)/01-Linux%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/04-Process%20Fundamentals%20(进程基础)%20⭐/04b-Process%20States%20&%20Scheduling%20(状态与调度).md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 03-Threads Synchronization and Signals (线程同步与信号) 的问题、核心机制与边界。

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
