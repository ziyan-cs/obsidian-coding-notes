#cpp #virtual #override #final #dynamic-binding

## ⚡ TL;DR（快速决策）

- `virtual`、`override`、`final` 本质上都在服务于：**更安全、更清晰的运行时多态**
- 作用分别是：
    - `virtual`：声明该函数支持动态绑定
    - `override`：明确表示这是在重写基类虚函数
    - `final`：禁止继续重写或继续继承
- 现代 C++ 里，重写虚函数时应尽量配 `override`

## 🧩 Core Idea（核心本质）

- `virtual` 决定“能否多态调用”
- `override` 决定“编译器帮你检查是不是真的重写成功”
- `final` 决定“到这里为止，别再改了”
- 一句话理解：
    - **这三个关键字是在给虚函数机制加规则、加保护。**

## 🔧 Usage Patterns（可复用代码模板）

1. `virtual`

```cpp
class Base {
public:
    virtual void f() {
        cout << "Base\\n";
    }
};
```

1. `override`

```cpp
class Derived : public Base {
public:
    void f() override {
        cout << "Derived\\n";
    }
};
```

1. `final` 修饰函数

```cpp
class Child : public Base {
public:
    void f() override final {}
};
```

1. `final` 修饰类

```cpp
class Last final {};
```

## ⚠️ Pitfalls（高频错误）

- 只写了同名函数，但参数不同，结果没真正 override
- 忘记 `override` 导致重写失败却不自知
- 不理解 `final` 会限制扩展
- 误把普通成员函数当成虚函数机制的一部分

## 🚀 Performance / Tips（性能优化）

- `override` 主要提升正确性与维护性
- `final` 除了设计约束，有时也能帮助编译器做更明确判断
- 团队开发里，`override` 几乎应视作好习惯

## 🧪 Common Scenarios（常见使用场景）

- 多态基类接口设计
- 派生类重写检查
- 限制接口继续扩展
- 框架类层次管理

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    virtual void show() { cout << "base\\n"; }
};

class Derived : public Base {
public:
    void show() override { cout << "derived\\n"; }
};

int main() {
    Base* p = new Derived();
    p->show();
    delete p;
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`virtual`、`override`、`final` 的核心就是：让虚函数机制更明确、更安全、更可维护。**