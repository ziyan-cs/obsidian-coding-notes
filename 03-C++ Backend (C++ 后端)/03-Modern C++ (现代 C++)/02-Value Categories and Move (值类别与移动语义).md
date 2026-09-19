---
status: stable
confidence: high
content_verified: 2026-09-19
verified: 2026-10-08
review_stage: learn
review_due: 2026-10-08
previous_review_due: 2026-09-18
---

> [!abstract] 学习目标：区分表达式值类别、移动构造与复制消除；能说明移动后对象的有效边界和 `noexcept` 的真实作用。

> [!summary] 核心摘要
>
> 值类别参与引用绑定和重载决议；`std::move` 只转换表达式类别，真正移动由目标类型的构造或赋值操作完成。标准库对象移动后有效但值不指定。

# Lvalues and Rvalues (左值与右值)

> [!note] 本节重点：左值/右值/将亡值的定义，右值引用的绑定规则

## 值类别

```
expression
  - lvalue: 可标识具体对象或函数的表达式；例：`x`、`*p`、`a[i]`
  - rvalue: temporary or expiring value
    - prvalue: `42`, temporary `std::string`, postfix `i++`
    - xvalue: `std::move(x)`, a function returning `T&&`
```

```cpp
int x = 10;
int& lref = x;          // OK：左值引用绑定左值
// int& err  = 42;      // 错误：非常量左值引用不能绑定该纯右值
const int& cref = 42;   // OK：const 左值引用可以绑定右值（延长生命周期）
int&& rref = 42;        // OK：右值引用绑定右值
// int&& err2 = x;      // 错误：右值引用不能绑定左值
int&& rref2 = std::move(x);  // OK：move 将左值转为将亡值
```

**关键点：右值引用变量本身是左值**（有名字）：

```cpp
void foo(int&& r) {
    // 在 foo 内部，r 是左值（有名字）
    // 若要继续以右值传递，需再次 std::move(r)
}
```

---

# Move Construction and Assignment (移动构造与移动赋值)

移动语义允许类型把资源从源对象转到目标对象，但 `std::move` 本身只是类型转换；是否调用移动构造取决于目标类型的重载。不要先手写裸指针 owner 练习业务代码：自定义拷贝、移动、析构任何一个环节出错都可能造成双重释放或泄漏。先用标准容器观察真实语义：

```cpp
#include <string>
#include <utility>
#include <vector>

std::vector<std::string> source{"alpha", "beta"};
auto copied = source;             // 两个 vector 各自拥有元素
auto moved = std::move(source);   // 选择 vector 的移动构造
// source 仍有效，但内容状态不指定；可赋新值或调用满足前提的操作。
source = {"new"};
```

对于自己编写的资源类型，优先让 `std::vector`、`std::string`、`std::unique_ptr` 等成员承担所有权，再依靠编译器生成的特殊成员函数（Rule of Zero）。确需手写移动时，必须保证源对象与目标对象在所有路径上都能安全析构，并测试自移动赋值、异常与资源释放。移动的复杂度由具体类型及分配器条件决定，不能统一写成 O(1)。


## 五法则（Rule of Five）

若类需要自定义以下任意一个，通常需要全部自定义：

|特殊函数|声明方式|
|---|---|
|析构函数|`~T()`|
|拷贝构造|`T(const T&)`|
|拷贝赋值|`T& operator=(const T&)`|
|**移动构造**|`T(T&&) noexcept`|
|**移动赋值**|`T& operator=(T&&) noexcept`|

```cpp
// 不需要资源管理时：用 = default 让编译器生成
struct Point {
    double x, y;
    Point(const Point&) = default;
    Point(Point&&)      = default;
    Point& operator=(const Point&) = default;
    Point& operator=(Point&&)      = default;
    ~Point() = default;
};
```

## `noexcept` 与容器异常保证

移动构造**不要求一律标记** `noexcept`。只有实现确实不会抛异常，才应声明 `noexcept`；否则抛出时会触发 `std::terminate`。`std::vector` 扩容时，为维持异常保证，可能在“移动可能抛异常且类型可拷贝”时选择拷贝旧元素；这不是所有容器、所有操作的固定规则，也不是移动赋值的统一要求。

```cpp
#include <type_traits>
#include <utility>

struct Copyable {
    Copyable(const Copyable&);
    Copyable(Copyable&&) noexcept(false);
};
static_assert(std::is_same_v<
    decltype(std::move_if_noexcept(std::declval<Copyable&>())),
    const Copyable&>);
```

问清楚三个问题：类型是否真的可拷贝？移动是否可能抛异常？容器操作承诺怎样的异常保证？更基础的原则仍是 Rule of Zero：让标准库成员管理资源，减少手写特殊成员函数。

---


# std move and RVO (移动语义与返回值优化)

> [!note] 本节重点：std::move 的本质（右值引用转换）、RVO/NRVO 编译器优化、返回值优化触发条件

## std::move

`std::move` **不移动任何东西**，只是将左值强制转换为右值引用（`static_cast<T&&>`），告知编译器"这个对象可以被移走"：

```cpp
std::string a = "hello";
std::string b = std::move(a);   // 选择移动构造；a 仍有效但其值不指定
// 可以重新赋值，也可调用不要求特定值的操作，例如 a.empty()。
```

**不要 move 的场景：**

```cpp
// 错误：move 局部变量会阻止 NRVO
std::string createStr() {
    std::string s = "hello";
    return std::move(s);   // ❌ 阻止编译器优化，改为直接 return s;
}

// 错误：move 后继续使用
std::string a = "hello";
foo(std::move(a));
std::cout << a;   // 合法，但不能依赖它仍为 "hello" 或一定为空
```

---

## RVO & NRVO（返回值优化）

返回对象可直接在结果对象的存储中构造；结果位于哪里由调用上下文和实现决定，不能一律称作“调用方的栈”：

```cpp
// RVO（Return Value Optimization）：返回无名临时对象
std::string foo() {
return std::string("hello");   // C++17 起同类型 prvalue 直接初始化结果对象
}

// NRVO（Named RVO）：返回具名局部变量
std::string bar() {
    std::string s = "hello";
    s += " world";
return s;   // NRVO 可省略复制/移动；即使 C++17 也并非强制
}
```

**C++17 起，同类型纯右值直接初始化结果对象，不再需要先构造临时对象再移动。** 返回具名局部变量的 NRVO 仍是允许但非强制的优化。

```cpp
// 正确做法：直接 return 局部变量，信任编译器 NRVO
// 通常不要 return std::move(s)：它会让此处不再符合 NRVO 条件，并可能多一次移动。
```

---


> [!info]- 延伸阅读
> - 下一步：[03-Smart Pointers (智能指针)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/03-Modern%20C++%20(现代%20C++)/03-Smart%20Pointers%20(智能指针).md)
