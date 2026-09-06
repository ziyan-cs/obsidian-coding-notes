---
tags:
  - cpp/core
status: 🌱
---

# OOP Principles — 面向对象三大特性

> [!important] **核心考点**：封装、继承、多态三大面向对象特性在 C++ 中的实现

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

多态与对象模型详见 → [Virtual Function & VTable Layout (虚函数与虚表结构)](/03-C++%20Programming%20(编程语言)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md)
