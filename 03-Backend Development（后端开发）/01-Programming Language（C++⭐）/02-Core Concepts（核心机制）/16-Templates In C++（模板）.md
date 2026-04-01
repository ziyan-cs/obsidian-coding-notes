#cpp #template #generic-programming #compile-time #core-concepts

## ⚡ TL;DR（快速决策）

- 模板本质是：**让同一份代码适用于多种类型**
- 一看到这些需求，要优先想到模板：
    - 逻辑相同，只是类型不同
    - 想写通用函数 / 通用类
    - 不想为 `int`、`double`、`string` 重复写代码
- 模板是 C++ 泛型编程的核心
- STL 的大量能力，背后都离不开模板

## 🧩 Core Idea（核心本质）

- 模板不是运行时“万能类型”，而是编译期生成对应代码
- 常见两类：
    - 函数模板
    - 类模板
- 一句话理解：
    - **模板就是“给类型留参数”。**
- 它让代码从“只处理一种类型”变成“按规则处理很多类型”

## 🔧 Usage Patterns（可复用代码模板）

1. 函数模板

```cpp
template <typename T>
T add(T a, T b) {
    return a + b;
}
```

1. 类模板

```cpp
template <typename T>
class Box {
public:
    T value;
    Box(T v) : value(v) {}
};
```

1. 多模板参数

```cpp
template <typename T, typename U>
auto sum(T a, U b) {
    return a + b;
}
```

1. 模板实例化使用

```cpp
cout << add<int>(2, 3) << '\\n';
Box<string> b("hello");
```

## ⚠️ Pitfalls（高频错误）

- 以为模板是运行时多态
- 模板报错信息通常很长，初学者容易慌
- 类型推导失败时不知道该显式指定类型
- 头文件和模板定义分离不当容易出链接问题

## 🚀 Performance / Tips（性能优化）

- 模板常在编译期展开，通常没有虚函数那类运行时开销
- 泛型代码可读性很重要，不要为了“泛型”把简单代码写得难懂
- 学模板时先掌握函数模板和类模板，再进阶特化、SFINAE、concepts

## 🧪 Common Scenarios（常见使用场景）

- 通用工具函数
- 容器类
- STL 算法与迭代器
- 数值计算泛型接口

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

template <typename T>
T mx(T a, T b) {
    return a > b ? a : b;
}

int main() {
    cout << mx(3, 5) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **模板就是：把类型也当作参数，让同一套代码在多种类型上复用。**