---
tags:
  - cpp/concurrency
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# 03-Condition Variable & Semaphore (条件变量与信号量)

> [!abstract] 核心考点：条件变量解决"等待某个条件成立"的问题、虚假唤醒、信号量 vs 条件变量的选择

## 条件变量（condition_variable）

```cpp
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
int data = 0;

// 生产者
void producer() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        data = 42;
        ready = true;
    }
    cv.notify_one();  // 通知一个等待线程
    // cv.notify_all();  // 通知所有等待线程
}

// 消费者
void consumer() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; });  // ✅ 等待条件成立
    // 等价于：
    // while (!ready) cv.wait(lock);
    std::cout << data;
}
```

## 虚假唤醒（Spurious Wakeup）

```cpp
// wait 的第二种形式是 while 循环 + wait 的语法糖：
cv.wait(lock);           // ❌ 裸等待：可能被虚假唤醒
                         // 被唤醒后需要重新检查条件

// ✅ 使用带谓词的 wait（内部就是 while 循环）
cv.wait(lock, [] { return ready; });
// 等价于：
// while (!ready) cv.wait(lock);
```

**为什么需要处理虚假唤醒**：
- 操作系统层面：线程可能从 `wait` 返回但条件并未满足
- 必须**始终在循环中检查条件**，不能假设被唤醒就是条件满足了

## notify_one vs notify_all

| | `notify_one` | `notify_all` |
|--|-------------|--------------|
| 行为 | 只唤醒一个等待线程 | 唤醒所有等待线程 |
| 适用场景 | 单生产者-单消费者 | 多生产者-多消费者 / barrier 模式 |
| 性能 | 更好（只唤醒一个） | 较差的惊群效应 |

## 信号量（Semaphore, C++20）

```cpp
#include <semaphore>

std::counting_semaphore<10> sem(3);  // 最大计数 10，初始值 3
// std::binary_semaphore 是 counting_semaphore<1> 的别名

void worker(int id) {
    sem.acquire();  // P 操作：计数器 -1，如果为 0 则阻塞
    // 访问有限资源
    std::println("Worker {} is working", id);
    std::this_thread::sleep_for(1s);
    sem.release();  // V 操作：计数器 +1，唤醒等待者
}
```

## 条件变量 vs 信号量

| | 条件变量 | 信号量（C++20） |
|--|---------|----------------|
| 核心用途 | 等待"条件"满足 | 控制"资源"的并发访问数 |
| **必须与 mutex 配合** | ✅ 必须 | ❌ 不需要 |
| 内部状态 | 无状态（只负责等待/通知）| 有状态（计数）|
| 虚假唤醒 | 需要处理 | 不需要（直接操作计数）|
| 适用场景 | 复杂的条件等待 | 简单的资源限流 |

```cpp
// ✅ 条件变量适合：等待队列非空/缓冲区有数据
// ✅ 信号量适合：限制同时访问数据库连接数

// C++17 前没有标准信号量，可以用条件变量模拟
// C++20 提供了 std::counting_semaphore
```

## 工程陷阱

```cpp
// ❌ 在持有锁时 notify
// 虽不会出错，但被唤醒的线程会立刻尝试获取锁，造成不必要的上下文切换
{
    std::lock_guard lock(mtx);
    ready = true;
    cv.notify_one();  // 锁内 notify
} // ✅ 更好的做法：在锁外 notify

// ✅ 推荐：锁内修改数据，锁外 notify
{
    std::lock_guard lock(mtx);
    ready = true;
}
cv.notify_one();

// ❌ 忘记 notify
// 消费者永远阻塞——超时是常见的调试手段
cv.wait_for(lock, 1s, [] { return ready; });  // 带超时的等待
```

> **面试重点**：条件变量内部的 `wait` 做了三步：1）解锁 mutex；2）阻塞等待通知；3）被唤醒后重新加锁。所以 `wait` 需要 `unique_lock`（可手动 lock/unlock）而不是 `lock_guard`。

---

互斥锁与锁管理详见 → [Mutex & Lock (互斥锁与锁管理)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/02-Mutex%20&%20Lock%20(互斥锁与锁管理)%20⭐.md)
