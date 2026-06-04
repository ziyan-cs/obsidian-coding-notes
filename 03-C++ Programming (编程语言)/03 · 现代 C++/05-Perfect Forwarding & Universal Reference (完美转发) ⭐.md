---
tags:
  - cpp
  - modern-cpp
---

> **核心考点**：万能引用、引用折叠、std::forward 的作用

## 万能引用（Universal Reference）

`T&&` 出现在**类型推导上下文**中是万能引用，可以绑定左值也可以绑定右值：

```cpp
template<typename T>
void foo(T&& arg);    // T&& 是万能引用，T 由调用方推导

auto&& x = expr;     // auto&& 也是万能引用
```

注意区分：`std::vector<int>&&` 是右值引用（类型已确定，无推导）。

## 引用折叠规则

T 被推导为引用类型时，`T&&` 按以下规则折叠：

|T 推导为|T&& 结果|
|---|---|
|`int`|`int&&`（右值引用）|
|`int&`|`int& &&` → `int&`（左值引用）|
|`int&&`|`int&& &&` → `int&&`（右值引用）|

**记忆：有左值引用就折叠为左值引用（& 优先）。**

```cpp
// 传入左值 → T 推导为 int& → T&& = int& （左值引用）
// 传入右值 → T 推导为 int  → T&& = int&&（右值引用）
int x = 10;
foo(x);           // T = int&,  arg 类型 int&
foo(42);          // T = int,   arg 类型 int&&
foo(std::move(x));// T = int,   arg 类型 int&&
```

## std::forward（完美转发）

在函数内部，参数 `arg` 无论如何都是**左值**（有名字）。`std::forward<T>(arg)` 根据 T 的推导结果，将 arg 恢复为原来的值类别：

```cpp
template<typename T>
void wrapper(T&& arg) {
    // arg 在这里是左值（有名字）
    foo(arg);                      // 永远传左值 ❌
    foo(std::move(arg));           // 永远传右值 ❌
    foo(std::forward<T>(arg));     // 保持原值类别 ✅
    // T=int&  → forward<int&>(arg)  → static_cast<int&>(arg)  → 左值
    // T=int   → forward<int>(arg)   → static_cast<int&&>(arg) → 右值
}
```

## 完美转发的实际应用

```cpp
// make_unique 的简化实现
template<typename T, typename... Args>
std::unique_ptr<T> my_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// emplace 系列函数底层原理
template<typename T>
class MyVector {
    void emplace_back(auto&&... args) {
        new (end_) T(std::forward<decltype(args)>(args)...);
    }
};

// 通用包装函数（日志、计时、缓存装饰器）
template<typename F, typename... Args>
decltype(auto) timed_call(F&& f, Args&&... args) {
    auto t0 = std::chrono::steady_clock::now();
    decltype(auto) result = std::forward<F>(f)(std::forward<Args>(args)...);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()
              << "us\n";
    return result;
}
```

---

## 关联笔记

- [Modern C++ Overview (现代 C++ 特性总览)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)
- [Type Deduction (类型推导)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/01-Type%20Deduction%20(类型推导).md)
- [Lambda & Function Object (Lambda与函数对象)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/02-Lambda%20&%20Function%20Object%20(Lambda与函数对象)%20⭐.md)
- [constexpr & Compile-time Computation (编译期计算)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/06-constexpr%20&%20Compile-time%20Computation%20(编译期计算).md)
- [optional & variant (新类型工具)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/07-optional%20&%20variant%20(新类型工具).md)
