---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

# 30 秒回答

**核心结论**：本专题将同一条学习链上的基础概念整合为一篇：先建立整体模型，再阅读机制、边界和例子。

# Mutex (互斥锁)

> [!note] 本节重点：核心考点：> 临界区与竞态条件、锁的实现（Peterson/硬件原子/自旋锁）、互斥锁 vs 自旋锁

# 临界区与竞态条件

```cpp
// 竞态条件示例：两个线程同时增加计数器
int counter = 0;

void thread_func() {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // 非原子操作！
        // 实际是三条指令：
        // LOAD counter → R1
        // ADD  R1, #1
        // STORE R1 → counter
    }
}

// 预期结果：2000000
// 实际结果：≈ 1074521（竞态条件导致更新丢失）
```

**临界区（Critical Section）：** 访问共享资源的代码段，同一时间只允许一个线程进入。

## 临界区三条件

1. **互斥（Mutual Exclusion）**：同时最多一个线程在临界区
2. **前进（Progress）**：无线程在临界区时，想进的线程应能进
3. **有限等待（Bounded Waiting）**：线程不能无限等待

---

# Peterson 算法（软件解）

```cpp
// Peterson 算法（两个线程的互斥，无硬件原子指令）
class PetersonMutex {
    bool flag[2] = {false, false};
    int turn = 0;

public:
    void lock(int id) {
        int other = 1 - id;
        flag[id] = true;           // 表示想进入
        turn = other;              // 让对方优先
        while (flag[other] && turn == other) {
            // 忙等待
        }
    }

    void unlock(int id) {
        flag[id] = false;
    }
};
```

**正确性验证：** 满足互斥、前进、有限等待。但现代 CPU 的乱序执行可能破坏 Peterson 算法（需要 memory barrier）。

---

# 硬件原子操作

## 硬件锁（TSL / XCHG）

```asm
; x86 LOCK 前缀 + XCHG 指令实现互斥
; lock = 0 表示空闲，1 表示占用

acquire_lock:
    mov    eax, 1           ; 设置 1
    xchg   eax, [lock]      ; 原子交换：eax ↔ lock
    test   eax, eax         ; 检查旧值
    jnz    acquire_lock     ; 非 0 说明锁被占用，重试
    ret                     ; 获得锁

release_lock:
    mov    [lock], 0        ; 释放
    ret
```

## C++ 原子操作

```cpp
#include <atomic>
#include <thread>
#include <iostream>

class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // 忙等待
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

int counter = 0;
SpinLock lock;

void safe_increment() {
    for (int i = 0; i < 1000000; i++) {
        std::lock_guard<SpinLock> guard(lock);
        counter++;
    }
}
```

---

# 互斥锁 vs 自旋锁

| 特性 | 互斥锁（Mutex） | 自旋锁（Spinlock） |
|------|----------------|-------------------|
| 等待时 | 线程睡眠（上下文切换） | CPU 循环忙等 |
| 适用场景 | 锁持有时间长 | 锁持有时间极短 |
| 开销 | 切换重（≈μs级），但不占CPU | 无切换，但占 CPU |
| 中断上下文 | 不可用（可能睡眠） | 可用（需关中断） |
| 实现基础 | futex（Linux） | atomic_flag / TSL |

## Linux futex

```c
// futex（Fast Userspace Mutex）—— Linux 互斥锁核心
// 用户态先尝试原子减（无竞争时不进内核）
// 有竞争时才系统调用睡眠

// 简化实现：
void mutex_lock(int *futex) {
    // 尝试在用户态获取锁
    if (atomic_dec_if_positive(futex) >= 0)
        return;  // 获得锁，无需内核调用
    
    // 竞争发生：进入内核等待
    syscall(SYS_futex, futex, FUTEX_WAIT, 0, ...);
}

// mutex 的 FUTEX_WAIT/FUTEX_WAKE 仅在有竞争时执行系统调用 —— 快速路径（无竞争）≈ 用户态原子操作
```

---

# 经典题型速查

| 题型 | 要点 |
|------|------|
| 临界区三条件 | 互斥 + 前进 + 有限等待 |
| Peterson 算法的限制 | 不处理乱序执行，需要 memory barrier |
| 自旋锁何时用 | 锁持有时间 < 上下文切换代价（≈ 2 次） |
| 互斥锁的快速路径 | futex 无竞争时仅用户态原子操作，无系统调用 |
| 可重入锁 | 同一线程可多次获取同一锁（需计数） |
| 死锁与锁顺序 | 固定锁获取顺序可避免死锁 |
| `LOCK` 前缀作用 | 锁总线/缓存行，确保多核原子性 |

