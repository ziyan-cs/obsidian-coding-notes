---
tags:
  - linux/process
status: 🌱
---

> **核心考点**：死锁四个必要条件（Coffman 条件）、锁顺序约定预防、死锁检测与恢复

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
std::scoped_lock lock(m1, m2);   // 原子地获取两个锁
```

`std::scoped_lock` 内部使用 `std::lock`，采用 **try-and-back-off** 策略避免死锁（如果拿不到所有锁，释放已拿到的，稍后重试）。

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
# 用 GDB 查看所有线程状态（死锁时线程都在等待锁）
thread apply all bt

# 用 perf 或 valgrind helgrind 检测
valgrind --tool=helgrind ./myapp
```

死锁原理与预防见 → [Mutex & Condition Variable (互斥锁与条件变量)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05b-Mutex%20&%20Condition%20Variable%20(互斥锁与条件变量).md) · [POSIX Thread (线程生命周期)](/04-Linux%20&%20System%20(Linux%20系统)/02%20·%20进程与线程/05-Threads%20&%20Synchronization%20(线程与同步)%20⭐/05a-POSIX%20Thread：%20pthread_create%20&%20lifecycle%20(线程生命周期).md)
