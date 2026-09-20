---
study_stage: learn
review_due: 2026-10-05
---


# 现代 C++ 的主线：让类型表达意图

“现代”不是把所有语法换成 `auto`，而是明确**所有权、生命周期、错误状态与泛型契约**。先熟悉 C++17 作为工程基线，再按项目要求学习 C++20 的 concepts、ranges 和 coroutines；新标准功能还受编译器与标准库版本影响。

| 主题 | 先解决什么问题 | 本文件夹的后续专题 |
| --- | --- | --- |
| `auto`、`decltype`、lambda | 减少样板但不丢类型语义 | 本篇 |
| 值类别、移动、转发 | 避免不必要复制，保持所有权边界 | 02、04 |
| `unique_ptr` / `shared_ptr` | 把动态所有权显式化 | 03 |
| `constexpr`、concepts | 编译期不变量与泛型接口 | 05、07 |
| `optional`、`variant`、视图 | 用类型表达缺席、错误和借用 | 06 |
| coroutines | 组织可暂停的控制流 | 08 |

# `auto` 与 `decltype`：推导仍有规则

`auto` 按初始化表达式推导，普通变量推导通常去掉顶层 `const` 和引用；需要借用时写 `auto&` / `const auto&`。它不会神奇地避免拷贝。

```cpp
#include <type_traits>
#include <vector>

int main() {
    int value = 7;
    int& ref = value;
    const int frozen = 9;

    auto copy = ref;              // int，独立副本
    auto& borrowed = ref;         // int&
    auto copied_const = frozen;   // int
    const auto& observed = frozen;// const int&

    static_assert(std::is_same_v<decltype(copy), int>);
    static_assert(std::is_same_v<decltype(borrowed), int&>);
    static_assert(std::is_same_v<decltype(copied_const), int>);
    static_assert(std::is_same_v<decltype(observed), const int&>);

    std::vector<int> items{1, 2, 3};
    for (auto& item : items) item *= 2;
    return items[0] == 2 ? 0 : 1;
}
```

`decltype(name)` 对未加括号的名字取得声明类型；`decltype((expr))` 按表达式值类别推导。因此 `decltype(value)` 是 `int`，`decltype((value))` 是 `int&`。`decltype(auto)` 用在返回类型时能保留引用，但**不能盲目给局部值加括号返回**：

```cpp
#include <type_traits>

int global_value = 3;
decltype(auto) copy_value() { return global_value; }   // int
decltype(auto) borrow_value() { return (global_value); } // int&

static_assert(std::is_same_v<decltype(copy_value()), int>);
static_assert(std::is_same_v<decltype(borrow_value()), int&>);
```

若 `return (local);` 中 `local` 是局部值，推导出的引用会悬垂。推导不替代生命周期设计。

# Lambda：捕获列表是生命周期契约

值捕获在闭包中保存副本；引用捕获依赖原对象继续存活。短期算法谓词用局部引用往往可行，但回调被保存、交给线程或在外层函数返回后执行时，默认 `[&]` 最容易悬垂。

```cpp
#include <algorithm>
#include <vector>

int main() {
    std::vector<int> values{3, 1, 2};
    std::sort(values.begin(), values.end(),
              [](int left, int right) { return left < right; });

    int threshold = 2;
    auto is_large = [threshold](int x) { return x > threshold; };
    return is_large(values.back()) ? 0 : 1;
}
```

成员函数中的 `[this]` 只捕获指针；对象若先销毁，调用闭包会悬垂。C++17 的 `[*this]` 复制当前对象，可用于确实要**快照**且对象可复制的场景，但副本成本与语义须审视：

```cpp
struct Counter {
    int value = 0;
    auto snapshot() const {
        return [*this] { return value; };
    }
};

int main() {
    auto read_snapshot = Counter{7}.snapshot();
    return read_snapshot() == 7 ? 0 : 1;
}
```

引用捕获、`this` 指针、`string_view` 等借用都不能自动延长寿命。跨线程回调还需要同步共享状态；“按值捕获”也不自动让所指对象线程安全。

# `std::function`：运行时统一接口的代价

C++17 的 `std::function<R(Args...)>` 是类型擦除包装器，可存储**可复制**的目标。它适合回调表、运行期可替换策略；如果调用方类型在编译期已知且处于热点路径，模板参数可能更容易内联。间接调用及可能的分配成本都由具体实现与捕获对象决定，不能写成固定纳秒数字。

```cpp
#include <algorithm>
#include <functional>
#include <vector>

void erase_if_match(std::vector<int>& values,
                    const std::function<bool(int)>& predicate) {
    values.erase(std::remove_if(values.begin(), values.end(), predicate),
                 values.end());
}

int main() {
    std::vector<int> values{1, 2, 3, 4};
    erase_if_match(values, [](int x) { return x % 2 == 0; });
    return values == std::vector<int>{1, 3} ? 0 : 1;
}
```

如果目标仅可移动，C++23 提供 `std::move_only_function`；能否使用要核对项目编译器与标准库。选择 API 时先问：需要保存回调吗？需要运行期替换吗？是否必须接受 move-only 捕获？答案决定选模板、`std::function` 还是其他明确的封装。

> [!note] 掌握标志
> 能解释 `auto` 的复制/借用、`decltype(auto)` 的返回引用、lambda 捕获的生命周期，以及 `std::function` 为什么不应无条件放进最热的调用路径。

延伸：[02-Value Categories and Move (值类别与移动语义)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C++%20(现代%20C++)/02-Value%20Categories%20and%20Move%20(值类别与移动语义).md)。


