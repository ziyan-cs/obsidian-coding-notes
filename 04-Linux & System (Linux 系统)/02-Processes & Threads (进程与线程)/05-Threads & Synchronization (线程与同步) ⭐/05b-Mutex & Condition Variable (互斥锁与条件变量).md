---
tags:
  - linux/process
status: 🌱
---

# Mutex & Condition Variable — 互斥锁与条件变量

> [!important] **核心考点**：pthread_mutex 互斥锁、pthread_cond 条件变量、生产者-消费者模型

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

互斥锁与条件变量详解见 → [POSIX Thread (线程生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md) · [Deadlock (死锁原理与预防)](/04-Linux%20&%20System%20(Linux%20系统)/02-Processes%20&%20Threads%20(进程与线程)/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05c-Deadlock：%20Causes%20&%20Prevention%20(死锁原理与预防).md)

## 30 秒回答

mutex 保护共享不变量；condition variable 不保存条件本身，只负责等待/通知，所以必须配合受同一 mutex 保护的 predicate，并在 `while` 中等待。通知不是“事件不会丢”的保证，正确性来自“修改 predicate 与检查 predicate 都在锁下”。
