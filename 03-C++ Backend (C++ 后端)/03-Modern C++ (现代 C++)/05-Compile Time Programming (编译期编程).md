---
study_stage: backlog
---

> [!abstract] 学习目标
> 能判断一次调用是否**必须**在常量求值中完成，并区分 `constexpr`、`consteval`、`constinit` 和 `if constexpr` 的职责；不用“编译期更快”代替真实成本分析。

# `constexpr`：允许常量求值，不等于每次都在编译期执行

`const` 只限制对象在该接口上的修改；`constexpr` 变量的初始化必须是常量表达式。`constexpr` 函数可用于常量表达式，也可接受运行期实参而在运行期求值。

```cpp
#include <cassert>

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

static_assert(fibonacci(10) == 55); // 这里必须常量求值

int main(int argc, char**) {
    const int runtime_n = argc;      // 运行期值
    assert(fibonacci(runtime_n) >= 0); // 普通运行期调用
}
```

示例仅用于小的非负输入；大 `n` 可能让 `int` 溢出。`constexpr` 函数能否在某个常量上下文求值，还取决于该次执行是否满足常量表达式规则；不能仅看函数声明。C++11 到 C++20 持续放宽可写的函数体与类型，项目需按最低语言标准编译验证。

## `const`、`constexpr` 与对象

```cpp
#include <array>

constexpr int capacity = 16;
std::array<int, capacity> values{}; // 模板实参需要常量表达式

int runtime_source();
void example() {
    const int snapshot = runtime_source(); // 运行期初始化的 const 对象
    (void)snapshot;
}
```

上例的 `runtime_source()` 仅声明，用于说明 `const` 与常量求值区别；若要运行 `example()`，必须提供该函数定义。静态数组界、非类型模板参数和 `static_assert` 等上下文要求常量表达式，无法在运行期“退回”求值。

# `if constexpr`：模板实例化时选择实现分支

`if constexpr` 根据编译期条件选分支，常用于模板实现差异；未选中分支仍要满足一般语法和非依赖的检查，不能藏任意非法代码。

```cpp
#include <type_traits>

template<class T>
constexpr const char* group() {
    if constexpr (std::is_integral_v<T>)
        return "integral";
    else if constexpr (std::is_floating_point_v<T>)
        return "floating";
    else
        return "other";
}

static_assert(group<int>()[0] == 'i');
static_assert(group<double>()[0] == 'f');
```

这个分支用于**选定模板之后**的实现；对调用方能否实例化 API 的限制，优先在 C++20 使用 `concept` / `requires`，或按旧标准使用 traits、SFINAE。

# `consteval` 与 `constinit`（C++20）

`consteval` 声明立即函数，其受常量求值约束的调用必须产生常量表达式；不能像一般 `constexpr` 函数那样把运行期实参直接交给它。`constinit` 则用于静态或线程存储期变量，要求**静态初始化**，但不使变量成为只读常量。

```cpp
consteval int square(int x) { return x * x; }
constexpr int table_size = square(8);

constinit int startup_counter = table_size; // 静态初始化
int main() {
    startup_counter += 1; // 运行期可修改
    return startup_counter == 65 ? 0 : 1;
}

// square(startup_counter); // 故意编译失败：实参不是常量表达式
```

`constinit` 有助于避免动态初始化顺序问题，但**不解决并发访问**：若多个线程写 `startup_counter`，仍需同步。`std::is_constant_evaluated()` 可查询当前是否处于常量求值上下文，使用前应明确两条路径是否保持相同业务语义。

# 何时值得放到编译期

把不变量、固定小表或类型约束放在编译期，能更早发现错误；代价是编译时间、诊断复杂度与可能的代码膨胀。先用 `static_assert` 固定边界，再分别编译运行期和常量求值调用；对性能收益也要像运行时代码一样测量。不要为展示模板技巧而搬移普通业务流程。

参考：[C++ 标准草案：常量表达式](https://eel.is/c++draft/expr.const)、[立即函数](https://eel.is/c++draft/dcl.constexpr)。
