---
status: stable
confidence: high
verified: 2026-09-30
review_stage: learn
review_due: 2026-09-30

previous_review_due: 2026-09-15
---

> [!abstract] 阅读方式：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

> [!summary] 核心摘要
>
> 模板在编译期按类型生成代码，兼顾泛型与零运行时分派；优先用约束清晰的小模板，理解推导、特化与实例化边界，避免难读的错误信息。

# Template Basics (模板基础)

> [!note] 本节重点：函数模板、类模板、模板特化、SFINAE 初步

# 函数模板

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

# 类模板

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

# 模板特化

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

# SFINAE 初步（Substitution Failure Is Not An Error）

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

# 模板与编译

```cpp
// 模板定义必须在头文件中（编译器需要看到完整定义才能实例化）
// .h 中声明+定义，或 .h 中声明 + .tpp 中定义再 include

// 显式实例化（减少重复编译）
// 在 .cpp 中：
template class Stack<int>;      // 显式实例化，只在此编译单元生成代码
// 在其他 .cpp 中：
extern template class Stack<int>;  // 告知编译器不要重复实例化
```

# std::type_traits 常用工具（C++11/17）

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


> [!info]- 延伸阅读
> - 下一步：[06-RAII and Custom Allocation (RAII 与自定义分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/06-RAII%20and%20Custom%20Allocation%20(RAII%20与自定义分配).md)
## 模板真正发生了什么

模板不是运行时的“万能函数”，而是编译器生成具体声明或定义的规则。阅读模板代码时，先区分三个阶段：

1. **解析模板（template parsing）**：检查不依赖模板参数的语法与名称。
2. **模板实参推导与约束检查（deduction and constraints）**：从调用点推导 `T`，再判断候选是否可用。
3. **实例化（instantiation）**：为实际使用的参数组合生成具体实体，如 `max<int>`。

这解释了两个常见现象：模板定义通常必须放在头文件中，因为实例化点需要看到完整定义；错误往往直到某个参数组合真正被实例化时才出现。

```cpp
template <class T>
T add(T a, T b) { return a + b; }

auto x = add(1, 2);       // T = int
// auto y = add(1, 2.0);  // 推导冲突：不会先把 int 隐式转换成 double 再推导
auto z = add<double>(1, 2.0); // 显式指定 T 后，1 可转换为 double
```

> [!important]
> 模板实参推导阶段通常不依靠普通隐式转换来“凑出”同一个 `T`；推导完成以后，才检查形参绑定和允许的转换。

### 依赖名称与 `typename`

`T::value_type` 的含义依赖 `T`。解析模板时，编译器无法确定它是类型还是静态成员，因此需要 `typename` 明确告诉编译器“这是类型”。调用依赖对象上的成员模板时，类似地可能需要 `template` 消除歧义。

```cpp
template <class Container>
void print_first(const Container& c) {
    typename Container::const_iterator it = c.begin();
    if (it != c.end()) std::cout << *it << '\n';
}
```

### 实例化位置、ODR 与代码膨胀

- 把模板定义只写在 `.cpp` 中，其他翻译单元通常无法隐式实例化它。
- `extern template` 可抑制某处的隐式实例化，再在一个 `.cpp` 中显式实例化常用类型，降低重复编译成本。
- 多个翻译单元生成的相同模板实例通常由链接器合并，但不能据此忽略 One Definition Rule（ODR）。宏、编译选项或条件编译让“同一模板”拥有不同定义，会产生未定义行为或隐蔽链接问题。
- 每个参数组合都可能生成机器码。模板提供零开销抽象的同时，也可能增加编译时间、二进制体积和指令缓存压力。

```cpp
// header.hpp
template <class T> T square(T x) { return x * x; }
extern template int square<int>(int);

// square.cpp
template int square<int>(int);  // 显式实例化定义
```

工程上先用构建时间、目标文件大小和 profile 证明问题，再决定是否显式实例化、减少参数组合，或把非依赖模板参数的逻辑移到普通函数。

## 设计模板接口

模板的目标不是“接受所有类型”，而是表达一族具有明确语义要求的操作。设计时按以下顺序判断：

- 只需要一个具体类型：优先普通函数。
- 需要一组静态多态类型：使用模板，并用 Concepts 表达要求。
- 需要稳定 ABI 或运行时替换实现：考虑虚函数、type erasure 或函数对象接口。
- 参数组合会爆炸：把稳定逻辑从模板中抽出，缩小实例化表面。

错误信息也是接口质量的一部分。把约束放在靠近声明的位置，比让错误深入几十层实例化栈后才暴露更易维护。

## 最小验证实验

学习本篇后应能独立完成：

1. 用 `-ftime-report`（GCC）或编译器对应报告比较模板前后的编译成本。
2. 用 `nm -C`、`objdump` 或 IDE 符号视图观察 `foo<int>` 与 `foo<double>` 是否生成独立符号。
3. 故意把模板定义移入 `.cpp`，解释为何另一个翻译单元调用时链接失败，再用显式实例化修复。
4. 构造一次“推导冲突”和一次“约束不满足”，分辨两类诊断发生在哪个阶段。

## 参考资料

- [Templates - cppreference](https://en.cppreference.com/w/cpp/language/templates)
- [Template parameters and arguments - cppreference](https://en.cppreference.com/w/cpp/language/template_parameters)
- [Explicit instantiation - cppreference](https://en.cppreference.com/w/cpp/language/class_template#Explicit_instantiation)
