---
status: stable
confidence: high
content_verified: 2026-09-18
---

> [!abstract] 学习目标：理解模板约束如何参与重载选择，并比较 Concepts 与传统 SFINAE 的可读性和诊断。

> [!note] 本节重点：Concepts (C++20) 约束模板参数、SFINAE 是实现模板重载的传统技法、enable_if 的使用

# Concept 基础（C++20）

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

# SFINAE（Substitution Failure Is Not An Error）

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
## 从“替换失败”到“语义约束”

SFINAE 的核心是：在模板实参替换的直接上下文中出现无效类型或表达式时，该候选从重载集合中移除，而不是让整个程序立即报错。它能完成能力探测，但诊断长、组合困难，也不直接表达接口意图。C++20 Concepts 把要求提升为一等语言结构。

```cpp
template <class T>
concept Hashable = requires(T value) {
    { std::hash<T>{}(value) } -> std::convertible_to<std::size_t>;
};

template <Hashable T>
std::size_t hash_value(const T& value) {
    return std::hash<T>{}(value);
}
```

`requires` 表达式检查“表达式是否良构以及结果满足什么要求”，不是运行时 `if`，也不会真的执行其中的表达式。约束在模板实例化前参与候选筛选，因此通常能给出更靠近接口的错误信息。

### 四类 requirement

- **simple requirement**：`value.begin();`，只检查表达式是否有效。
- **type requirement**：`typename T::value_type;`，检查嵌套类型存在。
- **compound requirement**：`{ *it } -> std::convertible_to<T>;`，同时检查有效性、`noexcept` 或返回类型关系。
- **nested requirement**：`requires sizeof(T) >= 8;`，检查另一个约束表达式。

标准 concept 往往包含语义要求。例如 `std::strict_weak_order` 不只是“可以调用并返回布尔值”，调用者还必须满足严格弱序。编译器无法普遍证明这种运行时语义；违反要求仍可能导致算法结果错误。因此，Concepts 是接口契约的一部分，不是完整的程序证明。

## 约束排序与过度约束

当多个重载都可用时，编译器会结合普通重载规则与约束的偏序关系选择更受约束的候选。能否判断“更受约束”依赖约束表达式的规范化与 subsumption（包含关系），不是简单比较条件数量。

```cpp
template <class T>
concept Number = std::integral<T> || std::floating_point<T>;

void describe(Number auto);        // 一般数值
void describe(std::integral auto); // 对整数更具体
```

尽量复用命名 concept，而不是在多个位置手写逻辑相同但语法不同的 `requires` 表达式，否则可能产生意外歧义。约束也不宜比实现真正需要的能力更强：若算法只需单遍读取，就不要要求 `random_access_range`；过度约束会拒绝本可正确工作的类型，削弱复用性。

## 工程迁移策略

维护旧标准代码时，不必机械删除所有 SFINAE：

1. 先用测试固定重载选择和边界行为。
2. 把重复的 detection idiom 提炼成命名 concept。
3. 将 `enable_if_t` 迁移到模板参数约束或尾随 `requires`。
4. 编写负向编译测试，确保不满足要求的类型确实被拒绝。
5. 检查是否改变重载优先级、ABI 暴露面或最低语言标准。

库边界应优先表达最小语义要求；实现内部若只需要局部探测，也可以保留轻量的 `if constexpr` 或 traits。Concepts、traits 和 `if constexpr` 是互补工具，而非互相替代。

## 验证清单

- 能解释“替换失败”和“模板实例化失败”的边界。
- 能写出包含 type、compound 与 nested requirement 的 concept。
- 能说明 `requires` 不执行表达式，也不能自动证明运行时语义。
- 能构造两个重载，验证约束排序；遇到歧义时能缩小到最小示例。
- 能解释为何最小约束比“越强越安全”更适合作为通用库接口。

## 参考资料

- [Constraints and concepts - cppreference](https://en.cppreference.com/w/cpp/language/constraints)
- [requires expression - cppreference](https://en.cppreference.com/w/cpp/language/requires)
- [Standard library concepts - cppreference](https://en.cppreference.com/w/cpp/concepts)
