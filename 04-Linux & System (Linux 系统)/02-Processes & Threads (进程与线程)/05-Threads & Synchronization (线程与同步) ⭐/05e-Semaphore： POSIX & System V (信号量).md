---
tags:
  - linux/process
status: 🌱
---

> [!important] **核心考点**：POSIX 与 System V 信号量 API、二值/计数信号量、PV 操作与生产者-消费者同步

## 信号量的本质

信号量是一个非负整数计数器，支持两个原子操作：

- **P 操作（wait/down）**：计数器减 1，若为负则阻塞
- **V 操作（signal/up/post）**：计数器加 1，唤醒等待者

**二值信号量（0/1）** 等价于互斥锁，**计数信号量** 用于控制并发数量。

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

信号量详解见 → [Mutex & Condition Variable (互斥锁与条件变量)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md) · [Deadlock (死锁原理与预防)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05c-Deadlock：%20Causes%20&%20Prevention%20(死锁原理与预防).md)
