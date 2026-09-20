---
study_stage: backlog
---


> [!summary] 核心摘要
>
> 继承用于表达稳定的 is-a 抽象，运行时多态通过虚函数经由基类接口选择具体行为。它的代价是耦合、间接调用和对象布局复杂度；优先组合，只有确实需要以统一接口替换不同实现时再用继承。

# 使用边界

| 需求 | 更合适的方式 |
| --- | --- |
| 复用实现细节 | 组合成员，而非 public inheritance |
| 运行时替换实现 | 纯虚接口 + 明确所有权 |
| 编译期多态 | templates / concepts |
| 异构对象集合 | 基类指针或 type erasure，并管理生命周期 |

## 必须守住的规则

1. 多态基类通常应有 virtual destructor，否则经由基类指针删除派生对象是未定义行为。
2. 构造和析构期间虚调用不会分派到尚未构造或已经析构的派生层。
3. 多继承与虚继承只用于明确的接口组合或菱形共享基类问题，不能作为复用捷径。

> [!question]- 自测：先回答再展开
> 1. 为什么有虚函数的类不等于应该被继承的类？
> 2. 哪个场景应选择组合而非继承？
> 3. virtual destructor 缺失会怎样造成资源问题？

# Virtual Functions and VTable (虚函数与虚表)

> [!note] 本节重点：标准规定动态分派语义；vtable/vptr 是常见 ABI 实现，不是语言要求。

## 虚函数表（VTable）

C++ 规定虚函数调用的动态分派语义，却**不规定**必须使用 vtable/vptr，更不规定对象中指针的位置、大小或表项次序。下图仅是常见 ABI 的概念示意，不代表任意编译器的真实布局：

```text
base pointer --> dynamic object --> implementation metadata
                                      |
                                      +--> final overrider of foo()
                                      +--> final overrider of bar()
```

面试时先讲动态类型与 final overrider，再补充“很多实现借助虚表”。对象大小、反汇编和布局输出可用于观察本机实现，不能拿来推导语言保证。

```cpp
class Base {
public:
    virtual void foo() { std::cout << "Base::foo\n"; }
    virtual void bar() { std::cout << "Base::bar\n"; }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void foo() override { std::cout << "Derived::foo\n"; }
    // bar() 未覆盖，调用继承来的 Base::bar
};

// 观察本机实现：这里的 sizeof 结果由编译器、ABI 与目标平台决定。
std::cout << sizeof(Base) << ' ' << sizeof(Derived) << '\n';
```

## 虚函数调用流程

```
Base* p = new Derived();
p->foo();
// 语义：根据对象动态类型调用 foo 的最终覆盖者。
// 实现可能使用虚表；优化器若证明动态类型，也可能去虚化并内联。
```

# override & final（C++11）

```cpp
class Base {
    virtual void foo(int);
    virtual void bar();
};

class Derived : public Base {
    void foo(int) override;   // 编译器检查：基类确实有此虚函数签名
    // void foo(double) override; // 故意注释：签名不匹配，取消注释会编译失败
    void bar() final;          // 禁止子类再覆盖 bar
};

class Leaf final : public Derived { };  // 禁止继承 Leaf
```


# Polymorphism and Dynamic Dispatch (多态与动态分发)

> [!note] 本节重点：运行时多态的实现机制、动态分派性能开销、RTTI typeid 原理

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
delete p;   // 静态类型 Base* 删除 Derived 且析构非虚：未定义行为，不能只描述为泄漏

// 修复：
class Base {
public:
virtual ~Base() = default;   // 允许经 Base* 删除派生对象时需虚析构
};
```

---

# Abstract Classes and Pure Virtual (抽象类与纯虚函数)

> [!note] 本节重点：若类中至少有一个纯虚函数的最终覆盖者仍为纯虚，该类就是抽象类，不能创建对象；这与“虚表不完整”无关。

```cpp
class Shape {
public:
    // 纯虚函数：= 0；派生类若仍未提供非纯最终覆盖者，也保持抽象
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

---

# Multiple and Virtual Inheritance (多继承与虚继承)

> [!note] 本节重点：多继承的二义性、虚继承解决菱形继承问题、对象布局变化

## 多继承

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

## 菱形继承问题

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

## 虚继承（解决菱形继承）

```cpp
struct A { int x = 0; };
struct B : virtual A { };   // 虚继承
struct C : virtual A { };   // 虚继承
struct D : B, C { };        // D 中只有一份 A

D d;
d.x = 42;    // 不再歧义，只有一个 A::x
```

**虚继承的代价：**

- 虚基类定位可能需要额外布局元数据或地址调整；`vbptr` 只是某些 ABI 的实现方式，标准不保证它存在，也不能预设固定开销
- 对象布局更复杂，构造顺序也有变化（最终派生类负责虚基类的构造）

规范核对：[虚基类子对象](https://eel.is/c++draft/class.derived) 规定共享一份基类子对象，并未规定 `vbptr` 等具体布局。

## 布局示意

```text
Diamond Virtual Inheritance Layout:

                    Base (int data)
                   /                \
                  ↓                  ↓
          Derived1               Derived2
          (+ int d1_data)        (+ int d2_data)
                  \                /
                   ↓              ↓
              DerivedFinal (+ int dd_data)
              ─────────────────────────────────
              Only one copy of Base sub-object
              (location depends on implementation)
```

---
