
## const

```cpp
const int x = 42;        // 不可修改
int const y = 42;         // 同上，等价写法

// 指针与 const（从右往左读）
int* const p1 = &x;       // 常量指针：指针本身不可改，指向的值可改
const int* p2 = &x;       // 指向常量的指针：指针可改，指向的值不可改
const int* const p3 = &x; // 双 const：指针和值都不可改

// 函数中的 const
void foo(const std::string& s);   // 传常量引用，避免拷贝且不可修改
int getValue() const;              // 成员函数 const：不修改对象状态
mutable int cache_;                // mutable：在 const 函数中也可修改
```

## typedef & using

```cpp
typedef unsigned long long ull;   // C 风格
using ull = unsigned long long;   // C++11，更清晰

// 函数指针别名（using 更直观）
typedef int (*FuncPtr)(int, int);
using FuncPtr = int(*)(int, int);

// 模板别名（typedef 不支持，只能用 using）
template<typename T>
using Vec = std::vector<T>;
Vec<int> v;   // = std::vector<int>
```

## enum & enum class

```cpp
// 传统 enum：值会污染外围作用域，隐式转换为 int
enum Color { RED, GREEN, BLUE };   // RED=0, GREEN=1, BLUE=2
int x = RED;                       // 隐式转为 int

// enum class（C++11，强类型枚举，推荐）
enum class Direction { NORTH, SOUTH, EAST, WEST };
Direction d = Direction::NORTH;     // 必须加作用域
// int x = d;   // 错误！不隐式转换

// 指定底层类型
enum class Status : uint8_t { OK = 0, ERROR = 1, TIMEOUT = 2 };

// 使用
switch (d) {
    case Direction::NORTH: break;
    case Direction::SOUTH: break;
    default: break;
}
```