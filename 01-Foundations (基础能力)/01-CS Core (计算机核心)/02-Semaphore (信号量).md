---
tags:
  - cs/os
status: 🌱
---

# Semaphore — 信号量

> [!abstract] 核心考点：信号量概念、P/V 操作、计数信号量 vs 二进制信号量、生产者消费者、读写者问题

## 信号量定义

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

### 二进制 vs 计数信号量

| 类型 | 初始值 | 用途 | 类比 |
|------|--------|------|------|
| 二进制（Mutex） | 1 | 互斥访问共享资源 | 一把钥匙 |
| 计数信号量 | N | 控制多个资源访问 | N 把钥匙 |

---

## 经典同步问题

### 生产者-消费者（有界缓冲区）

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

### 读者-写者问题

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

### 哲学家就餐问题

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

## 条件变量 vs 信号量

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
