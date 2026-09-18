---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习目标：用互斥锁、条件变量和信号量建立同步关系，并识别死锁条件。

# Mutex and Condition Variable (互斥锁与条件变量)

> [!note] 本节重点：pthread_mutex 互斥锁、pthread_cond 条件变量、生产者-消费者模型

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

互斥锁与条件变量详解见 → POSIX Thread (线程生命周期) · Deadlock (死锁原理与预防)

> [!summary] 核心摘要
>
> mutex 保护共享不变量；condition variable 不保存条件本身，只负责等待/通知，所以必须配合受同一 mutex 保护的 predicate，并在 `while` 中等待。通知不是“事件不会丢”的保证，正确性来自“修改 predicate 与检查 predicate 都在锁下”。
>
> ---

# Deadlock Causes and Prevention (死锁原因与预防)

> [!note] 本节重点：死锁四个必要条件（Coffman 条件）、锁顺序约定预防、死锁检测与恢复

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

死锁原理与预防见 → Mutex & Condition Variable (互斥锁与条件变量) · POSIX Thread (线程生命周期)

---

# Threads and Processes (线程与进程)

> [!note] 本节重点：线程 vs 进程的创建/切换/通信开销、Linux clone 系统调用、共享资源差异

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

线程进程对比见 → POSIX Thread (线程生命周期) · Mutex & Condition Variable (互斥锁与条件变量)

---

# Semaphores (信号量)

> [!note] 本节重点：POSIX 与 System V 信号量 API、二值/计数信号量、PV 操作与生产者-消费者同步

## 信号量的本质

信号量是一个非负整数计数器，支持两个原子操作：

- **P 操作（wait/down）**：仅当计数大于 0 时原子地减 1；否则等待
- **V 操作（signal/up/post）**：计数器加 1，唤醒等待者

**二值信号量（0/1）** 可用于互斥，但没有 mutex 的所有者语义与优先级等特性；**计数信号量** 更自然地表达可用资源数量。

---

# POSIX 信号量（推荐）

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

## 用信号量实现生产者-消费者

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

信号量详解见 → Mutex & Condition Variable (互斥锁与条件变量) · Deadlock (死锁原理与预防)

---
