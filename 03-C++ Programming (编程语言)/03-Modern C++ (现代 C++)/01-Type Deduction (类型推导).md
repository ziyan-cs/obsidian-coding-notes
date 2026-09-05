---
tags:
  - cpp/modern
status: 🌱
---


> [!important] **核心考点**：auto 的推导规则、decltype 与 auto 的区别、trailing return type

## auto

编译器根据初始化表达式推导变量类型，消除冗长的类型声明。

```cpp
auto i = 42;                        // int
auto d = 3.14;                      // double
auto s = std::string("hello");      // std::string
auto v = std::vector<int>{1,2,3};   // std::vector<int>

// 迭代器
for (auto it = v.begin(); it != v.end(); ++it) { ... }
for (auto& x : v) x *= 2;   // 引用避免拷贝
for (const auto& x : v) ... // 只读引用
```

### auto 的推导规则（重要）

```cpp
int  x = 10;
int& ref = x;

auto  a = ref;    // a 是 int（auto 丢弃引用！）
auto& b = ref;    // b 是 int&
auto  c = &x;     // c 是 int*

const int cx = 5;
auto  d = cx;     // d 是 int（auto 丢弃顶层 const！）
auto& e = cx;     // e 是 const int&（引用保留 const）
```

**auto 会丢弃：顶层 const、引用。** 需要保留时显式加 `const` 和 `&`。

```cpp
// 函数返回类型推导（C++14）
auto add(int a, int b) { return a + b; }  // 返回 int

// Trailing return type（C++11，用于返回类型依赖参数）
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) { return a + b; }
```

---

## decltype

推导**表达式的类型**，不计算表达式的值，保留引用和 const。

```cpp
int x = 0;
int& ref = x;

decltype(x)    a = 1;    // int
decltype(ref)  b = x;    // int&（保留引用！）
decltype((x))  c = x;    // int&（双括号 = 左值表达式，推导为引用）
decltype(x+1)  d = 0;    // int（表达式结果类型）
```

**`decltype(x)` vs `decltype((x))`：**

- `decltype(x)`：变量名，推导为变量声明类型 `int`
- `decltype((x))`：表达式（左值），推导为 `int&`

```cpp
// decltype(auto)：保留 auto 推导结果的引用和 const（C++14）
int x = 42;
int& ref = x;

auto         a = ref;          // int（丢弃引用）
decltype(auto) b = ref;        // int&（保留引用）

// 常见用途：完美转发返回值
template<typename F, typename... Args>
decltype(auto) call(F&& f, Args&&... args) {
    return std::forward<F>(f)(std::forward<Args>(args)...);
}
```

---

完美转发与类型推导紧密相关，详见 → [Perfect Forwarding & Universal Reference (完美转发)](/03-C++%20Programming%20(编程语言)/03-Modern%20C++%20(现代%20C++)/05-Perfect%20Forwarding%20&%20Universal%20Reference%20(完美转发)%20⭐.md)
