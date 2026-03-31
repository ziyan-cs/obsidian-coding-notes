#cpp #modern-cpp #constexpr #compile-time #constant-expression

## ⚡ TL;DR（快速决策）

- `constexpr` 本质是：**告诉编译器这个值 / 函数尽量在编译期就能确定**
- 一看到这些场景，要优先想到 `constexpr`：
    - 编译期常量
    - 数组大小、模板参数、常量表达式
    - 想让简单函数在编译期求值
- `const` 不等于 `constexpr`
- `constexpr` 更强调：**能不能在编译期算出来**

## 🧩 Core Idea（核心本质）

- `constexpr` 可用于变量、函数、构造等场景
- 它的目标是让某些结果在编译阶段就固定下来
- 一句话理解：
    - **`constexpr` = 编译期可求值。**

## 🔧 Usage Patterns（可复用代码模板）

1. 常量变量

```cpp
constexpr int N = 100;
int a[N];
```

1. `constexpr` 函数

```cpp
constexpr int square(int x) {
    return x * x;
}
```

1. 编译期使用

```cpp
constexpr int x = square(5);
```

## ⚠️ Pitfalls（高频错误）

- `constexpr` 函数不代表任何调用都一定在编译期发生
- 运行时输入参与时，就只能运行时求值
- `const` 和 `constexpr` 含义不同，别混

## 🚀 Performance / Tips（性能优化）

- 主要价值在表达常量语义和启用编译期计算
- 写常量配置、简单数学函数时很常见
- 现代 C++ 中优先用 `constexpr` 表达“编译期常量”意图

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

constexpr int square(int x) { return x * x; }

int main() {
    constexpr int x = square(4);
    cout << x << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`constexpr` 就是：让值或函数具备编译期求值能力的关键字。**