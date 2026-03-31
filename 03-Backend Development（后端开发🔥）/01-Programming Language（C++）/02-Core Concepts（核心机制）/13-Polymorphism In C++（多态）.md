#cpp #polymorphism #virtual-function #oop #dynamic-dispatch

## ⚡ TL;DR（快速决策）

- 多态本质是：**同一个接口，在不同对象上表现出不同实现**
- 一看到这些需求，要优先想到多态：
    - 希望统一调用方式
    - 不同派生类行为不同
    - 代码面对的是“抽象接口”而不是具体实现
- C++ 里最经典的运行时多态依赖虚函数

## 🧩 Core Idea（核心本质）

- “多态”字面上就是“多种形态”
- 基类指针 / 引用指向派生类对象时，若函数是虚函数，就能动态绑定到实际对象实现
- 一句话理解：
    - **接口统一，行为按真实对象决定。**
- 多态意义：
    - 降低耦合
    - 方便扩展
    - 提升抽象层设计质量

## 🔧 Usage Patterns（可复用代码模板）

1. 基础多态

```cpp
class Animal {
public:
    virtual void speak() {
        cout << "animal\\n";
    }
};

class Dog : public Animal {
public:
    void speak() override {
        cout << "wang\\n";
    }
};
```

1. 基类引用调用

```cpp
Dog d;
Animal& a = d;
a.speak();
```

1. 基类指针调用

```cpp
Animal* p = new Dog();
p->speak();
```

## ⚠️ Pitfalls（高频错误）

- 忘记加 `virtual`，结果不是动态绑定
- 以值传递基类对象，导致对象切片
- 基类析构函数没写成虚函数
- 误把函数重载当多态

## 🚀 Performance / Tips（性能优化）

- 运行时多态有一定动态派发开销，但大多数设计场景里是可接受的
- 当你更看重扩展性和抽象质量时，多态价值很高
- 小而固定的逻辑，不一定非要强行多态

## 🧪 Common Scenarios（常见使用场景）

- 图形绘制系统
- 插件系统
- 游戏对象行为
- 抽象接口编程

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    virtual void show() { cout << "base\\n"; }
};

class Derived : public Base {
public:
    void show() override { cout << "derived\\n"; }
};

int main() {
    Derived d;
    Base* p = &d;
    p->show();
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **多态就是：通过统一接口，在不同对象上表现出不同实现。**