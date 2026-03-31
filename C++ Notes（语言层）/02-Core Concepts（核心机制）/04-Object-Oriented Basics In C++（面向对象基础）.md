#cpp #oop #encapsulation #inheritance #polymorphism

## ⚡ TL;DR（快速决策）

- 面向对象本质是：**用对象来组织状态与行为，用抽象来管理复杂度**
- 核心关键词：
    - 封装
    - 继承
    - 多态
- 一看到这些需求，要优先想到 OOP：
    - 需要对现实对象建模
    - 代码规模变大，需要明确职责
    - 想让接口稳定、实现可扩展
- 面向对象不是“语法堆砌”，而是：**合理组织代码结构的方法**

## 🧩 Core Idea（核心本质）

- 对象 = 数据 + 操作这些数据的函数
- 类 = 对象的模板
- 一句话理解：
    - **把程序拆成一个个各司其职的对象。**
- 三大核心思想：
    - 封装：隐藏实现，暴露接口
    - 继承：表达“is-a”关系
    - 多态：统一接口，不同实现

## 🔧 Usage Patterns（可复用代码模板）

1. 简单封装

```cpp
class Counter {
private:
    int cnt;
public:
    Counter() : cnt(0) {}
    void add() { ++cnt; }
    int value() const { return cnt; }
};
```

1. 对象创建与调用

```cpp
Counter c;
c.add();
cout << c.value() << '\\n';
```

1. 接口与实现分离思维

```cpp
// 对外暴露函数接口
// 内部维护具体状态和细节
```

## ⚠️ Pitfalls（高频错误）

- 把面向对象理解成“必须用 class”
- 类设计职责不清，什么都塞进去
- 封装不足，成员全公开
- 为了炫技而继承，反而增加复杂度

## 🚀 Performance / Tips（性能优化）

- OOP 首先解决的是结构设计问题
- 复杂系统里，清晰的对象边界比微小性能差异更重要
- 小程序不必强行面向对象，大工程则很有价值

## 🧪 Common Scenarios（常见使用场景）

- 用户、订单、图形、设备等对象建模
- 游戏实体系统
- GUI / 工程项目模块化设计
- 大型 C++ 程序架构

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Person {
public:
    void sayHello() const {
        cout << "hello\\n";
    }
};

int main() {
    Person p;
    p.sayHello();
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **面向对象基础就是：通过对象、封装和抽象来组织程序结构。**