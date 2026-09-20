---
study_stage: backlog
---


> [!summary] 核心摘要
>
> C++20 协程让函数暂停并在以后恢复，编译器负责保存所需状态；**谁持有状态、谁恢复、在哪个线程恢复**由返回类型、awaitable 和执行器共同决定。`co_await` 本身不是非阻塞 I/O，也不会自动提供取消。

# 先用一个同步生成器认识模型

函数体出现 `co_await`、`co_yield` 或 `co_return` 时，它是协程。与普通函数不同，返回类型必须提供 `promise_type` 等协议。以下示例只用于教学：单线程、调用者同步执行 `next()`；它没有调度器，也没有异步 I/O。

```cpp
#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <utility>

template<class T>
class Generator {
public:
    struct promise_type {
        std::optional<T> current;
        std::exception_ptr error;

        Generator get_return_object() noexcept {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current.emplace(std::move(value));
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {
            error = std::current_exception();
        }
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle h) noexcept : handle_(h) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept
        : handle_(std::exchange(other.handle_, {})) {}
    Generator& operator=(Generator&&) = delete;
    ~Generator() { if (handle_) handle_.destroy(); }

    bool next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        if (handle_.promise().error)
            std::rethrow_exception(handle_.promise().error);
        return !handle_.done();
    }

    const T& value() const { return *handle_.promise().current; }

private:
    Handle handle_;
};

Generator<int> count_to(int n) {
    for (int i = 0; i < n; ++i) co_yield i;
}

int main() {
    auto numbers = count_to(3);
    while (numbers.next()) {
        std::cout << numbers.value() << ' '; // 0 1 2
    }
}
```

`initial_suspend()` 返回 `suspend_always`，所以调用 `count_to(3)` 只创建协程对象，不运行循环；每次 `next()` 才恢复。`co_yield` 把当前值存入 promise 并暂停。`final_suspend()` 留住已结束的 frame，等待拥有者析构时 `destroy()`；不能对已完成/已销毁的 handle 再 `resume()`。`value()` 只能在 `next()` 返回 `true` 后、下一次 `next()` 前读取；其引用不拥有协程状态。

## frame、promise、handle 各管什么

| 部件 | 职责 | 风险 |
| --- | --- | --- |
| coroutine frame | 保存跨暂停点仍需存在的参数、局部状态和 promise | 泄漏 handle 会泄漏 frame；销毁后引用悬垂 |
| `promise_type` | 定义启动、最终暂停、产值和异常处理 | `unhandled_exception` 策略必须明确 |
| `coroutine_handle` | 无所有权的低层恢复/销毁入口；示例由 `Generator` 独占管理 | 重复销毁、完成后恢复均不允许 |
| awaiter | `await_ready` / `await_suspend` / `await_resume` 决定暂停与恢复行为 | 回调持有悬垂 handle 可造成 UAF |

frame 的存储可经分配函数取得；满足条件时实现也可省略分配。不能写成“协程默认在堆上，`noop_coroutine` 可以优化掉分配”。C++20 标准库提供底层协程设施，但没有开箱即用的通用异步 `task`；C++23 引入 `std::generator`，使用前仍要核对编译器和标准库支持。

# `co_await` 并不自动异步

对一个 awaiter，`co_await` 大致经历：

1. `await_ready()`：已准备好则不暂停。
2. `await_suspend(handle)`：需要暂停时注册后续恢复逻辑；可返回 `void`、`bool` 或另一个 coroutine handle。
3. `await_resume()`：恢复后取得结果或抛出错误。

`await_suspend` 返回另一个 handle 时，语言支持将控制流转给该协程，即**对称转移**；不需要先断言“必须靠库额外实现”。但是执行器、定时器、socket 事件和线程切换策略仍需库实现。一个协程可在不同线程依次恢复；并行执行靠线程/执行器安排，不是 `co_await` 的自动属性。跨线程公布 handle 时要建立正确同步，尤其不能在公布后仍假定 awaiter 对象一定存活。

| 问题 | 线程 | 协程 |
| --- | --- | --- |
| 并行执行 | 多线程可并行 | 由恢复它的线程/执行器决定 |
| 等待 I/O | 同步等待可阻塞线程 | 需非阻塞 I/O 与 awaitable，暂停才不占该线程 |
| 生命周期 | 线程对象与工作函数要汇合/停止 | frame/handle、底层操作与回调要一起管理 |
| 性能 | 有线程栈和调度成本 | frame、调度与库开销；必须在真实负载测量 |

# 用于后端时必须定义的契约

- **所有权**：返回的 task/generator 谁持有？调用方提前放弃后，底层回调是否仍持有 handle？
- **启动与恢复**：`initial_suspend` 决定 eager/lazy；在什么执行器、什么线程恢复？暂停期间不能无条件持有 `mutex`。
- **取消**：C++20 核心协程不内置通用取消。先让底层 I/O 不再回调或能安全完成，再销毁可能被引用的 frame。
- **异常**：协程内未处理异常走 `promise_type::unhandled_exception()`；可像上例保存 `exception_ptr`，在调用方 `next()` 时重抛。生产 task 应定义统一错误传播协议。
- **借用数据**：协程参数若按引用传入，暂停后引用对象可能已销毁；和普通同步函数不同，必须把生命周期延伸到最后一次恢复或复制/转移所有权。

验证一个异步封装至少覆盖立即完成、实际暂停后恢复、异常、取消、调用方提前销毁、服务停止时仍有待完成 I/O 六种路径。协程解决的是控制流表达；是否比线程模型更合适，要连同库、负载、调试与维护成本测量。

参考：[C++ 标准草案：await 表达式](https://eel.is/c++draft/expr.await)、[协程定义及参数生命周期](https://eel.is/c++draft/dcl.fct.def.coroutine)。
