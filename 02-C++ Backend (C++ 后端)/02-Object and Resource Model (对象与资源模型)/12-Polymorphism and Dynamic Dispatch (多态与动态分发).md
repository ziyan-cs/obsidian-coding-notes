---
tags:
  - cpp/core
status: 🌱
---

# 12-Polymorphism and Dynamic Dispatch (多态与动态分发)

> [!abstract] 核心考点：运行时多态的实现机制、动态分派性能开销、RTTI typeid 原理

```cpp
// 动态分派：运行时根据对象实际类型查 VTable
void makeSpeak(Animal* a) {
    a->speak();   // 不是 Animal::speak，而是 a 实际类型的 speak
}

Dog d("Rex");
Cat c("Whiskers");
makeSpeak(&d);   // Derived::speak via VTable
makeSpeak(&c);   // Derived::speak via VTable
```

## 切片问题（Object Slicing）

```cpp
// 按值传递/赋值时，派生类部分被"切掉"
Dog dog("Rex");
Animal a = dog;    // 切片！a 只有 Animal 部分，vptr 指向 Animal::VTable
a.speak();         // 调用 Animal::speak，而非 Dog::speak

// 解决：通过指针或引用使用多态
Animal& ref = dog;
ref.speak();       // 正确，Dog::speak
```

## 虚析构函数的必要性

```cpp
class Base {
public:
    ~Base() { }          // 非虚析构！
};
class Derived : public Base {
    int* data_;
public:
    Derived() : data_(new int[100]) {}
    ~Derived() { delete[] data_; }   // 若不被调用 → 内存泄漏
};

Base* p = new Derived();
delete p;   // 若 ~Base() 非虚：只调用 ~Base()，~Derived() 不被调用 → 泄漏！

// 修复：
class Base {
public:
    virtual ~Base() = default;   // 有继承关系的基类析构必须是 virtual
};
```

---

虚函数表机制详见 → [Virtual Function & VTable Layout (虚函数与虚表结构)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md)
