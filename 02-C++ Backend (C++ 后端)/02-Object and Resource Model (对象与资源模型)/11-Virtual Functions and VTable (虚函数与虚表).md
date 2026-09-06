---
tags:
  - cpp/core
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# 11-Virtual Functions and VTable (虚函数与虚表)

> [!abstract] 核心考点：虚函数表结构、vptr 指针、单继承下的 VTable 布局

## 虚函数表（VTable）

每个含虚函数的**类**有一个 VTable（虚函数表），表中存放虚函数指针。每个**对象**有一个隐藏的 `vptr`（虚指针），指向其类的 VTable。

```text
┌───────────────────────┐    ┌─────────────────────────┐    ┌─────────────────────┐
│  Object Layout        │    │  VTable (one per class) │    │  Code Segment       │
├───────────────────────┤    ├─────────────────────────┤    ├─────────────────────┤
│  vptr (8 bytes) ──────│───→│  [0] virtual ~Base()   │    │  Base::foo()        │
├───────────────────────┤    ├─────────────────────────┤    │    implementation   │
│  int a                │    │  [1] virtual foo() ─────│───→│                     │
│    (member variable)  │    ├─────────────────────────┤    ├─────────────────────┤
├───────────────────────┤    │  [2] virtual bar() ─────│───→│  Base::bar()        │
│  double b             │    └─────────────────────────┘    │    implementation   │
│    (member variable)  │                                   └─────────────────────┘
└───────────────────────┘
```

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
    // bar() 未覆盖，VTable 中 bar 项指向 Base::bar
};

// vptr 的开销：每个对象多 8 字节（64位），每个类多一个静态 VTable
sizeof(Base);     // 8（只有 vptr）
sizeof(Derived);  // 8（继承 vptr，无额外数据成员）
```

## 虚函数调用流程

```
Base* p = new Derived();
p->foo();
// 汇编等价：
// 1. 从 p 读出 vptr（p 的前 8 字节）
// 2. 从 vptr[0] 读出 foo 的地址
// 3. 间接调用
// 代价：一次额外内存读取 + 不可内联
```

## override & final（C++11）

```cpp
class Base {
    virtual void foo(int);
    virtual void bar();
};

class Derived : public Base {
    void foo(int) override;   // 编译器检查：基类确实有此虚函数签名
    void foo(double) override; // 编译错误！基类没有 foo(double)，防止笔误
    void bar() final;          // 禁止子类再覆盖 bar
};

class Leaf final : public Derived { };  // 禁止继承 Leaf
```

## 30 秒回答 / 自测

- **30 秒回答**：每个含虚函数的类有一张 VTable（存虚函数地址），每个对象开头藏一个 vptr 指向它；虚调用 = 读 vptr → 查表 → 间接跳转，代价是一次额外内存访问 + 不可内联；对象大小多 8 字节（64 位），类多一张静态表。
- **常见误区**：以为 vptr 存在类里（实际每个对象各一个）；在构造函数/析构函数里调虚函数并期望动态分发（此时 vptr 已指向本类，不会派发到派生类）；多态基类忘记声明 `virtual` 析构。
- **自测**：1) `sizeof(Base)`（只有一个虚函数）在 64 位下是多少？ 2) 为什么构造函数里调虚函数不会动态派发到派生类？

---

多态与动态分发详见 → [Polymorphism & Dynamic Dispatch (多态与动态分发)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07b-Polymorphism%20&%20Dynamic%20Dispatch%20(多态与动态分发).md) · [Abstract Class & Pure Virtual (抽象类)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/07-Object%20Model%20&%20VTable%20(对象模型与虚表)%20⭐/07c-Abstract%20Class%20&%20Pure%20Virtual%20(抽象类).md)
