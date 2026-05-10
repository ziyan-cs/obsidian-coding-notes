
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