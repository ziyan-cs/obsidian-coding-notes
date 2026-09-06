---
tags:
  - cpp/modern
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# Lvalue vs Rvalue — 左值右值区分

> [!important] **核心考点**：左值/右值/将亡值的定义，右值引用的绑定规则

### 值类别

```
表达式
├── 左值（lvalue）：有名字、有持久地址，可以取地址
│       变量名、解引用 *p、数组下标 a[i]、前置++
└── 右值（rvalue）：临时对象，即将销毁
        ├── 纯右值（prvalue）：字面量 42、临时对象 std::string("hi")、后置 i++
        └── 将亡值（xvalue）：std::move(x)、函数返回右值引用
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
