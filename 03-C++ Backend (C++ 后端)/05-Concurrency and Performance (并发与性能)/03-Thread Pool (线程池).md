---
study_stage: backlog
---

> [!abstract] 学习目标
> 线程池不是“把任务塞进队列”这么简单。读完应能解释提交、唤醒、背压、异常传递和关闭协议，并用测试验证任务不丢失。以下示例以 C++17 为基线。

# 先定义线程池的契约

线程池用固定数量的 worker 复用线程，适合大量相对独立的短任务。队列解耦提交者与执行者，但也会引入排队延迟和积压。本篇采用以下可验证的契约：

| 场景 | 行为 |
|---|---|
| `submit` 成功 | 返回 `future`；任务恰好从队列取出一次 |
| 队列满 | 立即拒绝并抛异常，不无限积压 |
| `shutdown` 开始后提交 | 拒绝新任务 |
| `shutdown` | 处理完已接受任务，再 `join` worker |
| 任务抛异常 | `packaged_task` 将异常传到对应 `future::get()` |

这是**排空式关闭**（drain），不是取消；执行中的任务若永久阻塞，关闭也会一直等待。调用者应在外部停止生产新任务，保证线程池对象活到所有并发调用结束。不要从此池的 worker 内调用 `shutdown()`，否则可能等待自身；本教学实现要求只有一个控制线程执行关闭。

# 一个有界、可编译的教学实现

```cpp
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t workers, std::size_t queue_capacity)
        : capacity_(queue_capacity) {
        if (workers == 0 || capacity_ == 0)
            throw std::invalid_argument("workers and capacity must be positive");
        threads_.reserve(workers);
        try {
            for (std::size_t i = 0; i < workers; ++i)
                threads_.emplace_back([this] { run(); });
        } catch (...) {
            { std::lock_guard<std::mutex> lock(mutex_); stopping_ = true; }
            ready_.notify_all();
            for (auto& t : threads_) t.join();
            throw;
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<class F, class... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> {
        using R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
        auto bound = [fn = std::forward<F>(f),
                      values = std::make_tuple(std::forward<Args>(args)...)]() mutable -> R {
            return std::apply(std::move(fn), std::move(values));
        };
        auto task = std::make_shared<std::packaged_task<R()>>(std::move(bound));
        auto result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_) throw std::runtime_error("pool is stopping");
            if (tasks_.size() == capacity_) throw std::runtime_error("queue is full");
            tasks_.emplace([task] { (*task)(); });
        }
        ready_.notify_one();
        return result;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }
        ready_.notify_all();
        for (auto& t : threads_)
            if (t.joinable()) t.join();
    }

    ~ThreadPool() { shutdown(); }

private:
    void run() {
        for (;;) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                ready_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) return;
                job = std::move(tasks_.front());
                tasks_.pop();
            }
            job(); // packaged_task 捕获任务异常并交给 future
        }
    }

    const std::size_t capacity_;
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable ready_;
    bool stopping_{false};
};
```

此实现的队列容量只限制**等待中**任务，不限制已运行任务；提交过快会收到拒绝。复制进入 `std::function` 的是可复制的 `shared_ptr` 包装器，因此封装的实际可调用对象可以是 move-only。传入参数在提交时按值保存；确实需要引用语义时显式用 `std::ref`，并确保被引用对象活到任务结束。`future::get()` 可取返回值或重新抛出任务异常；丢弃 `future` 就失去了该错误通道。

构造函数的 `catch` 负责在线程创建失败时停止并回收已启动的线程。这也说明：构造失败清理不能依赖尚未构造完成的对象析构函数。

# 使用与验证

```cpp
#include <cassert>
#include <stdexcept>

int main() {
    ThreadPool pool{2, 16};
    auto answer = pool.submit([](int a, int b) { return a + b; }, 3, 4);
    assert(answer.get() == 7);

    auto failure = pool.submit([]() -> int {
        throw std::runtime_error("task failed");
    });
    try {
        (void)failure.get();
        assert(false);
    } catch (const std::runtime_error&) {
        // 异常由 future 返回给提交者，worker 继续运行。
    }
    pool.shutdown(); // 排空已接受任务
}
```

将两个代码块合并编译：`g++ -std=c++17 -pthread -Wall -Wextra pool.cpp`。还应写测试：零 worker／零容量必须拒绝；提交和关闭并发时，每个**成功提交**的任务最终完成；阻塞任务使队列填满时能观察拒绝。测试并发行为要重复运行并配合 ThreadSanitizer，不把单次通过当证明。

# 线程数与进一步工程化

`std::thread::hardware_concurrency()` 只是提示值，可能返回 0，也不等于容器 CPU 配额或“物理核心数”。线程数受任务 CPU/IO 比例、阻塞时间、队列长度、上下文切换和尾延迟影响；从可测基线开始调整。任务里再向同一个已满线程池同步提交并等待结果，可能发生饥饿或死锁。

生产级实现还要明确：多提交者与关闭方的生命周期同步、超时和取消策略、队列公平性、指标、worker 崩溃策略、任务优先级是否真的需要，以及容量拒绝或阻塞的背压选择。工作窃取、动态扩缩容不是“必须有”，只有负载证据支持时再增加复杂度。

> [!info]- 延伸阅读
> - [01-Threads Locks and Coordination (线程锁与协作)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/01-Threads%20Locks%20and%20Coordination%20(线程锁与协作).md)
> - [04-Lock Free and Performance (无锁与性能)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/05-Concurrency%20and%20Performance%20(并发与性能)/04-Lock%20Free%20and%20Performance%20(无锁与性能).md)
