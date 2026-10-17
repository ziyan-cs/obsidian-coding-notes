---
study_stage: learn
review_due: 2026-10-29
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

> [!info]- 延伸阅读
> - 下一步：[04-Polymorphism and Inheritance (多态与继承)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/04-Polymorphism%20and%20Inheritance%20(多态与继承).md)
