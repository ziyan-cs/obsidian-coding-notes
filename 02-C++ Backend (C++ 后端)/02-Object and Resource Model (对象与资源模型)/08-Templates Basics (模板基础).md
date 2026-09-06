---
tags:
  - cpp/core
status: 🌱
---

# Templates Basics — 模板基础

> [!abstract] 核心考点：函数模板、类模板、模板特化、SFINAE 初步

## 函数模板

```cpp
// 定义：T 是类型参数
template<typename T>
T max_val(T a, T b) { return a > b ? a : b; }

// 调用：编译器自动推导
max_val(1, 2);            // T = int
max_val(1.0, 2.0);        // T = double
max_val<std::string>("a", "b");  // 显式指定

// 多个类型参数
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) { return a + b; }

// 非类型模板参数
template<typename T, int N>
struct Array {
    T data[N];
    int size() const { return N; }
};
Array<int, 10> arr;
```

## 类模板

```cpp
template<typename T>
class Stack {
    std::vector<T> data_;
public:
    void push(const T& val) { data_.push_back(val); }
    void push(T&& val)      { data_.push_back(std::move(val)); }
    T&   top()              { return data_.back(); }
    void pop()              { data_.pop_back(); }
    bool empty() const      { return data_.empty(); }
    size_t size() const     { return data_.size(); }
};

Stack<int>         si;
Stack<std::string> ss;
```

## 模板特化

```cpp
// 主模板
template<typename T>
struct TypeName { static const char* name() { return "unknown"; } };

// 全特化（针对具体类型）
template<>
struct TypeName<int>    { static const char* name() { return "int"; } };
template<>
struct TypeName<double> { static const char* name() { return "double"; } };

// 偏特化（针对指针类型）
template<typename T>
struct TypeName<T*> { static const char* name() { return "pointer"; } };

// 使用
TypeName<int>::name();      // "int"
TypeName<float>::name();    // "unknown"
TypeName<int*>::name();     // "pointer"
```

## SFINAE 初步（Substitution Failure Is Not An Error）

```cpp
// 用 std::enable_if 约束模板（仅接受整型）
template<typename T,
         typename = std::enable_if_t<std::is_integral_v<T>>>
T double_val(T x) { return x * 2; }

double_val(5);     // OK
// double_val(3.14); // 编译错误：替换失败，不生成该重载

// C++20 Concepts（更清晰的约束）
template<std::integral T>
T double_val(T x) { return x * 2; }
```

## 模板与编译

```cpp
// 模板定义必须在头文件中（编译器需要看到完整定义才能实例化）
// .h 中声明+定义，或 .h 中声明 + .tpp 中定义再 include

// 显式实例化（减少重复编译）
// 在 .cpp 中：
template class Stack<int>;      // 显式实例化，只在此编译单元生成代码
// 在其他 .cpp 中：
extern template class Stack<int>;  // 告知编译器不要重复实例化
```

## std::type_traits 常用工具（C++11/17）

```cpp
#include <type_traits>

std::is_integral_v<int>        // true
std::is_floating_point_v<int>  // false
std::is_pointer_v<int*>        // true
std::is_same_v<int, int>       // true
std::is_base_of_v<Base, Derived> // true
std::is_copy_constructible_v<T>
std::is_trivially_copyable_v<T>  // 可安全 memcpy

// 类型变换
std::remove_const_t<const int>   // int
std::remove_reference_t<int&>    // int
std::decay_t<const int&>         // int（同时去 const 和引用）
std::add_pointer_t<int>          // int*
std::conditional_t<true, int, double>  // int
```

---

类型别名与模板紧密相关，详见 → [Const, Typedef & Enum (类型系统基础)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)
