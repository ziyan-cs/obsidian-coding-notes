#cpp #operator-overloading #class #syntax-sugar #oop

## ⚡ TL;DR（快速决策）

- 运算符重载本质是：**让自定义类型也能像内置类型那样使用某些运算符**
- 一看到这些场景，要优先想到运算符重载：
    - 复数、分数、矩阵、向量等数学对象
    - 想让类的使用方式更自然
    - 希望支持 `<<`、`+`、`==` 等语义操作
- 运算符重载的核心不是炫技，而是：**语义自然且不误导**

## 🧩 Core Idea（核心本质）

- 本质上是把运算符变成函数
- 例如 `a + b` 背后可对应某个 `operator+`
- 一句话理解：
    - **运算符重载就是给自定义类型定义“这些符号该怎么工作”。**
- 但运算符优先级、结合性等语言规则不会因为重载而改变

## 🔧 Usage Patterns（可复用代码模板）

1. 重载 `+`

```cpp
class Point {
public:
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
};
```

1. 重载 `==`

```cpp
bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
}
```

1. 重载输出运算符

```cpp
friend ostream& operator<<(ostream& os, const Point& p) {
    os << p.x << ' ' << p.y;
    return os;
}
```

## ⚠️ Pitfalls（高频错误）

- 为不合适的语义乱重载运算符
- 忘记 `const` 导致接口不自然
- `<<`、`>>` 通常适合写成友元或非成员函数
- 重载后行为和用户直觉不一致会非常糟糕

## 🚀 Performance / Tips（性能优化）

- 重载重点是接口设计，不是性能炫技
- 小对象可按值返回，大对象注意引用和拷贝开销
- 常见建议：
    - 只重载有明确数学 / 语义含义的运算符
    - 保持和内置类型尽量一致的直觉

## 🧪 Common Scenarios（常见使用场景）

- 复数 / 向量 / 矩阵类
- 比较运算
- 输出打印
- 智能指针和迭代器类接口设计

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

class Point {
public:
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
};

int main() {
    Point a(1, 2), b(3, 4);
    Point c = a + b;
    cout << c.x << ' ' << c.y << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **运算符重载就是：为自定义类型定义运算符行为，让接口更自然、更符合直觉。**