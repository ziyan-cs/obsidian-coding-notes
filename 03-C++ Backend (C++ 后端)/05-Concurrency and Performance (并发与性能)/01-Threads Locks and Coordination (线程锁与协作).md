---
study_stage: backlog
---


> [!summary] 核心摘要
>
> 线程用于并行或并发执行任务，但线程本身不解决共享数据正确性。先划分数据所有权；只有确实共享的状态才使用 mutex、condition variable 或 semaphore 协调。正确性优先于并发度：每个共享变量都要能说明谁写、谁读、由什么同步原语建立 happens-before。

# 选择模型

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

> [!warning]- 易错点
> - 用 `volatile` 修复数据竞争；它不提供线程同步。
> - `if (condition) wait()`；虚假唤醒和竞争会让一次判断失效。
> - 创建无限线程或无限队列；资源耗尽只会被延后，而不会消失。
> - 把锁范围扩大到整个函数；这常让吞吐下降并放大死锁面。

> [!question]- 自测：先回答再展开
> 1. 为什么 condition variable 的等待必须写成带谓词的循环？
> 2. 一个线程池怎样通知 worker “不再接受新任务但处理完存量任务后退出”？
> 3. 如何从共享数据与锁顺序，而非“感觉”，证明一段代码不会死锁？

# Thread Basics POSIX & std thread (线程基础)

> [!note] 本节重点：线程的创建/汇合/分离、std::thread 与 POSIX pthread 的关系、线程生命周期管理

## std::thread 基础

```cpp
#include <thread>

// 创建线程——任何可调用对象
void worker(int id) { /* ... */ }
std::thread t1(worker, 42);                 // 函数 + 参数
std::thread t2([](int id) { /* ... */ }, 1); // Lambda
t1.join();
t2.join(); // 本示例离开作用域前汇合，避免 joinable 析构时 terminate

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

`std::thread` 析构时若仍为 `joinable()`，会调用 `std::terminate()`。`join()` 等待结束并回收线程句柄；`detach()` 让线程独立运行，之后不能再 `join()` 或查询其结果。二者是**互斥选择**，不能在同一对象上依次调用。

```cpp
std::thread t(worker, 42);
t.join();  // t 不再 joinable

// 只有确实需要独立生命周期时才考虑 detach；
// 必须保证线程使用的引用、指针和外部资源一直有效。
std::thread background(worker, 43);
background.detach();
```

优先让线程有明确的拥有者，并在析构前汇合；`detach` 不等于“自动管理生命周期”。如果需要取消协作，C++20 的 `std::jthread` 更合适。

## 参数传递与生命周期

`std::thread` 默认把实参复制或移动到内部存储。线程函数需要引用时使用 `std::ref` / `std::cref`，且调用者要保证被引用对象活到线程结束。

```cpp
#include <functional>
#include <thread>

void modify(int& x) { x = 42; }
int val = 0;
std::thread t(modify, std::ref(val));
t.join();  // 此后读取 val 才有同步保证
```

直接写 `std::thread t(modify, val)` 不能把保存的值参数绑定给 `int&`，通常编译失败。传入局部对象的指针或引用再 `detach` 则可能悬垂；即使对象仍存活，并发读写也仍需同步。

## 平台接口与线程数

`std::thread` 是跨平台接口；其 `native_handle()` 类型和可用操作取决于实现。例如在使用 POSIX 线程的环境中，可借助平台 API 设置线程名，但这部分不是可移植 C++。`hardware_concurrency()` 只是并行度提示：可能返回 0，也不保证等于容器的 CPU 配额或适合的工作线程数。

## `std::jthread` 与异常边界（C++20）

```cpp
#include <thread>

std::jthread jt([](std::stop_token stop) {
    while (!stop.stop_requested()) {
        // 每轮完成有限工作后再次检查停止请求
    }
});
jt.request_stop();
```

`jthread` 析构时，若仍可汇合，会先请求停止再 `join()`。停止请求是**协作式**的，不会强制打断死循环或阻塞的 I/O；工作函数必须主动检查，等待操作也要设计可唤醒的退出路径。

线程函数中未捕获的异常不能由创建线程的 `try/catch` 接住，会导致 `std::terminate()`。应在线程内部处理异常，或通过 `std::promise` / `std::future` 等机制把失败传回拥有者。

> [!tip] 工程取舍
> 建线程、栈空间、调度与上下文切换都有成本，但固定的“几微秒”并不适用于所有机器和负载。短任务通常复用线程池；CPU 密集型任务可从可用并行度附近起步测量，I/O 密集型任务还需考虑阻塞比例、队列与资源上限，不能简单规定线程数不得超过 `hardware_concurrency()`。

---

# Mutex & Lock (互斥锁与锁管理)

> [!note] 本节重点：互斥锁保护共享数据、死锁预防、RAII 锁管理、锁的粒度

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

## 死锁与锁顺序

同时持有多把锁时，线程 A 若先拿 `a` 再等 `b`，线程 B 却先拿 `b` 再等 `a`，就可能形成循环等待。固定全局锁顺序是一种解法；同一组锁也可用 `std::scoped_lock` 的多锁构造，避免手写 `lock`/`adopt_lock` 配对。

```cpp
#include <mutex>

