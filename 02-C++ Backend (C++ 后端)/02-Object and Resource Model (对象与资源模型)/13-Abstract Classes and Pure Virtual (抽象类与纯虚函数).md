---
tags:
  - cpp/core
status: 🌱
---

# 13-Abstract Classes and Pure Virtual (抽象类与纯虚函数)

> [!abstract] 核心考点：纯虚函数与抽象类、接口设计、无法实例化的原因（VTable 不完整）

```cpp
class Shape {
public:
    // 纯虚函数：= 0，子类必须实现
    virtual double area()      const = 0;
    virtual double perimeter() const = 0;
    virtual void   draw()      const = 0;

    // 非纯虚：提供默认实现，子类可选择覆盖
    virtual std::string describe() const {
        return "Shape with area=" + std::to_string(area());
    }

    virtual ~Shape() = default;
};
// Shape 是抽象类：不能直接实例化 Shape s; （编译错误）

class Circle : public Shape {
    double r_;
public:
    Circle(double r) : r_(r) {}
    double area()      const override { return 3.14159 * r_ * r_; }
    double perimeter() const override { return 2 * 3.14159 * r_; }
    void   draw()      const override { std::cout << "drawing circle r=" << r_ << '\n'; }
};

// 纯虚函数也可以有实现（但仍是抽象类，必须在类外定义）
void Shape::draw() const { std::cout << "default draw\n"; }
// 子类可通过 Shape::draw() 显式调用
```

---

虚函数表与多态机制详见 → [Virtual Function & VTable Layout (虚函数与虚表结构)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md)
