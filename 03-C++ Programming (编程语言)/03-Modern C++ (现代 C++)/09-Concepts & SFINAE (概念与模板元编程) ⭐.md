---
tags:
  - cpp/modern
status: 🌱
---

> [!important] **核心考点**：Concepts (C++20) 约束模板参数、SFINAE 是实现模板重载的传统技法、enable_if 的使用

## Concept 基础（C++20）

```cpp
#include <concepts>

// 定义一个 Concept
template<typename T>
concept Integral = std::is_integral_v<T>;

template<typename T>
concept HasSize = requires(T t) {     // requires 表达式
    { t.size() } -> std::integral;     // 必须有 size() 且返回整数类型
    { t.begin() } -> std::same_as<decltype(t.begin())>;
};

// 使用 Concept 约束模板
template<Integral T>
T add(T a, T b) { return a + b; }

add(1, 2);      // ✅ int 满足 Integral
add(1.0, 2.0);  // ❌ double 不满足 Integral → 编译错误

// 更简洁的写法
void print(const HasSize auto& container) {
    std::cout << "size: " << container.size();
}

// auto 约束
std::integral auto half(std::integral auto x) { return x / 2; }
```

## 常用标准 Concepts

```cpp
// 核心 concepts
std::same_as<T, U>          // T == U
std::derived_from<T, Base>  // T 继承自 Base
std::convertible_to<T, U>   // T 可转换为 U
std::integral<T>             // 整数类型
std::floating_point<T>       // 浮点类型
std::copyable<T>             // 可拷贝
std::movable<T>              // 可移动
std::invocable<F, Args...>   // 可以 F(Args...) 方式调用

// 结合使用
template<typename T>
concept SortableContainer = requires(T c) {
    typename T::value_type;              // 有 value_type
    { c.begin() } -> std::same_as<typename T::iterator>;
    { c.end() }   -> std::same_as<typename T::iterator>;
    requires std::strict_weak_order<decltype(std::less{}), 
                                     typename T::value_type, 
                                     typename T::value_type>;
} && std::regular<T>;
```

## requires 表达式的三种形式

```cpp
// 1. 简单需求：成员存在
concept HasName = requires(T t) {
    t.name;        // T 必须有 name 成员（数据或函数）
};

// 2. 类型需求：类型存在
concept HasValueType = requires {
    typename T::value_type;  // T 必须有嵌套类型 value_type
};

// 3. 复合需求：表达式 + 返回类型
concept Serializable = requires(T t, std::ostream& os) {
    { serialize(t) } -> std::same_as<std::string>;
    { os << t }      -> std::convertible_to<std::ostream&>;
};

// 4. 嵌套需求
concept LargeIntegral = std::integral<T> && requires {
    requires sizeof(T) >= 4;  // 额外的约束
};
```

## SFINAE（Substitution Failure Is Not An Error）

```cpp
// 最基础的 enable_if 用法
template<typename T>
std::enable_if_t<std::is_integral_v<T>, T>
process(T val) { return val + 1; }  // 整数类型版本

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, T>
process(T val) { return val * 2; }  // 浮点类型版本

// 两个模板同时存在，根据 T 的类型选择合适的版本

// 另一种写法（更常见）：
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void func(T val);
```

## 更多 SFINAE 技法

```cpp
// 检测成员是否存在（传统 SFINAE）
template<typename T, typename = void>
struct has_value_type : std::false_type {};

template<typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> 
    : std::true_type {};

// 使用
static_assert(has_value_type<std::vector<int>>::value);  // ✅
static_assert(!has_value_type<int>::value);               // ✅

// decltype + SFINAE
template<typename T>
auto process(const T& t) -> decltype(t.size(), void(), t[0]) {
    return t[0];  // T 有 size() 和 operator[] 时可用
}
```

## Concepts 如何改进 SFINAE

| 方面 | 传统 SFINAE | C++20 Concepts |
|------|------------|----------------|
| 可读性 | 晦涩（多层 enable_if） | 清晰自然 |
| 错误信息 | 几十行模板错误 | 直接指出哪个 concept 不满足 |
| 重载优先级 | 靠 SFINAE 技巧 | `requires` 子句自然排序 |
| 代码量 | 多（需要定义 trait） | 少 |

```cpp
// SFINAE 版（噪音多）
template<typename T>
std::enable_if_t<
    std::is_same_v<decltype(std::declval<T>().size()), typename T::size_type>
    && std::is_same_v<decltype(std::declval<T>().begin()), typename T::iterator>,
    void
> print(const T& c) { /* ... */ }

// Concepts 版（语义清晰）
template<typename T>
concept Container = requires(T t) {
    { t.size() } -> std::same_as<typename T::size_type>;
    { t.begin() } -> std::same_as<typename T::iterator>;
};

void print(const Container auto& c) { /* ... */ }
```

## 工程建议

```cpp
// ✅ 如果你用 C++20，优先用 Concepts 而非 SFINAE
// ✅ Concepts 提供的错误信息对库使用者友好

// ✅ 编写模板库时同时保留 SFINAE 回退（兼容 C++17）
#if __cplusplus >= 202002L
    template<Integral T>
    T half(T x) { return x / 2; }
#else
    template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    T half(T x) { return x / 2; }
#endif

// ❌ 不要过度约束 — Concept 应根据接口需求设计
// ❌ 不用 concepts 定义运行时多态替代品（用虚函数就好）
```

> **面试重点**：Concepts 最常见的应用是约束模板参数类型。常被问到 "SFINAE 是什么"——回答：替换失败不是错误，编译器在模板参数推导时，如果某个特化失败，不会报错而是继续尝试其他重载。

---

Concepts 是 C++20 约束模板参数的重要特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/03-C++%20Programming%20(编程语言)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)
