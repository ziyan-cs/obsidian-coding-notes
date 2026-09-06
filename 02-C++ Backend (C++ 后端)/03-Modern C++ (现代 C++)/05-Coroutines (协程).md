---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

**核心结论**：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。


# Coroutines Overview (协程入门)

> [!note] 本节重点：核心考点：C++20 协程是可暂停/恢复的函数、基于栈的无栈协程、与异步操作的天然结合

# 什么是协程

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

**C++20 协程是无栈的**：不分配线程栈，暂停时把状态保存到堆上分配的对象中。暂停/恢复的开销远小于线程切换。

# 三个核心概念

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

# Generator（生成器）模式

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

# Task（异步任务）模式

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
| 开销 | 极低（堆上对象） | 高（线程栈 + 系统调用） | 低 |
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

# 工程注意事项

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

协程是 C++20 的核心新特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)



# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **05-Coroutines (协程)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
