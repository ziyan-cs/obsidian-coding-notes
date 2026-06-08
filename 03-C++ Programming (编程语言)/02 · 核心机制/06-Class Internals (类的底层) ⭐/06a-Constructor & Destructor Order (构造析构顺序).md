---
tags:
  - cpp/core
status: 🌱
---

> **核心考点**：构造/析构顺序（基类→成员→派生类）、virtual 析构函数的重要性

### 构造顺序

```cpp
class Base {
public:
    Base()  { std::cout << "Base ctor\n"; }
    ~Base() { std::cout << "Base dtor\n"; }
};

class Member {
public:
    Member()  { std::cout << "Member ctor\n"; }
    ~Member() { std::cout << "Member dtor\n"; }
};

class Derived : public Base {
    Member m_;
public:
    Derived()  { std::cout << "Derived ctor\n"; }
    ~Derived() { std::cout << "Derived dtor\n"; }
};

// Derived d; 输出顺序：
// Base ctor       ← 1. 基类构造
// Member ctor     ← 2. 成员变量按声明顺序构造
// Derived ctor    ← 3. 派生类构造体
// --- 析构顺序严格相反 ---
// Derived dtor    ← 4. 派生类析构体
// Member dtor     ← 5. 成员变量按声明逆序析构
// Base dtor       ← 6. 基类析构
```

### 初始化列表（Member Initializer List）

```cpp
class Foo {
    const int id_;       // const 成员只能在初始化列表赋值
    std::string name_;
    int& ref_;           // 引用成员只能在初始化列表初始化
public:
    // 初始化列表：按成员声明顺序执行（与列表书写顺序无关！）
    Foo(int id, std::string name, int& r)
        : id_(id), name_(std::move(name)), ref_(r) {}
    //  ↑ 直接构造，不先默认构造再赋值（更高效）
};
```

**初始化列表 vs 构造函数体赋值：**

```cpp
// 低效：name_ 先默认构造（空字符串），再拷贝赋值
Foo::Foo(std::string name) { name_ = name; }

// 高效：直接用 name 构造 name_（一次构造）
Foo::Foo(std::string name) : name_(std::move(name)) {}
```

### 委托构造（C++11）

```cpp
class Circle {
    double x_, y_, r_;
public:
    Circle(double x, double y, double r) : x_(x), y_(y), r_(r) {}
    Circle() : Circle(0, 0, 1) {}           // 委托给主构造函数
    Circle(double r) : Circle(0, 0, r) {}   // 同上
};
```

---

拷贝控制与五法则详见 → [Copy Control & Rule of 5 (拷贝控制与五法则)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/06-Class%20Internals%20(类的底层)%20⭐/06b-Copy%20Control%20&%20Rule%20of%205%20(拷贝控制与五法则).md)
