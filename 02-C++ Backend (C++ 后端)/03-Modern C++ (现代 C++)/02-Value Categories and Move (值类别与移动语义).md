---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

**核心结论**：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。


# Lvalues and Rvalues (左值与右值)

> [!note] 本节重点：核心考点：左值/右值/将亡值的定义，右值引用的绑定规则

## 值类别

```
expression
  - lvalue: named or persistent object; examples: `x`, `*p`, `a[i]`
  - rvalue: temporary or expiring value
    - prvalue: `42`, temporary `std::string`, postfix `i++`
    - xvalue: `std::move(x)`, a function returning `T&&`
```

```cpp
int x = 10;
int& lref = x;          // OK：左值引用绑定左值
// int& err  = 42;      // 错误：左值引用不能绑定右值（字面量无地址）
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

移动构造与移动赋值详解见 → [Move Constructor & Move Assignment (移动构造与移动赋值)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03b-Move%20Constructor%20&%20Move%20Assignment%20(移动构造与移动赋值).md)

---

# Move Construction and Assignment (移动构造与移动赋值)

> [!note] 本节重点：核心考点：移动构造函数与移动赋值运算符的实现、noexcept 的重要性、资源窃取语义

移动语义允许"偷走"临时对象的资源，而非深拷贝，大幅降低开销。

```cpp
class MyString {
    char* data;
    size_t size;
public:
    // 拷贝构造：深拷贝，O(n)
    MyString(const MyString& other) : size(other.size) {
        data = new char[size];
        memcpy(data, other.data, size);
    }

    // 移动构造：偷走指针，O(1)
    MyString(MyString&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;   // 将原对象置于合法但未定义状态
        other.size = 0;
    }

    // 移动赋值
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            delete[] data;          // 释放自己的旧资源
            data = other.data;      // 偷走
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~MyString() { delete[] data; }
};
```

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

## noexcept 的重要性

移动构造/赋值**必须标记 `noexcept`**，否则 `std::vector` 等容器在重新分配内存时会退回使用拷贝（无法保证异常安全）：

```cpp
std::vector<MyString> v;
v.push_back(MyString("hello"));
// 若移动构造未标 noexcept：push_back 扩容时用拷贝，性能损失！
```

# 30 秒回答 / 自测

- **30 秒回答**：移动构造/赋值"偷走"被移动对象的资源（指针/句柄）并把其置于可析构状态，把 O(n) 深拷贝降为 O(1)。
- **常见误区**：移动后仍使用被移动对象（合法但状态未定义，通常只允许重新赋值或析构）；移动构造没标 `noexcept`，导致 `std::vector` 扩容退回拷贝。
- **自测**：1) 移动构造后，被移动对象的指针成员应设为什么？ 2) 为什么 `std::vector` 要求移动构造 `noexcept` 才在扩容时用它？

---

返回值优化与移动语义详见 → [move & RVO (移动语义与返回值优化)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03c-move%20&%20RVO%20(移动语义与返回值优化).md)

---

# std move and RVO (移动语义与返回值优化)

> [!note] 本节重点：核心考点：std::move 的本质（右值引用转换）、RVO/NRVO 编译器优化、返回值优化触发条件

## std::move

`std::move` **不移动任何东西**，只是将左值强制转换为右值引用（`static_cast<T&&>`），告知编译器"这个对象可以被移走"：

```cpp
std::string a = "hello";
std::string b = std::move(a);   // a 的资源被移走，a 处于合法但空状态
// a 仍然可以析构、赋值，但不应该继续使用其内容
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
std::cout << a;   // ❌ a 处于未定义内容状态，不应使用
```

---

## RVO & NRVO（返回值优化）

编译器直接在调用方的栈上构造返回值，完全消除拷贝/移动：

```cpp
// RVO（Return Value Optimization）：返回无名临时对象
std::string foo() {
    return std::string("hello");   // 编译器直接在调用方内存构造，零拷贝
}

// NRVO（Named RVO）：返回具名局部变量
std::string bar() {
    std::string s = "hello";
    s += " world";
    return s;   // 编译器优化：直接在调用方内存构造 s（C++17 前是优化，后是强制）
}
```

**C++17 起，RVO（针对纯右值）是强制行为，不再是可选优化。** NRVO 仍是可选优化。

```cpp
// 正确做法：直接 return 局部变量，信任编译器 NRVO
// 不要 return std::move(s)，那样会禁用 NRVO，反而可能多一次移动
```

---

移动构造与移动赋值详见 → [Move Constructor & Move Assignment (移动构造与移动赋值)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03b-Move%20Constructor%20&%20Move%20Assignment%20(移动构造与移动赋值).md)



# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Value Categories and Move (值类别与移动语义)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
