---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 01-Threads Locks and Coordination (线程锁与协作)

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 30 秒回答

线程用于并行或并发执行任务，但线程本身不解决共享数据正确性。先划分数据所有权；只有确实共享的状态才使用 mutex、condition variable 或 semaphore 协调。正确性优先于并发度：每个共享变量都要能说明谁写、谁读、由什么同步原语建立 happens-before。

## 选择模型

| 问题 | 优先方案 | 关键约束 |
| --- | --- | --- |
| 互斥访问少量共享状态 | `std::mutex` + RAII lock | 临界区短，不在持锁时做慢 I/O |
| 等待状态变化 | `std::condition_variable` | 用谓词循环等待，防虚假唤醒 |
| 控制有限资源数量 | semaphore | 明确 acquire/release 的所有权 |
| 大量独立任务 | thread pool + queue | 需要停止、背压、异常与任务生命周期 |

## 关键不变量

1. 锁保护的是**数据不变量**，不是“某一行代码”。
2. 等待条件必须与锁关联；被唤醒后仍要重新检查条件。
3. 多把锁必须有固定顺序，或使用统一的加锁策略，避免死锁。
4. 线程退出、任务取消与队列关闭都要有明确协议，不能依赖进程结束回收。



## 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

## 常见误区

- 用 `volatile` 修复数据竞争；它不提供线程同步。
- `if (condition) wait()`；虚假唤醒和竞争会让一次判断失效。
- 创建无限线程或无限队列；资源耗尽只会被延后，而不会消失。
- 把锁范围扩大到整个函数；这常让吞吐下降并放大死锁面。

## 自测

1. 为什么 condition variable 的等待必须写成带谓词的循环？
2. 一个线程池怎样通知 worker “不再接受新任务但处理完存量任务后退出”？
3. 如何从共享数据与锁顺序，而非“感觉”，证明一段代码不会死锁？

## Thread Basics POSIX & std thread (线程基础)

> [!note] 本节重点：核心考点：线程的创建/汇合/分离、std::thread 与 POSIX pthread 的关系、线程生命周期管理

## std::thread 基础

```cpp
#include <thread>

// 创建线程——任何可调用对象
void worker(int id) { /* ... */ }
std::thread t1(worker, 42);                 // 函数 + 参数
std::thread t2([](int id) { /* ... */ }, 1); // Lambda

// RAII 包装：确保线程被 join 或 detach
class ThreadGuard {
    std::thread& t;
public:
    explicit ThreadGuard(std::thread& t_) : t(t_) {}
    ~ThreadGuard() {
        if (t.joinable()) t.join();
    }
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};
```

## 线程生命周期管理

```cpp
std::thread t(worker, 42);

t.join();   // 阻塞等待线程结束，之后 t 不再 joinable
// 或
t.detach(); // 分离，线程在后台运行，t 不再关联线程

// 重要：析构前必须 join 或 detach
// 否则 std::thread::~thread() 会调用 std::terminate()！
```

**关键规则**：
- 每个 `std::thread` 对象在析构前必须调用 `join()` 或 `detach()`
- `joinable()` 检查线程是否可被 join
- `detach` 后的线程无法再获取其状态

## 参数传递陷阱

```cpp
// ❌ 危险：传递引用时忘记用 std::ref
void modify(int& x) { x = 42; }
int val = 0;
std::thread t(modify, val);    // 编译错误或拷贝！thread 会拷贝参数
std::thread t(modify, std::ref(val));  // ✅ 正确传递引用

// ❌ 危险：传入临时对象的指针
void process(const Data& d);
Data d;
std::thread t(process, std::cref(d));  // ✅ 确保 d 在线程执行期间存活
```

## 线程与 POSIX pthread 的关系

```cpp
// std::thread 底层封装了 pthread（Linux/macOS）或 Windows Threads

// 获取原生句柄
std::thread t(worker, 1);
pthread_t handle = t.native_handle();  // Linux 返回 pthread_t
pthread_setname_np(handle, "worker-1"); // 设置线程名称（调试用）
t.detach();

// 硬件并发
unsigned int n = std::thread::hardware_concurrency();  // 逻辑 CPU 核心数
```

## std::jthread (C++20)

```cpp
// C++20 引入：自动 join + 可取消
std::jthread jt([](std::stop_token st) {
    while (!st.stop_requested()) {
        // 工作循环
    }
});
// jt 析构时自动 join()

// 请求停止
jt.request_stop();
```

## 线程 ID 与异常安全

```cpp
// 获取当前线程 ID
std::cout << std::this_thread::get_id();

// 异常安全：在线程中捕获所有异常
try {
    std::thread t([&] {
        try {
            throw std::runtime_error("error");
        } catch (...) {
            // 处理异常
        }
    });
    t.join();
} catch (...) {
    // 不能在此捕获线程内的异常！
}
```

> [!tip]- **工程要点**：线程是稀缺资源。创建线程的开销大约为几微秒（栈分配 + 系统调用）。**不要为短任务创建线程**——用线程池。一个进程的线程数通常不超过 `hardware_concurrency`。

---

互斥锁与锁管理详见 → [Mutex & Lock (互斥锁与锁管理)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20Programming%20(并发编程)/02-Mutex%20&%20Lock%20(互斥锁与锁管理)%20⭐.md)

---

## Mutex & Lock (互斥锁与锁管理)

> [!note] 本节重点：核心考点：互斥锁保护共享数据、死锁预防、RAII 锁管理、锁的粒度

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

---

## Condition Variable & Semaphore (条件变量与信号量)

> [!note] 本节重点：核心考点：条件变量解决"等待某个条件成立"的问题、虚假唤醒、信号量 vs 条件变量的选择

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

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Threads Locks and Coordination (线程锁与协作)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
