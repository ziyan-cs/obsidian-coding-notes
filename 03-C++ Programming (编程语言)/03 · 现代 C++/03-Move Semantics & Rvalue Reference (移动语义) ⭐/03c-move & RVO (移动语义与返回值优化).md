---
tags:
  - cpp/modern
status: 🌱
---

> [!important] **核心考点**：std::move 的本质（右值引用转换）、RVO/NRVO 编译器优化、返回值优化触发条件

### std::move

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

### RVO & NRVO（返回值优化）

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

移动构造与移动赋值详见 → [Move Constructor & Move Assignment (移动构造与移动赋值)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03b-Move%20Constructor%20&%20Move%20Assignment%20(移动构造与移动赋值).md)
