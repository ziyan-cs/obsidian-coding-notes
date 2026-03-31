#cpp #struct #class #oop #encapsulation

## ⚡ TL;DR（快速决策）

- `struct` 和 `class` 本质都是：**把数据和操作组织在一起的自定义类型**
- 最大语法区别：
    - `struct` 默认公有
    - `class` 默认私有
- 一看到这些需求，要优先想到它们：
    - 想把多个相关数据打包
    - 想定义对象行为
    - 想进行封装和抽象
- 在 C++ 里，`struct` 不只是“纯数据”，也能有函数

## 🧩 Core Idea（核心本质）

- 它们都可以拥有：
    - 成员变量
    - 成员函数
    - 构造函数
    - 访问控制
- 一句话理解：
    - **类 / 结构体就是程序员自定义的数据类型。**
- 设计意义：
    - 让数据和操作绑定在一起
    - 提高可读性与可维护性

## 🔧 Usage Patterns（可复用代码模板）

1. `struct` 基础

```cpp
struct Point {
    int x, y;
    void print() {
        cout << x << ' ' << y << '\\n';
    }
};
```

1. `class` 基础

```cpp
class Person {
private:
    string name;
public:
    void setName(const string& s) { name = s; }
    string getName() const { return name; }
};
```

1. 创建对象

```cpp
Point p{1, 2};
p.print();
```

1. `struct` 与 `class` 默认访问差别

```cpp
struct A { int x; };   // 默认 public
class B { int x; };    // 默认 private
```

## ⚠️ Pitfalls（高频错误）

- 以为 `struct` 不能写函数
- 不清楚默认访问权限差异
- 成员初始化和赋值混为一谈
- 访问私有成员方式错误

## 🚀 Performance / Tips（性能优化）

- `struct` / `class` 的核心是设计，不是直接性能优化
- 小型数据聚合常用 `struct`
- 更强调封装和接口时常用 `class`
- 先关注语义清晰，再关注形式选择

## 🧪 Common Scenarios（常见使用场景）

- 点、边、节点等数据建模
- 用户自定义对象
- 封装状态和行为
- 图论 / 几何 / 模拟题中的自定义类型

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x, y;
};

int main() {
    Point p{3, 4};
    cout << p.x << ' ' << p.y << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`struct` 与 `class` 的核心就是：定义包含数据和行为的自定义类型。**