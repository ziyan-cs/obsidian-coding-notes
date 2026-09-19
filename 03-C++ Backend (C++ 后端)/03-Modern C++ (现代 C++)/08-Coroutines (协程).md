---
status: stable
confidence: high
content_verified: 2026-09-19
---

> [!abstract] 阅读方式：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

> [!summary] 核心摘要
>
> C++20 协程由编译器转换为可暂停、可恢复的状态机；它擅长组织异步控制流，但不等于线程，也不会自动提供调度器、取消或资源安全。

# 协程模型与编译器变换

> [!note] 本节重点：C++20 协程可暂停和恢复，暂停所需状态保存在协程状态中；它本身不提供线程调度或异步 I/O，需要调度器及 I/O 库配合。

## 什么是协程

```cpp
// 协程是"可暂停和恢复执行的函数"
// 当函数中含有以下关键字之一，它就是协程：
// co_await  — 等待异步操作
// co_yield  — 产生一个值（类似生成器）
// co_return — 返回并结束协程

// 最简单的协程起手式
generator<int> counter(int n) {
    for (int i = 0; i < n; ++i)
        co_yield i;  // 每次产出一个值，暂停执行
}  // 调用 next() 时恢复执行
```

**C++20 协程是无栈协程**：暂停状态保存在 coroutine frame 中；frame 的存储与暂停/恢复成本由实现、返回类型和 awaitable 决定，不能假定总在堆上或固定快于线程切换。

## 三个核心概念

```text
协程框架由三部分组成：
1. promise_type    — 控制协程的行为（返回值、异常处理）
2. coroutine_handle— 操作协程的句柄（恢复/销毁）
3. awaitable       — 定义了 co_await 行为（是否暂停、暂停后做什么）
```

但这些在工程中可以靠库来封装（不需要每次手动实现）：

```cpp
// 使用 cppcoro 库（或自行封装）后的实际使用
cppcoro::task<int> fetch_data() {
    auto result = co_await http_get("api.example.com/data");
    // 发起请求 → 暂停 → 等待 IO 完成 → 恢复 → 继续执行
    co_return parse_result(result);
}

cppcoro::task<> process() {
    auto data = co_await fetch_data();
    std::println("Got: {}", data);
}
```

# 返回对象与执行模式

```cpp
// 一个简单的 Generator 封装（简化版）
template<typename T>
struct Generator {
    struct promise_type {
        T current_value;
        
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    
    std::coroutine_handle<promise_type> handle_;
    
    explicit Generator(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Generator() { if (handle_) handle_.destroy(); }
    
    bool next() {
        handle_.resume();
        return !handle_.done();
    }
    T value() { return handle_.promise().current_value; }
};

// 使用：
Generator<int> fib(int n) {
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    auto gen = fib(10);
    while (gen.next()) {
        std::cout << gen.value() << " ";  // 0 1 1 2 3 5 8 13 21 34
    }
}
```

## Task（异步任务）模式

```cpp
// Task 是协程最常用的模式：封装异步操作
// 完整的实现较复杂，通常使用库（cppcoro, folly::coro）

// 概念理解：一个协程的执行流
task<int> async_process(int x) {
    // 在协程框架中：
    // 1. initial_suspend：是否立即开始（suspend_never）还是延迟（suspend_always）
    auto result = co_await async_compute(x);
    // 2. co_await awaitable：根据 awaitable 决定是否暂停
    // 3. 当 async_compute 完成时，恢复执行
    co_return result + 1;
}
```

# 协程 vs 线程 vs 回调

| 特性 | 协程 (C++20) | 线程 | 回调 |
|------|-------------|------|------|
| 开销 | frame 分配与调度取决于实现 | 线程栈与内核调度 | 闭包与调度取决于框架 |
| 暂停/恢复 | ✅ 语言支持 | ❌ 需要系统调度 | ✅ 函数调用 |
| 同步写法 | ✅ 同步风格写异步 | ✅ 同步 | ❌ 回调地狱 |
| 并行 | ❌（同一线程内协作）| ✅（真并行）| ❌ |
| 栈需求 | 无栈 | 有栈 (MB 级) | 无栈 |

```cpp
// 协程让异步代码看起来像同步代码
// 对比：

// 回调方式
void fetch_callback() {
    async_request([](Response r) {
        async_process(r, [](Result res) {
            std::cout << res;
        });
    });
}

// 协程方式（同步风格）
task<void> fetch_coro() {
    auto r = co_await async_request();  // 像同步调用
    auto res = co_await async_process(r);
    std::cout << res;
}
```

## 工程注意事项

```cpp
// 1. 协程默认在堆上分配状态
// 某些场景可以用 std::noop_coroutine 优化

// 2. 协程无法用 return 返回值（必须用 co_return）
// ❌ int coro() { return 42; }  // 不是协程
// ✅ task<int> coro() { co_return 42; }

// 3. 协程中慎用线程局部存储（TLS）
// 协程可能在恢复时切换到不同线程

// 4. 非对称转移：一个协程只能"返回"给它的调用者/恢复者
// 对称转移需要库级支持

// 5. 标准库支持有限（C++20）
// C++20 只提供了协程框架（coroutine_handle, promise_type, awaitable）
// 没有提供标准 task / generator
// 需要使用第三方库（cppcoro, folly::coro）或自己封装
```

> [!tip]- **工程要点**：C++20 协程是"框架级"设施（像虚函数/模板一样），不是"开箱即用"的。生产项目中需要配合库使用（cppcoro, folly::coro 或自己封装）。协程的最大价值是**用同步写法写异步代码**，消除回调地狱。对于 IO 密集型的后端服务，协程是比线程更轻量的并发方案。

---

# 生命周期、取消与工程边界

编译器把局部状态、promise 和暂停点所需信息放入 coroutine frame。frame 经常通过分配函数获得存储，但标准允许在满足条件时省略/嵌入分配，因此不能断言“总在堆上”或给出固定开销。真正成本要由目标编译器、awaitable 和负载测量。

调用协程函数通常先创建返回对象，不等于异步工作已经开始；`initial_suspend` 决定 eager/lazy。暂停后由谁持有 handle、谁恢复、谁最终 `destroy()` 必须唯一明确。对已完成或已销毁 handle 调用 `resume` 是错误，泄漏 handle 会泄漏整个 frame。

`co_await` 不自动切换线程。awaitable 的 `await_suspend` 把 continuation 注册到事件循环/执行器，完成事件在哪个线程恢复取决于库。协程恢复到不同线程后，原线程的 TLS、锁和线程亲和资源可能不再成立。

## 取消、异常和生命周期

取消不是 C++20 核心协程自动提供的能力，需要库定义 stop token、取消槽或操作对象。取消后仍必须等待底层 I/O 回调不再引用 frame，不能先销毁 frame 再让完成回调恢复悬空 handle。

异常在协程体内进入 `promise_type::unhandled_exception()`；task 类型应保存并在 await 时重新抛出或转成显式错误。析构阶段不得让异常逃逸。持有 mutex 跨越 `co_await` 往往危险，因为暂停时间无界且恢复线程不确定。

工程实践优先使用已有异步库的 task、executor、timer 和 I/O awaitable，并阅读其 eager/lazy、取消、线程和销毁契约。最小测试覆盖立即完成、真正暂停、异常、取消、调用方提前销毁和服务关闭。

参考：[C++ coroutine language support](https://en.cppreference.com/w/cpp/language/coroutines.html)。
