#cpp #type-conversion #cast #implicit-conversion #explicit-conversion

## ⚡ TL;DR（快速决策）

- 类型转换本质是：**把一个类型的值按规则转换成另一种类型**
- 一看到这些场景，要优先想到类型转换：
    - `int` 和 `double` 混算
    - 基类 / 派生类指针转换
    - 字符、整数、布尔值之间转换
    - 需要显式 `cast`
- C++ 里类型转换分两类：
    - 隐式转换
    - 显式转换
- 安全性上通常优先清楚、显式的转换写法

## 🧩 Core Idea（核心本质）

- 编译器有时会自动做类型提升或转换
- 有时需要程序员明确写出转换意图
- 一句话理解：
    - **类型转换就是在不同类型表示之间做“翻译”。**
- 常见显式转换：
    - `static_cast`
    - `dynamic_cast`
    - `const_cast`
    - `reinterpret_cast`

## 🔧 Usage Patterns（可复用代码模板）

1. 隐式转换

```cpp
int a = 3;
double b = a;
```

1. 显式数值转换

```cpp
double x = 3.14;
int y = static_cast<int>(x);
```

1. 基类指针转换

```cpp
Base* p = new Derived();
Derived* d = dynamic_cast<Derived*>(p);
```

1. 避免整数除法陷阱

```cpp
double ans = static_cast<double>(a) / b;
```

## ⚠️ Pitfalls（高频错误）

- 忘记整数除法会截断
- 隐式转换导致精度丢失
- 滥用 C 风格强转
- 不清楚 `static_cast` 和 `dynamic_cast` 的适用场景
- `reinterpret_cast` 非常底层，别乱用

## 🚀 Performance / Tips（性能优化）

- 类型转换的重点首先是正确性和安全性
- 数值计算里要特别关注精度和溢出
- 现代 C++ 通常优先使用命名 cast，而不是 C 风格转换

## 🧪 Common Scenarios（常见使用场景）

- 整型与浮点型混算
- 多态类型转换
- 枚举与整数转换
- 函数重载参数匹配

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    double x = 3.9;
    int y = static_cast<int>(x);
    cout << y << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **类型转换就是：按规则把一个类型的值转成另一个类型，并尽量保证语义清楚与安全。**