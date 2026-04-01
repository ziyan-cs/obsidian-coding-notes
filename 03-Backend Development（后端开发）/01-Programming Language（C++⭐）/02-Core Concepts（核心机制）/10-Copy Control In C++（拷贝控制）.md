#cpp #copy-control #copy-constructor #assignment #rule-of-five

## ⚡ TL;DR（快速决策）

- 拷贝控制本质是：**定义对象在复制、赋值、移动、销毁时的行为**
- 一看到这些场景，要优先想到拷贝控制：
    - 类中管理动态资源
    - 对象复制后不能共享同一块裸资源
    - 需要自定义拷贝 / 移动语义
- 经典关注点：
    - 拷贝构造
    - 拷贝赋值
    - 析构
    - 移动构造
    - 移动赋值

## 🧩 Core Idea（核心本质）

- 默认生成的拷贝行为有时只做“浅拷贝”
- 对于资源类，这往往不够安全
- 一句话理解：
    - **拷贝控制就是管理对象“复制以后到底该共享还是独立”。**
- 经典经验：
    - Rule of Three
    - Rule of Five
    - Rule of Zero

## 🔧 Usage Patterns（可复用代码模板）

1. 拷贝构造

```cpp
class A {
public:
    A(const A& other) {
        // 拷贝 other 的状态
    }
};
```

1. 拷贝赋值

```cpp
A& operator=(const A& other) {
    if (this == &other) return *this;
    // 释放旧资源，拷贝新资源
    return *this;
}
```

1. 删除拷贝

```cpp
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
```

1. 资源类思维

```cpp
// 若类自己管理裸资源，要认真考虑拷贝 / 移动 / 析构
```

## ⚠️ Pitfalls（高频错误）

- 默认浅拷贝导致双重释放
- 拷贝赋值忘记处理自赋值
- 写了析构却没同步考虑拷贝控制
- 不清楚什么时候应禁止拷贝

## 🚀 Performance / Tips（性能优化）

- 资源类要认真理解 Rule of Five
- 普通值类型若不直接管理资源，尽量遵循 Rule of Zero
- 现代 C++ 更推荐让标准库类型帮你管理资源

## 🧪 Common Scenarios（常见使用场景）

- 自定义资源类
- 智能封装句柄类
- 禁止对象复制
- 明确对象所有权语义

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class A {
public:
    int x;
    A(int v = 0) : x(v) {}
    A(const A& other) : x(other.x) {}
};

int main() {
    A a(5);
    A b = a;
    cout << b.x << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **拷贝控制就是：规定对象在复制、赋值、移动和销毁时应该如何正确处理资源与状态。**