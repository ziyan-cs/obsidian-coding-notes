---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习目标：区分 constexpr、consteval、constinit 与 if constexpr 的生效阶段和使用边界。

> [!note] 本节重点：constexpr 函数、if constexpr、编译期 vs 运行期的边界

## constexpr 变量

```cpp
constexpr int N = 100;           // 编译期常量
constexpr double PI = 3.14159;
constexpr int arr[N] = {};       // 数组大小可用 constexpr

// const vs constexpr
const int x = rand();            // OK：运行期 const
constexpr int y = rand();        // 错误：constexpr 必须编译期可知
```

# constexpr 函数（C++11/14/17 逐步放宽限制）

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

# if constexpr（C++17，编译期条件分支）

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

# consteval & constinit（C++20）

```cpp
// consteval：必须在编译期求值，不能作为运行期函数
consteval int square(int n) { return n * n; }
// square(rand());   // 编译错误：rand() 是运行期值

// constinit：变量必须静态初始化（编译期初始化），但可以运行期修改
constinit int g = 42;      // 全局变量，保证静态初始化
g = 100;                    // 运行期可修改
```

---

# 常量求值不是“另一种函数”

`constexpr` 函数可以在常量求值或运行期求值，取决于调用上下文和参数。需要常量表达式的地方（`static_assert`、非类型模板参数等）若无法求值会编译失败；普通初始化则可以退回运行期。

```cpp
constexpr int checked_square(int x) {
    if (x > 46340 || x < -46340) throw "overflow";
    return x * x;
}
static_assert(checked_square(12) == 144);
int n = read();
int value = checked_square(n); // 运行期执行，越界时抛出
```

`consteval` 声明 immediate function，每次潜在求值调用都必须产生常量；适合编译期校验和生成。`constinit` 只适用于静态或线程存储期变量，保证静态初始化，避免动态初始化顺序问题；它不让变量成为 `const`，也不表示所有后续访问发生在编译期。

`if constexpr` 只丢弃未选中的从属分支；分支外的语法错误以及与模板参数无关的非法代码仍可能报错。`std::is_constant_evaluated()` 可以区分求值环境，但不应让同一 API 的业务语义悄悄分叉。

编译期计算会增加编译时间、诊断复杂度和二进制实例化成本。适合不变量、查表和类型约束，不要为展示技巧把普通运行期逻辑搬进模板。用 `static_assert` 验证边界，并分别编译常量与运行期调用。

参考：[cppreference constant expressions](https://en.cppreference.com/w/cpp/language/constant_expression.html)。
