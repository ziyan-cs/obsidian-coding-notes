#cpp #modern-cpp #lambda #callable #function-object

## ⚡ TL;DR（快速决策）

- Lambda 本质是：**可以在代码里临时定义的匿名函数对象**
- 一看到这些场景，要优先想到 lambda：
    - 排序自定义比较器
    - 回调函数
    - 局部小逻辑不想单独写函数
- 基本形式：
    - `[capture](params) -> return_type { ... }`
- 算法题里最常见用法：`sort` 比较器、局部 DFS / check

## 🧩 Core Idea（核心本质）

- Lambda 不是单纯“语法更短的函数”，而是一个可调用对象
- capture 列表决定它如何使用外部变量
- 一句话理解：
    - **lambda = 就地定义的小函数。**
- 常见 capture：
    - `[]`：不捕获
    - `[=]`：值捕获
    - `[&]`：引用捕获
    - `[&x, y]`：混合捕获

## 🔧 Usage Patterns（可复用代码模板）

1. 基础 lambda

```cpp
auto add = [](int a, int b) {
    return a + b;
};
cout << add(2, 3) << '\\n';
```

1. 排序比较器

```cpp
vector<int> v = {3, 1, 2};
sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;
});
```

1. 捕获外部变量

```cpp
int base = 10;
auto f = [base](int x) {
    return x + base;
};
```

1. 引用捕获修改外部变量

```cpp
int cnt = 0;
auto inc = [&]() { ++cnt; };
inc();
```

## ⚠️ Pitfalls（高频错误）

- 搞不清值捕获和引用捕获
- 捕获了局部变量后，生命周期问题要小心
- 写递归 lambda 时通常需要配合 `std::function` 或自传参数技巧
- capture 写太大（如 `[&]`）有时会降低可读性

## 🚀 Performance / Tips（性能优化）

- lambda 非常适合局部逻辑
- 小比较器、小回调优先 lambda
- 需要长期复用、逻辑复杂时，单独写函数更清楚

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {3, 1, 2};
    sort(v.begin(), v.end(), [](int a, int b) { return a < b; });
    for (int x : v) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **Lambda 表达式就是：可以在当前位置临时定义并使用的匿名函数对象。**