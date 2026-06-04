---
tags:
  - cpp
  - core-mechanism
---

> **核心考点**：指针与引用的本质区别、函数指针、智能指针底层原理的关系

## 指针详解

```cpp
int  x = 42;
int* p = &x;      // p 存储 x 的地址
*p = 100;         // 解引用，修改 x
p++;              // 指针算术：移动 sizeof(int) 字节

// 空指针
int* null1 = nullptr;   // C++11 推荐（类型安全）
// int* null2 = NULL;   // C 风格，NULL = 0，可能与 int 重载混淆
// int* null3 = 0;      // 同上

// 指针 vs 数组
int arr[] = {1,2,3};
int* p = arr;     // 数组名退化为首元素指针
*(p + 1) == arr[1];      // true，等价
p[2]     == *(p + 2);    // true

// 函数指针
void (*fp)(int) = &myFunc;
(*fp)(42);   // 或直接 fp(42)
```

## 引用详解

```cpp
int x = 42;
int& ref = x;    // 引用：ref 是 x 的别名，必须初始化，不能重新绑定
ref = 100;       // 修改 x

// 引用不是对象，没有地址（通常实现为常量指针，但这是实现细节）
// 不能有引用的引用、引用的指针、引用的数组

// 常量引用：可以绑定临时对象（延长其生命周期）
const int& cr = 42;   // 临时 int 对象生命周期延长至 cr 的作用域

// 右值引用（见移动语义章节）
int&& rr = std::move(x);
```

## 指针 vs 引用

| |指针|引用|
|---|---|---|
|可为空|✅（nullptr）|❌（必须绑定有效对象）|
|可重新指向|✅|❌（一旦绑定不可改）|
|需要解引用|`*p`|直接用|
|可做算术|✅|❌|
|传参惯用法|可为 null 或需要算术|不为 null 且不需要重新绑定|

## 常见内存错误

```cpp
// 1. 悬空指针（Dangling Pointer）
int* p = new int(42);
delete p;
*p = 100;   // 未定义行为！p 已悬空

// 2. 野指针（未初始化）
int* p;     // p 指向随机地址
*p = 1;     // 未定义行为

// 3. 双重释放
delete p;
delete p;   // 未定义行为

// 4. 内存泄漏
int* p = new int(42);
// 忘记 delete p;

// 解决：智能指针
auto p = std::make_unique<int>(42);   // 自动管理生命周期
```

---

## 关联笔记

- [Const%2C Typedef %26 Enum (类型系统基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/01-Const%2C%20Typedef%20%26%20Enum%20(类型系统基础).md)
- [Const, Typedef & Enum (类型系统基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)
- [Type Conversion & Casting (类型转换)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/02-Type%20Conversion%20&%20Casting%20(类型转换).md)
- [OOP Principles (面向对象三大特性)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/05-OOP%20Principles%20(面向对象三大特性).md)
- [Templates Basics (模板基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/08-Templates%20Basics%20(模板基础).md)