> [!tip]- **工程要点**
> 临界区应尽可能小——只保护共享数据，不要在锁内做 I/O。优先用标准库 `std::mutex`（内部已优化，快速路径 ≈ 用户态原子操作，无需自旋锁除非性能分析证明必要）。`lock_guard`/`unique_lock` 确保异常安全（RAII）。

---

进阶话题见 → [信号量](01-CS%20Core%20(计算机核心基础)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/02-Semaphore%20(信号量).md) · [死锁](01-CS%20Core%20(计算机核心基础)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/03-Deadlock%20(死锁).md)

---

# Semaphores (信号量)

> [!note] 本节重点：核心考点：信号量概念、P/V 操作、计数信号量 vs 二进制信号量、生产者消费者、读写者问题

# 信号量定义

信号量是一个非负整数变量，支持两种原子操作：

- **P（wait / down）**：如果值 > 0 则减 1，否则阻塞等待
- **V（signal / up）**：值加 1，唤醒一个等待线程

```cpp
// 信号量抽象定义
class Semaphore {
    int count;
    Queue waiting;  // 等待队列

public:
    Semaphore(int initial) : count(initial) {}

    void wait() {   // P 操作
        count--;
        if (count < 0) {
            // 将当前线程加入等待队列
            // 阻塞线程
        }
    }

    void signal() { // V 操作
        count++;
        if (count <= 0) {
            // 从等待队列移除一个线程
            // 唤醒该线程
        }
    }
};
```

## 二进制 vs 计数信号量

| 类型 | 初始值 | 用途 | 类比 |
|------|--------|------|------|
| 二进制（Mutex） | 1 | 互斥访问共享资源 | 一把钥匙 |
| 计数信号量 | N | 控制多个资源访问 | N 把钥匙 |

---

# 经典同步问题

## 生产者-消费者（有界缓冲区）

```cpp
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class BoundedBuffer {
    std::queue<int> buf;
    int capacity;
    std::mutex mtx;
    std::condition_variable not_full, not_empty;

public:
    BoundedBuffer(int cap) : capacity(cap) {}

    void produce(int item) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return buf.size() < capacity; });
        buf.push(item);
        not_empty.notify_one();
    }

    int consume() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return !buf.empty(); });
        int item = buf.front(); buf.pop();
        not_full.notify_one();
        return item;
    }
};
```

**信号量版本 P/V 操作逻辑：**

```cpp
// sem_full = 0（已用空间），sem_empty = N（空闲空间）
// sem_mutex = 1（互斥访问缓冲区）

void producer() {
    while (true) {
        item = produce_item();
        P(sem_empty);     // 申请空位
        P(sem_mutex);     // 互斥访问
        buf.push(item);
        V(sem_mutex);     // 释放互斥
        V(sem_full);      // 增加满位计数
    }
}

void consumer() {
    while (true) {
        P(sem_full);      // 申请满位
        P(sem_mutex);
        item = buf.front(); buf.pop();
        V(sem_mutex);
        V(sem_empty);     // 增加空位计数
        consume_item(item);
    }
}
```

**注意：** P 的顺序不能颠倒（先资源信号量再互斥），否则可能死锁。

---

## 读者-写者问题

```
允许多个读者同时读取，写者必须独占访问。
```

```cpp
class ReadWriteLock {
    int readers = 0;
    std::mutex mtx;
    std::condition_variable writer;

public:
    void read_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        while (readers == -1)  // 有写者
            writer.wait(lock);
        readers++;
    }

    void read_unlock() {
        std::unique_lock<std::mutex> lock(mtx);
        if (--readers == 0)
            writer.notify_one();
    }

    void write_lock() {
        std::unique_lock<std::mutex> lock(mtx);
        while (readers != 0)   // 等待所有读者完成
            writer.wait(lock);
        readers = -1;           // 标记写者占用
    }

    void write_unlock() {
        readers = 0;
        writer.notify_all();
    }
};
```

**读者优先 vs 写者优先：** 上述实现为读者优先（读者持续进入可能饿死写者）。真正的写者优先需要额外信号量。

---

## 哲学家就餐问题

```
五位哲学家围坐，每两人之间一根筷子。
需要两根筷子才能吃饭。
```

