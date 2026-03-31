#cpp #inheritance #oop #base-class #derived-class

## ⚡ TL;DR（快速决策）

- 继承本质是：**让新类复用旧类已有的成员和接口**
- 一看到这些需求，要优先想到继承：
    - 存在明显的“is-a”关系
    - 多个类共享公共部分
    - 想在保留基础能力的同时扩展新能力
- 继承不是单纯代码复用工具，更重要的是类型关系建模

## 🧩 Core Idea（核心本质）

- 基类（base class）提供公共接口和共性行为
- 派生类（derived class）在此基础上扩展或重写
- 一句话理解：
    - **继承就是“在已有类型基础上继续长出来”。**
- 最常见写法：
    - `class Derived : public Base`
- `public` 继承通常表达真正的“is-a”关系

## 🔧 Usage Patterns（可复用代码模板）

1. 基础继承

```cpp
class Animal {
public:
    void eat() {
        cout << "eat\\n";
    }
};

class Dog : public Animal {
public:
    void bark() {
        cout << "bark\\n";
    }
};
```

1. 派生类对象使用基类成员

```cpp
Dog d;
d.eat();
d.bark();
```

1. 访问控制影响

```cpp
class Base {
protected:
    int x = 1;
};
```

## ⚠️ Pitfalls（高频错误）

- 不是“is-a”关系却硬用继承
- 公有、保护、私有继承语义不清
- 误把继承当万能复用手段，忽略组合
- 基类析构函数该虚析构时没写 `virtual`

## 🚀 Performance / Tips（性能优化）

- 继承核心是结构设计，不是性能优化
- 设计时先问：
    - 是继承更自然，还是组合更自然？
- 若只是“has-a”关系，通常更适合组合而不是继承

## 🧪 Common Scenarios（常见使用场景）

- 图形基类与具体图形
- 动物基类与具体动物
- GUI 控件体系
- 可扩展框架设计

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    void hello() { cout << "base\\n"; }
};

class Derived : public Base {};

int main() {
    Derived d;
    d.hello();
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **继承就是：让派生类在基类已有能力之上继续扩展，并建立类型层次关系。**