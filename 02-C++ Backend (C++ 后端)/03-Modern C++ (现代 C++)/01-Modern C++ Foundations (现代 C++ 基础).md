---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Modern C++ Overview (现代 C++ 总览)

> [!note] 本节重点：核心考点：> C++11/14/17/20 关键特性一览、现代 C++ 的核心设计理念

|特性|标准|核心价值|
|---|---|---|
|auto / decltype|C++11|减少冗余类型声明|
|Lambda|C++11/14|就地定义可调用对象|
|右值引用 / 移动语义|C++11|消除不必要的深拷贝|
|unique_ptr / shared_ptr|C++11|自动内存管理，消除裸 new/delete|
|完美转发|C++11|泛型代码中保持值类别|
|constexpr|C++11/14/17|将计算移到编译期|
|if constexpr|C++17|编译期条件分支，替代 SFINAE|
|std::optional|C++17|表达"可能无值"，替代哨兵值|
|std::variant|C++17|类型安全联合体，替代 union|
|string_view|C++17|零拷贝字符串视图|
|Structured Bindings|C++17|解包 pair/tuple/struct|
|Concepts|C++20|约束模板参数类型，替代 SFINAE|
|Ranges|C++20|可组合的惰性序列操作|
|Coroutines|C++20|协程支持，异步编程|

---

类型推导详见 → [Type Deduction (类型推导)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Type%20Deduction%20(类型推导).md)

---

# Type Deduction (类型推导)

> [!note] 本节重点：核心考点：auto 的推导规则、decltype 与 auto 的区别、trailing return type

# auto

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

## auto 的推导规则（重要）

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

# decltype

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

完美转发与类型推导紧密相关，详见 → [Perfect Forwarding & Universal Reference (完美转发)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/05-Perfect%20Forwarding%20&%20Universal%20Reference%20(完美转发)%20⭐.md)

---

# Lambda and Function Objects (Lambda 与函数对象)

> [!note] 本节重点：核心考点：捕获方式、泛型 lambda、std::function 的开销

> [!warning] 捕获列表就是生命周期契约
> 值捕获复制状态，引用捕获依赖外部对象仍然存活。把 lambda 存起来、异步执行或作为回调传出时，默认引用捕获尤其容易产生悬空引用。

# Lambda 基本语法

```cpp
[捕获列表](参数列表) mutable -> 返回类型 { 函数体 }

auto add = [](int a, int b) -> int { return a + b; };
auto greet = [] { std::cout << "hello\n"; };  // 无参数时参数列表可省略
```

# 捕获方式

```cpp
int x = 10, y = 20;

// 值捕获（拷贝，lambda 内的 x 与外部无关）
auto f1 = [x]() { return x; };

// 引用捕获（lambda 持有引用，需注意悬空）
auto f2 = [&x]() { x++; };

// 混合捕获
auto f3 = [x, &y]() { return x + y; };

// 默认值捕获（捕获所有用到的局部变量的副本）
auto f4 = [=]() { return x + y; };

// 默认引用捕获
auto f5 = [&]() { x++; y++; };

// 混合默认：引用捕获 x，值捕获其余
auto f6 = [&, y]() { x++; return y; };

// 捕获 this（在成员函数中使用）
struct Foo {
    int val = 42;
    auto getF() {
        return [this]() { return val; };     // 捕获 this 指针
        // C++17 推荐：[*this]() { return val; };  // 捕获 this 的副本，更安全
    }
};
```

# mutable：修改值捕获的副本

```cpp
int x = 0;
auto f = [x]() mutable { x++; return x; };  // 不加 mutable，值捕获不可修改
f();  // 返回 1，但外部 x 仍为 0
```


# 泛型 Lambda（C++14）

```cpp
// auto 参数，等价于模板函数
auto print = [](auto x) { std::cout << x << '\n'; };
print(42);
print("hello");
print(3.14);

// 多参数
auto max_val = [](auto a, auto b) { return a > b ? a : b; };
```

# Lambda 作为排序谓词

```cpp
std::vector<std::pair<int,int>> v = {{3,1},{1,2},{2,3}};

// 按第二个元素降序
std::sort(v.begin(), v.end(), [](const auto& a, const auto& b){
    return a.second > b.second;
});

// 捕获外部变量作为比较依据
int pivot = 5;
auto less_than = [pivot](int x) { return x < pivot; };
```

---

# std::function

类型擦除的通用函数包装器，可存储任何可调用对象（函数、lambda、函数对象）：

```cpp
#include <functional>

std::function<int(int, int)> f;

f = [](int a, int b) { return a + b; };   // 存 lambda
f = std::plus<int>{};                      // 存函数对象
f = add;                                   // 存函数指针

// 回调/策略模式
void process(std::vector<int>& v, std::function<bool(int)> pred) {
    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}
process(nums, [](int x){ return x % 2 == 0; });  // 删除偶数
```

## std::function 的代价

`std::function` 使用**类型擦除**保存不同种类的可调用对象。实现细节不由标准规定，但相较模板参数通常可能带来以下成本：

- 间接调用，调用点通常不易内联
- 某些实现或较大捕获对象可能发生额外分配
- 性能敏感且类型可在编译期确定时，可优先传模板参数

```cpp
// 性能敏感时：模板参数（编译期确定类型，可内联）
template<typename F>
void process(std::vector<int>& v, F pred) {
    v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
}
```

---

完美转发与 Lambda 表达式常配合使用，详见 → [Perfect Forwarding & Universal Reference (完美转发)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/05-Perfect%20Forwarding%20&%20Universal%20Reference%20(完美转发)%20⭐.md)

# 30 秒回答

lambda 是编译器生成的闭包对象；捕获方式决定它保存副本还是引用。短期算法谓词常用无捕获或值捕获，跨作用域/异步回调要把对象生命周期说清。`std::function` 适合需要统一回调类型的运行时接口；若类型可见且处于热点路径，模板参数更容易优化。

# 自测

1. 为什么 `[&]` 返回的 lambda 可能在调用者处悬空？
2. `[this]` 与 `[*this]` 的资源与生命周期语义有什么差异？
3. 什么时候 API 应接收 `std::function`，什么时候用模板参数？



# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Modern C++ Foundations (现代 C++ 基础)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
