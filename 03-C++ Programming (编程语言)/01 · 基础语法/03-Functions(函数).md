---
tags:
  - cpp
  - syntax
---

> **核心考点**：函数重载、默认参数、值传递/引用传递、inline 函数

```cpp
// 函数重载（编译期，根据参数类型/数量区分）
int    abs(int x)    { return x < 0 ? -x : x; }
double abs(double x) { return x < 0 ? -x : x; }

// 默认参数（只能在声明中写，且必须从右向左）
void log(std::string msg, int level = 1, bool flush = false);

// 内联函数（建议编译器内联展开，消除函数调用开销）
inline int square(int x) { return x * x; }

// 函数指针
int (*fp)(int, int) = add;   // 指向 add 函数
fp(1, 2);                    // 调用

// 可变参数（C++11 variadic template，取代 C 的 va_list）
template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << '\n';  // 折叠表达式（C++17）
}

// 递归：注意栈溢出，深度过大改用迭代或尾递归
int fib(int n) { return n <= 1 ? n : fib(n-1) + fib(n-2); }
```

---

## 关联笔记

- [Variables, Types & Operators (变量、类型与运算符)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)
- [Control Flow(流程控制)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/02-Control%20Flow(流程控制).md)
- [Array & String(数组与字符串)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/04-Array%20&%20String(数组与字符串).md)
- [IO Basic(标准输入输出)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/05-IO%20Basic(标准输入输出).md)
- [Const, Typedef & Enum (类型系统基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)