```cpp
// 方案一：信号量解法（可能死锁——每人拿左边筷子）
// 方案二：限制最多 4 人同时进食（破坏循环等待）
// 方案三：奇数先左后右，偶数先右后左（破坏循环等待）

const int N = 5;
Semaphore chopsticks[N] = {1, 1, 1, 1, 1};
Semaphore room(4);  // 方案二：最多 4 人同时吃饭

void philosopher(int i) {
    while (true) {
        think();
        room.wait();              // 占一个位
        chopsticks[i].wait();     // 左筷
        chopsticks[(i+1)%N].wait(); // 右筷
        eat();
        chopsticks[(i+1)%N].signal();
        chopsticks[i].signal();
        room.signal();
    }
}
```

---

# 条件变量 vs 信号量

| | 条件变量（condition_variable） | 信号量 |
|--|-------------------------------|--------|
| 本质 | 等待某个条件成立 | 计数资源管理 |
| 使用 | 必须配合 mutex | 独立使用 |
| 唤醒 | notify_one / notify_all | V 操作 |
| 虚假唤醒 | 需要 while 循环检查条件 | 无此问题 |
| 语义 | 无资源计数 | 显式资源计数 |

> [!tip]- **工程要点**：C++ 标准库没有信号量（C++20 才引入 `std::counting_semaphore`），多线程同步首选 `mutex + condition_variable`。信号量在生产者消费问题中自然表达资源计数，但信号量的 P/V 错序容易导致死锁——使用 `condition_variable` 时这类错误更少。

---

互斥锁与死锁详见 → [Mutex（互斥锁）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/01-Mutex%20(互斥锁).md) · [Deadlock（死锁）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/03-Deadlock%20(死锁).md)

---

# Deadlock (死锁)

> [!note] 本节重点：核心考点：死锁四条件、死锁预防/避免/检测/恢复、银行家算法、资源分配图

# 死锁定义

多个进程因竞争资源而互相等待，导致所有进程无法继续执行的状态。

---

# 死锁四必要条件

**四个条件必须同时满足才会死锁：**

| 条件 | 说明 | 破坏方法 |
|------|------|---------|
| 1. **互斥**（Mutual Exclusion） | 资源一次只能被一个进程使用 | 允许多个进程共享资源（不总是可行） |
| 2. **持有并等待**（Hold and Wait） | 进程持有资源的同时等待其他资源 | 一次性申请所有资源 / 先释放再申请 |
| 3. **不可剥夺**（No Preemption） | 已分配的资源不能强制剥夺 | 可剥夺资源（如 CPU） |
| 4. **循环等待**（Circular Wait） | 存在进程-资源循环等待链 | 按固定顺序申请资源 |

```cpp
// 死锁示例：两个线程以不同顺序获取锁
std::mutex lock1, lock2;

void thread_a() {
    lock1.lock();
    std::this_thread::sleep_for(10ms);  // 增加死锁概率
    lock2.lock();  // 可能死锁！
    lock2.unlock();
    lock1.unlock();
}

void thread_b() {
    lock2.lock();
    std::this_thread::sleep_for(10ms);
    lock1.lock();  // 可能死锁！
    lock1.unlock();
    lock2.unlock();
}

// 修复：固定锁获取顺序（都先 lock1 再 lock2）
```

---

# 死锁处理策略

## 1. 预防（Prevention）

破坏四个条件之一。常用方法—**破坏循环等待**：

```cpp
// 按地址顺序获取锁（避免死锁的通用技巧）
void safe_lock(std::mutex& a, std::mutex& b) {
    if (&a < &b) {  // 按锁地址排序
        a.lock(); b.lock();
    } else {
        b.lock(); a.lock();
    }
}
```

**其他预防方法：**
- 破坏"持有并等待"：`std::lock(a, b)` 一次性锁多个

## 2. 避免（Avoidance）

动态判断，避免进入不安全状态。

## 3. 检测（Detection）

允许死锁发生，定期检测并恢复。

## 4. 恢复（Recovery）

- **终止进程**：杀掉死锁进程（或逐个终止直到环消失）
- **资源抢占**：回滚检查点，剥夺资源

---

# 银行家算法（Banker's Algorithm）

判断系统分配资源后是否处于安全状态。