struct PairState {
    std::mutex left_mutex;
    std::mutex right_mutex;
    int left = 0;
    int right = 0;

    void update_both() {
        std::scoped_lock both{left_mutex, right_mutex};
        ++left;
        ++right;
    }
};
```

`scoped_lock` 避免的是这组互斥量互相争用时的加锁死锁，不保证调用链在别处没有锁环。调用外部回调、日志或阻塞 I/O 前，仍要审查是否持锁及其反向调用路径。

## 锁的粒度与快照

临界区只覆盖维持共享不变量所需的读写，但不能把依赖共享状态的检查盲目移出去。需要在锁外做长计算时，先在锁内取一致快照，再对快照计算：

```cpp
#include <mutex>
#include <vector>

struct Repository {
    std::mutex mutex;
    std::vector<int> data;

    std::vector<int> snapshot() {
        std::lock_guard lock{mutex};
        return data; // 拷贝可能耗时；若是热点，重新设计数据所有权。
    }
};

int sum_snapshot(Repository& repo) {
    const auto copy = repo.snapshot();
    int sum = 0;
    for (int value : copy) sum += value;
    return sum;
}
```

快照方式给出的是取快照时刻的一致数据，不自动保证“计算完成时仍为最新”。若操作必须检查并提交同一个条件，应在锁内完成或使用版本校验/事务协议。

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

> [!warning]- 易错点
> ```cpp
> // 1. 返回受保护数据的引用（锁失效！）
> std::vector<int>& unsafe_get() {
>     std::lock_guard lock(mtx);
>     return data;  // ❌ 返回引用后，调用者可能不用锁就访问
> }
>
> // 2. 锁的传递
> void bad() {
>     std::unique_lock lock(mtx);
>     // 把 lock 传递给下层函数
>     helper(lock);  // 函数内可能 unlock/lock，破坏封装
> }
>
> // 3. 忘记锁
> void unchecked() {  // ❌ 忘记加锁
>     shared_data++;
> }
> ```
>
> > **面试重点**：`lock_guard` vs `unique_lock` vs `scoped_lock`（C++17）。`scoped_lock` 等价于 `lock_guard` 但支持多锁且用 `std::lock` 避免死锁。在需要锁多个 mutex 时首选 `scoped_lock`。
>
> ---
>
>
> ---

# Condition Variable & Semaphore (条件变量与信号量)

> [!note] 本节重点：条件变量解决"等待某个条件成立"的问题、虚假唤醒、信号量 vs 条件变量的选择

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

## `notify_one` 与 `notify_all`

选择依据是**当前状态变化后有多少等待者可能前进**，而不是生产者和消费者各有几个。队列新增一个任务，通常 `notify_one()` 就够了，即使是多生产者、多消费者；关闭队列、广播配置变化等使所有等待者都应重新检查状态时，用 `notify_all()`。通知可能被合并或在无人等待时消失，真正的条件必须保存在受锁保护的状态里。

## 信号量（Semaphore，C++20）

计数信号量保存“还有多少许可”；`acquire()` 等待并消耗一个许可，`release()` 归还许可。它不自动保护被访问对象的内部状态。

```cpp
#include <semaphore>

std::counting_semaphore<3> slots(3);  // 同时最多三个许可

void use_resource() {
    slots.acquire();
    try {
        // 占用一个有限资源；共享数据仍需各自的同步保护
        slots.release();
    } catch (...) {
        slots.release();
        throw;
    }
}
```

真实代码应把归还许可封装成 RAII，避免新增提前返回路径时漏掉 `release()`；不要把这种手工 `try/catch` 当作最终资源管理方案。`std::binary_semaphore` 通常定义为 `counting_semaphore<1>` 的别名。

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

## 通知时机与超时

`notify_one()` 在锁内或锁外调用都可以是正确的；常见做法是先在锁内修改谓词，再解锁通知，以免被唤醒的线程立刻竞争同一把锁。是否能安全地在锁外通知还取决于等待者、条件变量及其拥有对象的生命周期，不可一概而论。

```cpp
{
    std::lock_guard<std::mutex> lock(mtx);
    ready = true;
}
cv.notify_one();
```

忘记通知时，`wait_for` 的超时可以帮助发现卡住，但**不能代替通知协议**。超时返回也需在持锁状态下重新检查谓词，业务上要区分“达到条件”和“等待超时”。

> **面试重点**：条件变量内部的 `wait` 做了三步：1）解锁 mutex；2）阻塞等待通知；3）被唤醒后重新加锁。所以 `wait` 需要 `unique_lock`（可手动 lock/unlock）而不是 `lock_guard`。

---


> [!info]- 延伸阅读
> - 下一步：[02-Atomics and Memory Order (原子与内存序)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/02-Atomics%20and%20Memory%20Order%20(原子与内存序).md)
