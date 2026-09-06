---
tags:
  - cpp/modern
status: 🌱
---

# 11-constexpr and Compile Time Computation (编译期计算)

> [!abstract] 核心考点：constexpr 函数、if constexpr、编译期 vs 运行期的边界

## constexpr 变量

```cpp
constexpr int N = 100;           // 编译期常量
constexpr double PI = 3.14159;
constexpr int arr[N] = {};       // 数组大小可用 constexpr

// const vs constexpr
const int x = rand();            // OK：运行期 const
constexpr int y = rand();        // 错误：constexpr 必须编译期可知
```

## constexpr 函数（C++11/14/17 逐步放宽限制）

```cpp
// C++11：函数体只能有 return 语句
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// C++14+：可以有局部变量、循环、if
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int tmp = a + b; a = b; b = tmp;
    }
    return b;
}

constexpr int f10 = fibonacci(10);   // 编译期计算
int arr[fibonacci(8)] = {};          // 数组大小，编译期确定

// 若参数是运行期值，constexpr 函数退化为普通函数
int n;
std::cin >> n;
int runtime_fib = fibonacci(n);      // 运行期计算，完全合法
```

## constexpr 类

```cpp
struct Point {
    double x, y;
    constexpr Point(double x, double y) : x(x), y(y) {}
    constexpr double dist2() const { return x*x + y*y; }
};

constexpr Point p{3.0, 4.0};
constexpr double d = p.dist2();   // 25.0，编译期计算
static_assert(d == 25.0);         // 编译期断言
```

## if constexpr（C++17，编译期条件分支）

```cpp
template<typename T>
void print(T val) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "int: " << val << '\n';
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "float: " << val << '\n';
    } else {
        std::cout << "other: " << val << '\n';
    }
    // 未选中的分支不参与编译，避免类型不兼容的编译错误
}
```

## consteval & constinit（C++20）

```cpp
// consteval：必须在编译期求值，不能作为运行期函数
consteval int square(int n) { return n * n; }
// square(rand());   // 编译错误：rand() 是运行期值

// constinit：变量必须静态初始化（编译期初始化），但可以运行期修改
constinit int g = 42;      // 全局变量，保证静态初始化
g = 100;                    // 运行期可修改
```

---

现代 C++ 编译期计算作为核心特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)
