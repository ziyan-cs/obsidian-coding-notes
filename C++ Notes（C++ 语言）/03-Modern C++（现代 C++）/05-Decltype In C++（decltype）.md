#cpp #modern-cpp #decltype #type-deduction #template

## ⚡ TL;DR（快速决策）

- `decltype` 本质是：**根据一个表达式，推导出它的类型**
- 一看到这些场景，要优先想到 `decltype`：
    - 想复用某个表达式的准确类型
    - 模板代码里类型难写
    - 配合 `auto` 做返回值推导
- `auto` 看初始化值，`decltype` 看表达式本身
- 它很强，但对初学者最重要的是先理解常见规则

## 🧩 Core Idea（核心本质）

- `decltype(expr)` 会产生 `expr` 的类型
- 它不会真正计算表达式，只分析类型
- 一句话理解：
    - **`decltype` 是“问编译器这个表达式是什么类型”。**
- 常见配套：
    - `auto`
    - `decltype(auto)`

## 🔧 Usage Patterns（可复用代码模板）

1. 基础用法

```cpp
int x = 10;
decltype(x) y = 20;   // y 是 int
```

1. 配合容器元素

```cpp
vector<int> v = {1, 2, 3};
decltype(v[0]) a = v[1];
```

1. 函数返回类型推导

```cpp
auto add(int a, int b) -> decltype(a + b) {
    return a + b;
}
```

## ⚠️ Pitfalls（高频错误）

- `decltype(x)` 和 `decltype((x))` 结果可能不同
- 表达式是左值时，`decltype` 可能得到引用类型
- 别把 `decltype` 当运行时工具，它完全是编译期概念

## 🚀 Performance / Tips（性能优化）

- 主要价值在泛型编程和复杂类型推导
- 普通简单代码里不一定比直接写类型更清楚
- 和 `auto` 搭配能写出更灵活的现代 C++ 代码

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5;
    decltype(x) y = 10;
    cout << y << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`decltype` 就是：根据表达式规则，让编译器推导出准确类型。**