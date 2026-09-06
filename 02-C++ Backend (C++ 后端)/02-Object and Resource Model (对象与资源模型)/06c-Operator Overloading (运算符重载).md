---
tags:
  - cpp/core
status: 🌱
---

# Operator Overloading — 运算符重载

> [!abstract] 核心考点：运算符重载规则（成员 vs 非成员）、常见运算符重载模式、类型转换运算符

```cpp
public:
    double x, y;
    Vec2(double x=0, double y=0) : x(x), y(y) {}

    // 成员运算符（隐式第一个操作数为 this）
    Vec2  operator+(const Vec2& rhs) const { return {x+rhs.x, y+rhs.y}; }
    Vec2  operator-() const { return {-x, -y}; }           // 一元负号
    Vec2& operator+=(const Vec2& rhs) { x+=rhs.x; y+=rhs.y; return *this; }
    bool  operator==(const Vec2& rhs) const { return x==rhs.x && y==rhs.y; }

    // 下标运算符
    double& operator[](int i) { return i==0 ? x : y; }
    const double& operator[](int i) const { return i==0 ? x : y; }

    // 类型转换运算符
    explicit operator bool() const { return x!=0 || y!=0; }

    // 友元：非成员但需访问私有成员
    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
    // 对称二元运算符也常用友元（支持 scalar * Vec2）
    friend Vec2 operator*(double s, const Vec2& v) { return {s*v.x, s*v.y}; }
};

// 使用
Vec2 a{1,2}, b{3,4};
Vec2 c = a + b;                 // {4, 6}
std::cout << c << '\n';         // (4, 6)
double x = 2.0 * a;            // 友元支持左侧 scalar
```

**不可重载的运算符：** `::` `.` `.*` `?:` `sizeof` `typeid`

---

拷贝控制与运算符重载常配合使用，详见 → [Copy Control & Rule of 5 (拷贝控制与五法则)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/06-Class%20Internals%20(类的底层)%20⭐/06b-Copy%20Control%20&%20Rule%20of%205%20(拷贝控制与五法则).md)
