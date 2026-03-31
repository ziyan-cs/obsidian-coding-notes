#cpp #constructor #destructor #lifecycle #raii

## ⚡ TL;DR（快速决策）

- 构造与析构本质是：**对象生命周期开始和结束时自动执行的特殊函数**
- 构造函数负责初始化
- 析构函数负责清理资源
- 一看到这些场景，要优先想到它们：
    - 对象创建时必须完成某些准备
    - 对象销毁时必须自动释放资源
    - 想用 RAII 管理资源

## 🧩 Core Idea（核心本质）

- 构造函数名字和类名相同，没有返回值
- 析构函数形如 `~ClassName()`
- 一句话理解：
    - **构造负责“出生配置”，析构负责“离场清理”。**
- 它们是 C++ 对象生命周期管理的核心机制

## 🔧 Usage Patterns（可复用代码模板）

1. 默认构造

```cpp
class A {
public:
    A() {
        cout << "construct\\n";
    }
};
```

1. 带参数构造

```cpp
class Point {
public:
    int x, y;
    Point(int a, int b) : x(a), y(b) {}
};
```

1. 析构函数

```cpp
class A {
public:
    ~A() {
        cout << "destroy\\n";
    }
};
```

1. RAII 示例

```cpp
class FileWrapper {
public:
    FileWrapper() { /* 打开资源 */ }
    ~FileWrapper() { /* 释放资源 */ }
};
```

## ⚠️ Pitfalls（高频错误）

- 在构造函数里“赋值”与初始化列表混淆
- 资源类忘记写合适析构逻辑
- 误以为析构函数需要手动调用
- 多态基类析构函数忘记加 `virtual`

## 🚀 Performance / Tips（性能优化）

- 初始化列表通常比先默认构造再赋值更自然、更高效
- RAII 是现代 C++ 资源管理核心
- 构造 / 析构不只是语法点，而是理解生命周期的基础

## 🧪 Common Scenarios（常见使用场景）

- 对象初始化
- 文件 / 锁 / 内存等资源管理
- 封装对象创建和销毁行为
- 容器中对象的自动生命周期管理

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class A {
public:
    A() { cout << "construct\\n"; }
    ~A() { cout << "destroy\\n"; }
};

int main() {
    A a;
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **构造与析构就是：对象创建时自动初始化、销毁时自动清理资源的生命周期机制。**