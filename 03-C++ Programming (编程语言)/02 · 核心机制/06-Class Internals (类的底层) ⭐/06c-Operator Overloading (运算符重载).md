
```cpp
class Vec2 {
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