```cpp
// 银行家算法核心：判断是否存在安全序列
#include <vector>
#include <iostream>

bool is_safe(const std::vector<int>& available,
             const std::vector<std::vector<int>>& allocation,
             const std::vector<std::vector<int>>& need) {
    int n = allocation.size();  // 进程数
    int m = available.size();   // 资源类型数
    
    std::vector<int> work = available;
    std::vector<bool> finish(n, false);
    
    for (int k = 0; k < n; k++) {  // 最多尝试 n 轮
        for (int i = 0; i < n; i++) {
            if (finish[i]) continue;
            
            bool can_alloc = true;
            for (int j = 0; j < m; j++) {
                if (need[i][j] > work[j]) {
                    can_alloc = false;
                    break;
                }
            }
            
            if (can_alloc) {
                // 假设分配并回收
                for (int j = 0; j < m; j++)
                    work[j] += allocation[i][j];
                finish[i] = true;
            }
        }
    }
    
    // 检查是否全部完成
    for (bool f : finish)
        if (!f) return false;
    return true;
}

int main() {
    // 示例：5 个进程，3 种资源
    std::vector<int> available = {3, 3, 2};
    std::vector<std::vector<int>> allocation = {
        {0, 1, 0},  // P0
        {2, 0, 0},  // P1
        {3, 0, 2},  // P2
        {2, 1, 1},  // P3
        {0, 0, 2}   // P4
    };
    std::vector<std::vector<int>> need = {
        {7, 4, 3},  // P0
        {1, 2, 2},  // P1
        {6, 0, 0},  // P2
        {0, 1, 1},  // P3
        {4, 3, 1}   // P4
    };
    
    std::cout << (is_safe(available, allocation, need)
                  ? "Safe" : "Unsafe") << std::endl;
    return 0;
}
```

**安全状态**：存在一个安全序列（进程按此顺序执行都能完成）。

---

# 资源分配图

```text
┌────────────────────────────────────────────────────┐
│  RESOURCE ALLOCATION GRAPH (DEADLOCK CYCLE)        │
├────────────────────────────────────────────────────┤
│                                                    │
│   Thread A ────(holds)────→ Resource 1 (Lock)      │
│       ↑                               │            │
│       │                               │            │
│  (waited by)                    (waited by)        │
│       │                               │            │
│       │                               ▼            │
│   Resource 2 (Lock) ←──(holds)──── Thread B        │
│                                                    │
│  Cycle detected: A waits for R2, B waits for R1    │
│  → Deadlock                                        │
└────────────────────────────────────────────────────┘
```

**检测死锁：** 资源分配图中有环 ⇔ 可能存在死锁（每种资源只有一个实例时，有环 = 死锁；多种实例时需进一步判断）。

---

# 经典题型速查 · 延伸要点 2
| 题型 | 要点 |
|------|------|
| 死锁四条件 | 互斥 + 持有等待 + 不可剥夺 + 循环等待 |
| 预防 vs 避免 | 预防静态破坏条件；避免动态判断安全性 |
| 银行家算法前提 | 需预先知道每个进程的最大需求量（实际中难以满足） |
| 死锁检测 | RAG 有环 + 环上资源均只有一个实例 = 死锁 |
| 活锁 vs 死锁 | 活锁：进程在运行但无进展（如 ELB 退避重试） |
| 饥饿 vs 死锁 | 饥饿：某个进程长时间得不到资源，但其他进程仍可运行 |
| 死锁恢复代价 | 终止进程可能丢失数据，资源抢占需回滚 |

> [!tip]- **工程要点**：实际开发中最实用的死锁预防就是**固定锁顺序**和 `std::lock(a, b)` 批量获取。Linux 内核有 `lockdep` 检测潜在死锁。分布式系统中的死锁更难检测，常用超时 + 重试策略。C++ 中 RAII 包装的 `lock_guard` 在异常时自动解锁，可避免忘记释放导致的隐式死锁。

---

互斥锁与信号量详见 → [Mutex（互斥锁）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/01-Mutex%20(互斥锁).md) · [Semaphore（信号量）](/01-Foundations%20(基础能力)/01-CS%20Core%20(计算机核心)/03-Operating%20System%20(操作系统)/05-Synchronization%20(同步与互斥)/02-Semaphore%20(信号量).md)

# 零基础阅读路径

先读本页的总览与术语，再沿“数据/指令 → 硬件状态 → 操作系统抽象 → 可见结果”追踪一个例子。遇到性能数字先跳过，等能解释状态流转后再回来比较。

# 常见误区

- 把 **07-Synchronization and Deadlock (同步与死锁)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **07-Synchronization and Deadlock (同步与死锁)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
