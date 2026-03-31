#cpp #modern-cpp #auto #type-deduction #decltype

## ⚡ TL;DR（快速决策）

- `auto` 本质是：**让编译器根据初始化表达式自动推导类型**
- 一看到这些场景，要优先想到 `auto`：
    - 迭代器类型太长
    - lambda、模板代码里类型难写
    - 想减少样板代码
- 但 `auto` 不是“无类型”，只是**把类型推导交给编译器**
- 写法舒服，不代表可以乱用；可读性仍然优先

## 🧩 Core Idea（核心本质）

- `auto x = expr;` 会根据 `expr` 推出 `x` 的类型
- 本质上和模板类型推导很像
- 一句话理解：
    - **`auto` 是“自动写出正确类型”的语法糖。**
- 常见配套：
    - `auto`
    - `auto&`
    - `const auto&`

## 🔧 Usage Patterns（可复用代码模板）

1. 基础推导

```cpp
auto x = 10;       // int
auto y = 3.14;     // double
auto s = string("hi");
```

1. 遍历容器

```cpp
vector<int> v = {1, 2, 3};
for (auto x : v) cout << x << ' ';
```

1. 迭代器简化

```cpp
vector<int> v = {1, 2, 3};
auto it = v.begin();
cout << *it << '\\n';
```

1. 引用与 const

```cpp
int a = 5;
auto b = a;          // 拷贝
auto& c = a;         // 引用
const auto& d = a;   // 常量引用
```

## ⚠️ Pitfalls（高频错误）

- `auto` 会丢掉某些引用 / 顶层 `const`
- 想保留引用时要显式写 `auto&`
- `auto` 让代码更短，但不一定更清晰
- 不能只写 `auto x;`，通常必须初始化

## 🚀 Performance / Tips（性能优化）

- `auto` 主要提升可读性与开发效率，不是运行时优化
- 容器遍历常优先 `const auto&`
- 类型非常明显时，直接写真实类型也完全可以

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3};
    for (const auto& x : v) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`auto` 就是：让编译器根据初始化值自动推导变量类型。**