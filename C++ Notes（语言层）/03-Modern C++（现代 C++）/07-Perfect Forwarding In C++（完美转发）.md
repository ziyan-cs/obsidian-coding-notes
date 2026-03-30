#cpp #modern-cpp #perfect-forwarding #forward #template

## ⚡ TL;DR（快速决策）

- 完美转发本质是：**把参数原本的左值 / 右值属性尽量原样转交给下一个函数**
- 关键组合：
    - `T&&`（转发引用）
    - `std::forward<T>(x)`
- 一看到这些场景，要优先想到完美转发：
    - 模板包装函数
    - 工厂函数
    - 中间层想无损传递参数
- 它的目标不是“更短”，而是“别把参数性质传丢”

## 🧩 Core Idea（核心本质）

- 普通传参、普通 `std::move` 都可能改变值类别
- 完美转发希望保持调用者传入时的原始属性
- 一句话理解：
    - **左值进来还是左值，右值进来还是右值。**
- 常见写法：
    - `template <class T> void f(T&& x) { g(std::forward<T>(x)); }`

## 🔧 Usage Patterns（可复用代码模板）

1. 基础模板

```cpp
#include <iostream>
#include <utility>
using namespace std;

void g(int&) { cout << "lvalue\\n"; }
void g(int&&) { cout << "rvalue\\n"; }

template <class T>
void f(T&& x) {
    g(std::forward<T>(x));
}
```

1. 工厂函数思路

```cpp
template <class T, class... Args>
auto makeObj(Args&&... args) {
    return T(std::forward<Args>(args)...);
}
```

## ⚠️ Pitfalls（高频错误）

- `T&&` 不一定总是右值引用，在模板里可能是转发引用
- 完美转发和单纯 `std::move` 不是一回事
- `std::forward<T>(x)` 必须和模板参数 `T` 配套使用
- 初学者最容易把右值引用、移动、完美转发混成一团

## 🚀 Performance / Tips（性能优化）

- 主要用于泛型库和中间层封装
- 普通业务逻辑里不一定需要频繁手写
- 但理解它有助于彻底搞懂现代 C++ 参数传递体系

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <utility>
using namespace std;

void g(int&) { cout << "L\\n"; }
void g(int&&) { cout << "R\\n"; }

template <class T>
void f(T&& x) {
    g(std::forward<T>(x));
}

int main() {
    int a = 1;
    f(a);
    f(2);
}
```

## 📌 One-liner Summary（一句话总结）

- **完美转发就是：在模板封装中尽量原样保留参数的值类别并继续传递。**