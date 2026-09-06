---
status: stable
confidence: high
verified: 2026-09-06
review_due: 2026-09-11
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

C++ 对象生命周期由构造、拷贝/移动、赋值与析构共同定义。类只要管理资源，就必须先写清所有权，再决定是否允许拷贝、如何移动以及析构时做什么。现代 C++ 优先使用 RAII 成员类型以遵循 Rule of Zero；只有直接管理资源时才需要 Rule of Five。

# 生命周期模型

```text
construct -> usable object -> copy or move -> assignment -> destruction

construction failure -> completed members are destroyed
moved-from object -> valid for destruction and assignment
```

# Rule of Zero 与 Rule of Five

| 设计 | 适用情况 | 要点 |
| --- | --- | --- |
| Rule of Zero | 成员已用 `vector`、`string`、智能指针等 RAII 类型 | 不自己声明析构、拷贝、移动 |
| Rule of Five | 类直接拥有裸资源、句柄或自定义分配 | 同时审视析构、拷贝构造/赋值、移动构造/赋值 |
| 禁止拷贝、允许移动 | 独占 socket、文件、锁等资源 | 删除 copy，保证 move 后源对象仍有效 |

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只写析构函数，却忘记自定义拷贝，导致浅拷贝与 double free。
- 认为 moved-from 对象不可再用；正确要求是它处于有效但未指定状态。
- 为所有类手写五个函数；这通常比 Rule of Zero 更容易制造 bug。

# 自测

1. 一个独占 `FILE*` 的 wrapper 应该支持拷贝吗？移动后源对象必须满足什么条件？
2. 为什么 `std::vector<T>` 会关心 `T` 的 move constructor 是否 `noexcept`？
3. 哪些成员类型已经让你无需自己写析构函数？

# Object Oriented Programming (面向对象编程)

> [!note] 本节重点：核心考点：封装、继承、多态三大面向对象特性在 C++ 中的实现

# 封装（Encapsulation）

```cpp
class BankAccount {
    double balance_;      // private：外部不可直接访问
    std::string owner_;

public:
    BankAccount(std::string owner, double init)
        : owner_(std::move(owner)), balance_(init) {}

    // 公开接口控制访问方式
    void deposit(double amount) {
        if (amount > 0) balance_ += amount;
    }
    bool withdraw(double amount) {
        if (amount > balance_) return false;
        balance_ -= amount;
        return true;
    }
    double balance() const { return balance_; }  // const 成员函数
};
```

**访问控制：**

|说明符|类内|派生类|外部|
|---|---|---|---|
|`private`|✅|❌|❌|
|`protected`|✅|✅|❌|
|`public`|✅|✅|✅|

`class` 默认 `private`，`struct` 默认 `public`。

---

# 继承（Inheritance）

```cpp
class Animal {
public:
    std::string name;
    Animal(std::string n) : name(std::move(n)) {}
    virtual void speak() const { std::cout << name << " makes a sound\n"; }
    virtual ~Animal() = default;   // 多态基类析构必须是 virtual！
};

class Dog : public Animal {
public:
    Dog(std::string n) : Animal(std::move(n)) {}
    void speak() const override { std::cout << name << " barks\n"; }
};

class Cat : public Animal {
public:
    Cat(std::string n) : Animal(std::move(n)) {}
    void speak() const override { std::cout << name << " meows\n"; }
};
```

**继承方式与访问控制变化：**

|继承方式|基类 public|基类 protected|基类 private|
|---|---|---|---|
|`public`|public|protected|不可访问|
|`protected`|protected|protected|不可访问|
|`private`|private|private|不可访问|

---

# 多态（Polymorphism）

```cpp
// 运行时多态（虚函数）
std::vector<std::unique_ptr<Animal>> zoo;
zoo.push_back(std::make_unique<Dog>("Rex"));
zoo.push_back(std::make_unique<Cat>("Whiskers"));

for (auto& a : zoo)
    a->speak();   // 运行时根据实际类型调用（动态分派）

// 编译期多态（模板，CRTP）
template<typename Derived>
struct Shape {
    double area() const { return static_cast<const Derived*>(this)->areaImpl(); }
};
struct Circle : Shape<Circle> {
    double r;
    double areaImpl() const { return 3.14159 * r * r; }
};
```

---

多态与对象模型详见 → Virtual Function & VTable Layout (虚函数与虚表结构)

---

# Construction and Destruction (构造与析构)

> [!note] 本节重点：核心考点：构造/析构顺序（基类→成员→派生类）、virtual 析构函数的重要性

## 构造顺序

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

## 初始化列表（Member Initializer List）

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

## 委托构造（C++11）

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

拷贝控制与五法则详见 → Copy Control & Rule of 5 (拷贝控制与五法则)

---

# Copy Control and Rule of Five (拷贝控制与五法则)

> [!note] 本节重点：核心考点：Rule of Five（析构/拷贝构造/拷贝赋值/移动构造/移动赋值）、浅拷贝 vs 深拷贝

```cpp
    int*   data_;
    size_t size_;

public:
    // 构造
    Resource(size_t n) : data_(new int[n]()), size_(n) {}

    // 析构
    ~Resource() { delete[] data_; }

    // 拷贝构造（深拷贝）
    Resource(const Resource& other) : size_(other.size_) {
        data_ = new int[size_];
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // 拷贝赋值（copy-and-swap 惯用法，异常安全）
    Resource& operator=(Resource other) {   // 按值传入：触发拷贝或移动
        swap(*this, other);                  // 交换资源
        return *this;
    }                                        // other 析构，释放旧资源

    // 移动构造
    Resource(Resource&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // 移动赋值
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;   size_ = other.size_;
            other.data_ = nullptr; other.size_ = 0;
        }
        return *this;
    }

    friend void swap(Resource& a, Resource& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }
};
```

## 编译器自动生成的规则

```cpp
// = default：显式要求编译器生成默认实现
// = delete ：禁止该操作

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&)            = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&)                 = default;
    NonCopyable& operator=(NonCopyable&&)      = default;
};

// 若声明了析构/拷贝构造/拷贝赋值，编译器不自动生成移动操作
// → 五法则：要么全定义，要么全 default/delete
```

---

构造析构顺序详见 → Constructor & Destructor Order (构造析构顺序)

---

# Operator Overloading (运算符重载)

> [!note] 本节重点：核心考点：运算符重载规则（成员 vs 非成员）、常见运算符重载模式、类型转换运算符

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

拷贝控制与运算符重载常配合使用，详见 → Copy Control & Rule of 5 (拷贝控制与五法则)

# 学习闭环

## 复述

- 不看正文，说明 03-Object Lifetime and Copy Control (对象生命周期与拷贝控制) 的问题、核心机制与边界。

## 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

## 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

# 关联学习

- 导航：[00-Object and Resource Map (对象与资源导航)](/02-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/00-Object%20and%20Resource%20Map%20(对象与资源导航).md)
- 下一步：[04-Polymorphism and Inheritance (多态与继承)](/02-C++%20Backend%20(C++%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)
