---
tags:
  - cpp/modern
status: seed
review_due: 2026-09-12
confidence: 1
verified: stable
---

# Move Constructor & Move Assignment — 移动构造与移动赋值

> [!important] **核心考点**：移动构造函数与移动赋值运算符的实现、noexcept 的重要性、资源窃取语义

移动语义允许"偷走"临时对象的资源，而非深拷贝，大幅降低开销。

```cpp
class MyString {
    char* data;
    size_t size;
public:
    // 拷贝构造：深拷贝，O(n)
    MyString(const MyString& other) : size(other.size) {
        data = new char[size];
        memcpy(data, other.data, size);
    }

    // 移动构造：偷走指针，O(1)
    MyString(MyString&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;   // 将原对象置于合法但未定义状态
        other.size = 0;
    }

    // 移动赋值
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            delete[] data;          // 释放自己的旧资源
            data = other.data;      // 偷走
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~MyString() { delete[] data; }
};
```

### 五法则（Rule of Five）

若类需要自定义以下任意一个，通常需要全部自定义：

|特殊函数|声明方式|
|---|---|
|析构函数|`~T()`|
|拷贝构造|`T(const T&)`|
|拷贝赋值|`T& operator=(const T&)`|
|**移动构造**|`T(T&&) noexcept`|
|**移动赋值**|`T& operator=(T&&) noexcept`|

```cpp
// 不需要资源管理时：用 = default 让编译器生成
struct Point {
    double x, y;
    Point(const Point&) = default;
    Point(Point&&)      = default;
    Point& operator=(const Point&) = default;
    Point& operator=(Point&&)      = default;
    ~Point() = default;
};
```

### noexcept 的重要性

移动构造/赋值**必须标记 `noexcept`**，否则 `std::vector` 等容器在重新分配内存时会退回使用拷贝（无法保证异常安全）：

```cpp
std::vector<MyString> v;
v.push_back(MyString("hello"));
// 若移动构造未标 noexcept：push_back 扩容时用拷贝，性能损失！
```

## 30 秒回答 / 自测

- **30 秒回答**：移动构造/赋值"偷走"被移动对象的资源（指针/句柄）并把其置于可析构状态，把 O(n) 深拷贝降为 O(1)。
- **常见误区**：移动后仍使用被移动对象（合法但状态未定义，通常只允许重新赋值或析构）；移动构造没标 `noexcept`，导致 `std::vector` 扩容退回拷贝。
- **自测**：1) 移动构造后，被移动对象的指针成员应设为什么？ 2) 为什么 `std::vector` 要求移动构造 `noexcept` 才在扩容时用它？

---

返回值优化与移动语义详见 → [move & RVO (移动语义与返回值优化)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/03-Move%20Semantics%20&%20Rvalue%20Reference%20(移动语义)%20⭐/03c-move%20&%20RVO%20(移动语义与返回值优化).md)
