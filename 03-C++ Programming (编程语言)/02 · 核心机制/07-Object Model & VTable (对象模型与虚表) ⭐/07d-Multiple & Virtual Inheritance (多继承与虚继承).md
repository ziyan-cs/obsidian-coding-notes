---
tags:
  - cpp/core
status: 🌱
---

> [!important] **核心考点**：多继承的二义性、虚继承解决菱形继承问题、对象布局变化

### 多继承

```cpp
class Flyable { public: virtual void fly() = 0; };
class Swimmable { public: virtual void swim() = 0; };

class Duck : public Flyable, public Swimmable {
public:
    void fly()  override { std::cout << "Duck flies\n"; }
    void swim() override { std::cout << "Duck swims\n"; }
};

// 多继承时 this 指针可能调整（不同基类子对象偏移不同）
Duck d;
Flyable*   fp = &d;   // fp 可能与 &d 相同或有偏移
Swimmable* sp = &d;   // 同上，取决于布局
```

### 菱形继承问题

```cpp
struct A { int x; };
struct B : A { };
struct C : A { };
struct D : B, C { };  // D 有两份 A::x，产生歧义！

D d;
// d.x;        // 错误：歧义，是 B::A::x 还是 C::A::x？
d.B::x = 1;    // 显式指定
d.C::x = 2;
```

### 虚继承（解决菱形继承）

```cpp
struct A { int x = 0; };
struct B : virtual A { };   // 虚继承
struct C : virtual A { };   // 虚继承
struct D : B, C { };        // D 中只有一份 A

D d;
d.x = 42;    // 不再歧义，只有一个 A::x
```

**虚继承的代价：**

- 每个虚基类子对象通过**虚基类指针（vbptr）** 间接访问，额外内存和性能开销
- 对象布局更复杂，构造顺序也有变化（最终派生类负责虚基类的构造）

### 布局示意

```
普通多继承 D 的布局：
┌──────────┐
│ B 子对象 │ ← 含 A 的一份副本
│   A::x   │
├──────────┤
│ C 子对象 │ ← 含 A 的另一份副本（菱形问题）
│   A::x   │
├──────────┤
│ D 自有   │
└──────────┘

虚继承 D 的布局：
┌──────────┐
│ B 子对象 │ ← vbptr → A 子对象
├──────────┤
│ C 子对象 │ ← vbptr → A 子对象（同一份！）
├──────────┤
│ D 自有   │
├──────────┤
│ A 子对象 │ ← 只有一份，放在最后
│   A::x   │
└──────────┘
```

---

虚函数表在多继承中的布局详见 → [Virtual Function & VTable Layout (虚函数与虚表结构)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07a-Virtual%20Function%20&%20VTable%20Layout%20(虚函数与虚表结构).md)
