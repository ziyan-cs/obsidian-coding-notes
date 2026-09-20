---
study_stage: learn
review_due: 2026-10-01
---


> [!summary] 核心摘要
>
> C++ 对象生命周期由构造、拷贝/移动、赋值与析构共同定义。类只要管理资源，就必须先写清所有权，再决定是否允许拷贝、如何移动以及析构时做什么。现代 C++ 优先使用 RAII 成员类型以遵循 Rule of Zero；只有直接管理资源时才需要 Rule of Five。

# 生命周期模型

```text
construct -> usable object -> copy or move -> assignment -> destruction

construction failure -> completed members are destroyed
moved-from object -> valid for destruction and assignment
```

## Rule of Zero 与 Rule of Five

| 设计 | 适用情况 | 要点 |
| --- | --- | --- |
| Rule of Zero | 成员已用 `vector`、`string`、智能指针等 RAII 类型 | 不自己声明析构、拷贝、移动 |
| Rule of Five | 类直接拥有裸资源、句柄或自定义分配 | 同时审视析构、拷贝构造/赋值、移动构造/赋值 |
| 禁止拷贝、允许移动 | 独占 socket、文件、锁等资源 | 删除 copy，保证 move 后源对象仍有效 |

> [!warning]- 易错点
> - 只写析构函数，却忘记自定义拷贝，导致浅拷贝与 double free。
> - 认为 moved-from 对象不可再用；正确要求是它处于有效但未指定状态。
> - 为所有类手写五个函数；这通常比 Rule of Zero 更容易制造 bug。

> [!question]- 自测：先回答再展开
> 1. 一个独占 `FILE*` 的 wrapper 应该支持拷贝吗？移动后源对象必须满足什么条件？
> 2. 为什么 `std::vector<T>` 会关心 `T` 的 move constructor 是否 `noexcept`？
> 3. 哪些成员类型已经让你无需自己写析构函数？

# Object Oriented Programming (面向对象编程)

> [!note] 本节重点：封装、继承、多态三大面向对象特性在 C++ 中的实现

## 封装（Encapsulation）

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
        if (amount <= 0 || amount > balance_) return false;
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

## 继承（Inheritance）

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

## 多态（Polymorphism）

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

# Construction and Destruction (构造与析构)

> [!note] 本节重点：构造/析构顺序（基类→成员→派生类）、virtual 析构函数的重要性

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

# Copy Control and Rule of Five (拷贝控制与五法则)

> [!note] 本节重点：Rule of Five（析构/拷贝构造/拷贝赋值/移动构造/移动赋值）、浅拷贝 vs 深拷贝

```cpp
#include <algorithm>
#include <cstddef>
#include <utility>

class Resource {
    int* data_{nullptr};
    std::size_t size_{0};
public:
    explicit Resource(std::size_t n)
        : data_(n ? new int[n]{} : nullptr), size_(n) {}
    ~Resource() { delete[] data_; }

    Resource(const Resource& other) : Resource(other.size_) {
        if (size_ != 0) std::copy_n(other.data_, size_, data_);
    }
    Resource& operator=(const Resource& other) {
        if (this != &other) {
            Resource copy{other}; // 复制失败则当前对象不变
            swap(*this, copy);
        }
        return *this;
    }

    Resource(Resource&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    friend void swap(Resource& a, Resource& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
        swap(a.size_, b.size_);
    }
};

void example() {
    Resource a{3};
    Resource b{a};       // 深拷贝
    b = Resource{5};     // 移动赋值
}
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

// 用户声明析构、拷贝构造或拷贝赋值会抑制隐式声明的移动操作。
// 仅写 = default 也要检查成员类型是否让函数变为 deleted。
// 真实项目优先使用 RAII 成员，让类遵循 Rule of Zero。
```

---

# Operator Overloading (运算符重载)

运算符应保留读者熟悉的语义。`operator+=` 修改左侧并返回其引用；`operator+` 可以基于它返回新值。下标越界必须有明确契约，不能默默把所有非零下标当成第二个元素。需要左操作数隐式转换的对称运算符通常写成非成员。

```cpp
#include <iostream>
#include <stdexcept>

class Vec2 {
    double x_{};
    double y_{};

public:
    Vec2(double x = 0, double y = 0) : x_(x), y_(y) {}

    Vec2& operator+=(const Vec2& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }
    friend Vec2 operator+(Vec2 lhs, const Vec2& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend Vec2 operator*(double scale, const Vec2& v) {
        return {scale * v.x_, scale * v.y_};
    }
    friend std::ostream& operator<<(std::ostream& out, const Vec2& v) {
        return out << '(' << v.x_ << ", " << v.y_ << ')';
    }

    double& at(int index) {
        if (index == 0) return x_;
        if (index == 1) return y_;
        throw std::out_of_range("Vec2 index");
    }
};

int main() {
    Vec2 a{1, 2}, b{3, 4};
    Vec2 c = a + b;
    Vec2 scaled = 2.0 * a;
    std::cout << c << ' ' << scaled << '\n'; // (4, 6) (2, 4)
}
```

转换运算符如果容易产生意外重载，标为 `explicit`。例如 `explicit operator bool() const` 可用于 `if (value)`，但不会作为普通隐式数值转换。重载比较运算符时，浮点数的精确相等是否符合业务语义要单独决定。`::`、`.`、`.*`、`?:`、`sizeof`、`typeid` 不能重载。



> [!info]- 延伸阅读
> - 下一步：[04-Polymorphism and Inheritance (多态与继承)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)
