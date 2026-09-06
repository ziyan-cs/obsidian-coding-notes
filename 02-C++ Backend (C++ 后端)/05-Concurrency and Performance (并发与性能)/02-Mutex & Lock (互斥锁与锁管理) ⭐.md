---
tags:
  - cpp/concurrency
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# 02-Mutex & Lock (互斥锁与锁管理)

> [!abstract] 核心考点：互斥锁保护共享数据、死锁预防、RAII 锁管理、锁的粒度

## std::mutex 与 RAII 锁

```cpp
#include <mutex>

std::mutex mtx;
int shared_data = 0;

// ✅ lock_guard：构造时 lock，析构时 unlock（最简单的 RAII 包装）
void safe_increment() {
    std::lock_guard<std::mutex> lock(mtx);
    ++shared_data;
}  // 自动 unlock

// ✅ unique_lock：比 lock_guard 更灵活（可提前 unlock、延迟锁定、转移所有权）
std::unique_lock<std::mutex> ulock(mtx, std::defer_lock);  // 先不锁
// ... 做一些不需要锁的操作
ulock.lock();  // 手动锁
shared_data++;
ulock.unlock();  // 提前解锁
```

## 锁类型对比

| 锁 | 用途 | 特点 |
|----|------|------|
| `std::mutex` | 基础互斥锁 | 独占锁，不支持递归 |
| `std::recursive_mutex` | 同一线程可多次 lock | 每次 lock 需对应 unlock |
| `std::timed_mutex` | 带超时的互斥锁 | `try_lock_for` / `try_lock_until` |
| `std::shared_mutex` (C++17) | 读写锁 | `lock_shared` 共享读，`lock` 独占写 |

```cpp
// shared_mutex：读多写少场景
std::shared_mutex rw_mtx;
int data = 0;

void reader() {
    std::shared_lock lock(rw_mtx);       // 共享锁
    std::cout << data;                    // 只读
}

void writer() {
    std::unique_lock lock(rw_mtx);       // 独占锁
    ++data;
}
```

## 死锁与预防

**死锁四条件**：
1. 互斥（资源不可共享）
2. 持有并等待（线程持有资源同时等待其他资源）
3. 不可剥夺（资源必须由持有者释放）
4. 循环等待（A 等 B，B 等 A 的资源）

```cpp
// ❌ 典型死锁：两个锁顺序不一致
std::mutex a, b;

void thread1() {
    std::lock_guard lk1(a);
    std::lock_guard lk2(b);  // 与 thread2 顺序相反 → 可能死锁
}

void thread2() {
    std::lock_guard lk1(b);
    std::lock_guard lk2(a);
}

// ✅ 方案 1：固定锁顺序（都先锁 a 再锁 b）
void thread1() {
    std::lock_guard lk1(a);
    std::lock_guard lk2(b);
}
void thread2() {
    std::lock_guard lk1(a);  // 同样顺序
    std::lock_guard lk2(b);
}

// ✅ 方案 2：std::lock 一次锁多个（C++11）
void safe_lock() {
    std::lock(a, b);  // 同时锁 a 和 b，避免死锁
    // 所有权已转移，但仍需管理解锁
    std::lock_guard lk1(a, std::adopt_lock);
    std::lock_guard lk2(b, std::adopt_lock);
    // 临界区
}
```

## 锁的粒度

```cpp
// ❌ 粗粒度：整个操作期间持有锁（性能差）
void process_big() {
    std::lock_guard lock(mtx);
    read_sensor();      // 可能花 100ms
    compute_result();   // 可能花 50ms
    write_database();   // 可能花 200ms
}

// ✅ 细粒度：只在访问共享数据时持有锁
void process_better() {
    auto data = [&] {
        std::lock_guard lock(mtx);
        return read_sensor();    // 只锁这行
    }();
    compute_result(data);        // 不锁
    // ...
}
```

## std::call_once

```cpp
// 线程安全的单次初始化
std::once_flag flag;

void init() { /* 只执行一次 */ }

void worker() {
    std::call_once(flag, init);  // 多线程同时调用，只有第一个会执行 init
    // 之后的工作
}
```

## 常见陷阱

```cpp
// 1. 返回受保护数据的引用（锁失效！）
std::vector<int>& unsafe_get() {
    std::lock_guard lock(mtx);
    return data;  // ❌ 返回引用后，调用者可能不用锁就访问
}

// 2. 锁的传递
void bad() {
    std::unique_lock lock(mtx);
    // 把 lock 传递给下层函数
    helper(lock);  // 函数内可能 unlock/lock，破坏封装
}

// 3. 忘记锁
void unchecked() {  // ❌ 忘记加锁
    shared_data++;
}
```

> **面试重点**：`lock_guard` vs `unique_lock` vs `scoped_lock`（C++17）。`scoped_lock` 等价于 `lock_guard` 但支持多锁且用 `std::lock` 避免死锁。在需要锁多个 mutex 时首选 `scoped_lock`。

---

条件变量与互斥锁配合使用，详见 → [Condition Variable & Semaphore (条件变量与信号量)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/03-Condition%20Variable%20&%20Semaphore%20(条件变量与信号量)%20⭐.md)
