---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

**核心结论**：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Thread Pool Implementation (线程池手写)

> [!note] 本节重点：核心考点：> 线程池的设计与实现、任务队列、动态扩缩容、C++ 后端面试手撕代码

# 基础线程池实现

```cpp
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency())
        : stop_(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(queue_mtx_);
                        cv_.wait(lock, [this] {
                            return stop_ || !tasks_.empty();
                        });
                        if (stop_ && tasks_.empty())
                            return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        {
            std::lock_guard lock(queue_mtx_);
            if (stop_)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return result;
    }

    ~ThreadPool() {
        {
            std::lock_guard lock(queue_mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& worker : workers_)
            worker.join();
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mtx_;
    std::condition_variable cv_;
    bool stop_;
};
```

# 使用示例

```cpp
ThreadPool pool(4);

// 提交无返回值任务
pool.enqueue([] { std::println("Task"); });

// 提交带返回值的任务
auto future = pool.enqueue([](int a, int b) { return a + b; }, 3, 4);
int result = future.get();  // 7

// 批量任务
std::vector<std::future<int>> futures;
for (int i = 0; i < 100; ++i) {
    futures.push_back(pool.enqueue([i] { return i * i; }));
}
```

# 线程数设计原则

| 应用类型 | 推荐线程数 | 原因 |
|---------|-----------|------|
| CPU 密集型 | `hardware_concurrency` | 避免过多线程竞争 CPU |
| IO 密集型 | `hardware_concurrency * 2` 或更高 | IO 等待时让出 CPU |
| 混合型 | 分离 CPU/IO 线程池 | 避免 IO 阻塞 CPU 任务 |

```cpp
// 获取 CPU 核心数（不仅仅是逻辑线程数，考虑超线程）
unsigned int cpu_cores() {
    unsigned int threads = std::thread::hardware_concurrency();
    // 在 Linux 上可以读取 /proc/cpuinfo 获取物理核心数
    return threads > 0 ? threads : 4;  // fallback
}
```

# 进阶设计：动态调整

```cpp
class DynamicThreadPool {
    // ...
    void adjust(size_t target) {
        while (workers_.size() < target) {
            workers_.emplace_back([this] { /* worker loop */ });
        }
        if (target < workers_.size()) {
            resize_ = true;
            cv_.notify_all();  // 让多余线程退出
        }
    }
    // 需要在线程循环中检测 resize_ 标志并退出
};
```

# 常见面试题：线程池核心要素

| 要素 | 实现方式 |
|------|---------|
| 任务队列 | `std::queue` + `std::mutex` + `std::condition_variable` |
| 线程创建 | `std::vector<std::thread>` |
| 获取返回值 | `std::packaged_task` + `std::future` |
| 优雅关闭 | 设置 stop 标志 → notify_all → join |
| 异常安全 | task 内异常存储在 future 中，get 时重新抛出 |

```cpp
// 不使用 packaged_task 的简化版（无返回值）
class SimplePool {
    // 同上但 enqueue 返回 void
    void enqueue(std::function<void()> task) {
        // 直接 push 到队列
    }
};
```

> [!tip]- **工程要点**
> 生产级线程池还需要：**工作窃取**（每条线程有自己的任务队列）、**优先级队列**（紧急任务插队）、**定时任务**、**监控接口**（当前队列深度、活跃线程数）。但面试手撕以上基础版本就够了。

# 30 秒回答 / 自测

- **30 秒回答**：线程池 = 任务队列 + 固定线程集合 + 条件变量通知。`enqueue` 把任务包装成 `packaged_task` 入队，worker 循环取任务执行，析构时置 stop 标志 + `notify_all` + `join`。
- **常见误区**：`enqueue` 里先检查 `stop_` 再 push 存在竞态（应持锁检查）；worker 里 `stop_` 为 true 且队列非空时过早退出（丢任务）。
- **自测**：1) 为什么 `enqueue` 的 `stop_` 判断必须在锁内？ 2) 任务抛异常会怎样，如何拿回异常？

---

配套的并发原语见 → [Mutex & Lock](../02-Mutex%20&%20Lock%20(互斥锁与锁管理)%20⭐.md) · [Condition Variable](../03-Condition%20Variable%20&%20Semaphore%20(条件变量与信号量)%20⭐.md)
- Lock-free Structures Overview (无锁结构概念)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Thread Pool (线程池)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Concurrency Map (并发与性能导航)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/00-Concurrency%20Map%20(并发与性能导航).md)
- 下一步：[04-Lock Free and Performance (无锁与性能)](/02-C++%20Backend%20(C++%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/04-Lock%20Free%20and%20Performance%20(无锁与性能).md)
