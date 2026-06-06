---
tags:
  - cpp
  - modern-cpp
---

> **核心考点**：左值/右值/将亡值的定义，右值引用的绑定规则

### 值类别

```
表达式
├── Lvalue（lvalue）：有名字、有持久address，可以取address
│       变量名、解引用 *p、Array下标 a[i]、前置++
└── Rvalue（rvalue）：临时对象，即将销毁
        ├── 纯Rvalue（prvalue）：字面量 42、临时对象 std::string("hi")、后置 i++
        └── 将亡Value（xvalue）：std::move(x)、函数ReturnRvalue引用
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

## 关联笔记

- [Move Constructor & Move Assignment (移动构造与移动赋值)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03b-Move%20Constructor%20&%20Move%20Assignment%20(移动构造与移动赋值).md)
- [move & RVO (移动语义与返回值优化)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03c-move%20&%20RVO%20(移动语义与返回值优化).md)
- [Variables, Types & Operators (变量、类型与运算符)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)
- [Control Flow(流程控制)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/02-Control%20Flow(流程控制).md)
- [Functions(函数)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/03-Functions(函数).md)
