#cpp #abstract-class #pure-virtual #interface #oop

## ⚡ TL;DR（快速决策）

- 抽象类本质是：**不能直接实例化、只用于定义公共接口或抽象行为的基类**
- 一看到这些场景，要优先想到抽象类：
    - 希望只定义接口，不给完整实现
    - 想强制派生类实现某些函数
    - 想统一管理一组行为相似的对象
- 抽象类通常和纯虚函数一起出现

## 🧩 Core Idea（核心本质）

- 只要类中存在纯虚函数，就成为抽象类
- 纯虚函数写法：`virtual void f() = 0;`
- 一句话理解：
    - **抽象类就是“定义规范，但不直接生成对象”的类。**
- 它的作用更像“接口模板”或“抽象基类”

## 🔧 Usage Patterns（可复用代码模板）

1. 纯虚函数

```cpp
class Shape {
public:
    virtual double area() const = 0;
};
```

1. 派生类实现

```cpp
class Circle : public Shape {
public:
    double r;
    Circle(double x) : r(x) {}
    double area() const override {
        return 3.14159 * r * r;
    }
};
```

1. 抽象基类指针

```cpp
Shape* p = new Circle(2.0);
cout << p->area() << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- 试图直接创建抽象类对象
- 派生类没实现全部纯虚函数，结果自身也变抽象类
- 抽象类基类析构函数通常也应设计为虚析构
- 把“抽象类”和“完全没有实现的类”混为一谈

## 🚀 Performance / Tips（性能优化）

- 抽象类重点是接口设计，不是性能技巧
- 它适合大型系统中稳定抽象层的建立
- 小程序未必要上抽象类，但复杂工程里价值很高

## 🧪 Common Scenarios（常见使用场景）

- 图形系统统一接口
- 设备驱动接口
- 插件接口
- 多态框架设计

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    virtual void show() = 0;
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void show() override { cout << "derived\\n"; }
};

int main() {
    Derived d;
    d.show();
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **抽象类就是：用纯虚函数定义公共接口、自己不能直接实例化的基类。**