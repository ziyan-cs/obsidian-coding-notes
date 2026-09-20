---
study_stage: backlog
---

> [!abstract] 学习目标
> 能写出最小、可测试的模板约束；区分“表达式可编译”和“算法语义正确”；遇到多个候选时能解释约束如何参与重载决议。

# 从模板报错到接口契约

泛型函数常依赖类型的某些能力。与其让实现深处才报错，不如在函数入口说明要求。C++20 `concept` 是返回布尔值的编译期约束；`requires` 表达式用于检查语法和返回类型关系，不会真的执行其中的业务表达式。

```cpp
#include <concepts>

template<class T>
concept Integer = std::integral<T>;

template<Integer T>
constexpr T twice(T value) {
    return value + value;
}

static_assert(twice(3) == 6);
// twice(3.0); // 不满足 Integer，故意拒绝
```

这里把非法调用留在**注释**中，正常示例才能编译。`std::integral` 是标准概念，包含 `bool` 和字符等整数类型；如果业务只接受“计数”，还应约束正值、范围或另写领域接口，不能把“整数类型”误认为“有效业务值”。

## 四种 requirement：只约束真正用到的能力

```cpp
#include <concepts>
#include <cstddef>
#include <vector>

template<class T>
concept IndexReadable = requires(const T& object) {
    typename T::value_type; // type：嵌套类型存在
    object.size();           // simple：表达式可形成
    { object[0] } -> std::convertible_to<typename T::value_type>;
                             // compound：表达式及结果类型关系
    requires sizeof(typename T::value_type) >= 1;
                             // nested：额外的布尔约束
};

static_assert(IndexReadable<std::vector<int>>);
static_assert(!IndexReadable<int>);
```

`object[0]` 在 `requires` 中只是**未求值的表达式**；它检查索引操作是否可用，不检查实际容器非空。若实现要访问首元素，运行时仍必须检查 `empty()`。类型约束写得越强，接受的合法类型越少；例如只需要迭代时不应强行要求随机访问。

复合 requirement 中的 `-> std::convertible_to<U>` 约束的是表达式的 `decltype((expr))`；它不是“返回类型必须字面上等于 U”。若确实要求完全相同，使用 `std::same_as<U>`，并注意引用、`const` 会影响结果。

# 约束怎样影响重载

约束满足只决定候选是否可用，最终选择还会经过普通重载决议。两个候选同时可用时，约束偏序可能使更具体的候选胜出：

```cpp
#include <concepts>

template<class T>
concept Number = std::integral<T> || std::floating_point<T>;

constexpr int kind(Number auto) { return 1; }
constexpr int kind(std::integral auto) { return 2; }

static_assert(kind(42) == 2);
static_assert(kind(3.5) == 1);
```

不要简单数 `&&` 条件的数量来预测优先级。编译器依据规范化后的原子约束判断包含关系；把逻辑看起来相同的 `requires` 在两个重载里各写一遍，也可能导致歧义。复用命名 concept，并用正向/负向编译用例验证重载选择。

Concept 满足语法并不等于证明语义。例如 `std::strict_weak_order` 要求比较关系满足严格弱序；编译器不能对任意运行时值证明传递性。把它视为库与调用方共同遵守的契约，而不是自动正确性证明。

# C++17 的 SFINAE：为什么仍要认识

在模板实参替换的**直接上下文**中，某候选的类型或表达式无法形成时，SFINAE 会将该候选移出重载集合，而非立刻报错。它不负责吞掉函数体里的任意编译错误。

```cpp
#include <type_traits>

template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr T increment(T value) {
    return value + 1;
}

static_assert(increment(4) == 5);
```

上面代表传统 `enable_if` 写法；要兼容 C++17 的公共库可能需要它。C++20 新接口通常优先用 concept 表达意图，但迁移时应固定既有重载选择和最低语言标准，不能只凭“错误信息更短”机械替换。

传统 detection idiom 还可以用 `std::void_t` 检测嵌套类型：

```cpp
#include <type_traits>
#include <vector>

template<class T, class = void>
struct has_value_type : std::false_type {};

template<class T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

static_assert(has_value_type<std::vector<int>>::value);
static_assert(!has_value_type<int>::value);
```

## 把知识落到工程上

为新泛型 API 列出实现实际使用的表达式，写最小 concept；再用一种满足类型、一种不满足类型、一个语义边界用例测试。维护旧代码时先理解 SFINAE 是怎样筛选候选，再评估 C++20 迁移是否改变重载、可用标准或二进制接口。`if constexpr` 用于**选定模板后的实现分支**，不是 `requires` 的完全替代品。

参考：[C++ 标准草案：requires 表达式](https://eel.is/c++draft/expr.prim.req)、[约束偏序](https://eel.is/c++draft/temp.constr.order)。
