---
tags:
  - cpp/modern
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# Perfect Forwarding & Universal Reference — 完美转发

> [!important] **核心考点**：万能引用、引用折叠、std::forward 的作用

> [!warning] `std::forward` 只用于保留原始值类别
> 它不是“更快的 `std::move`”。转发同一个对象后仍继续依赖其状态会让调用者难以判断所有权；只有下游确实需要按原值类别重载时才使用。

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
    template<typename... Args>
    void emplace_back(Args&&... args) {
        new (end_) T(std::forward<decltype(args)>(args)...);
    }
};

// 通用包装函数（日志、计时、缓存装饰器）
template<typename F, typename... Args>
decltype(auto) timed_call(F&& f, Args&&... args) {
    auto t0 = std::chrono::steady_clock::now();
    // 为简洁起见，此版本只示意返回非 void 的调用；void 需单独分支处理。
    decltype(auto) result = std::forward<F>(f)(std::forward<Args>(args)...);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()
              << "us\n";
    return result;
}
```

---

类型推导是完美转发的基础，详见 → [Type Deduction (类型推导)](/03-C++%20Programming%20(编程语言)/03-Modern%20C++%20(现代%20C++)/01-Type%20Deduction%20(类型推导).md)

## 30 秒回答

转发引用（常称 universal reference）只在 `T&&` 且 `T` 发生推导时成立。传入左值会让 `T` 推导为左值引用，传入右值会让 `T` 推导为非引用类型；函数参数本身有名字，表达式永远是左值，因此用 `std::forward<T>(arg)` 才能把原始值类别交给下游。

## 自测

1. 为什么 `foo(arg)` 与 `foo(std::move(arg))` 都不是通用 wrapper 的正确默认写法？
2. `std::vector<int>&&` 为什么不是转发引用？
3. `std::forward` 后为什么不能假设对象仍保留原来的值？
