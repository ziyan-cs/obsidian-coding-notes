---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Perfect Forwarding and Universal References (完美转发)

> [!note] 本节重点：核心考点：万能引用、引用折叠、std::forward 的作用

> [!warning] `std::forward` 只用于保留原始值类别
> 它不是“更快的 `std::move`”。转发同一个对象后仍继续依赖其状态会让调用者难以判断所有权；只有下游确实需要按原值类别重载时才使用。

# 万能引用（Universal Reference）

`T&&` 出现在**类型推导上下文**中是万能引用，可以绑定左值也可以绑定右值：

```cpp
template<typename T>
void foo(T&& arg);    // T&& 是万能引用，T 由调用方推导

auto&& x = expr;     // auto&& 也是万能引用
```

注意区分：`std::vector<int>&&` 是右值引用（类型已确定，无推导）。

# 引用折叠规则

T 被推导为引用类型时，`T&&` 按以下规则折叠：

|T 推导为|T&& 结果|
|---|---|
|`int`|`int&&`（右值引用）|
|`int&`|`int& &&` → `int&`（左值引用）|
|`int&&`|`int&& &&` → `int&&`（右值引用）|

**记忆：有左值引用就折叠为左值引用（& 优先）。**

```cpp
// 传入左值 → T 推导为 int& → T&& = int& （左值引用）
// 传入右值 → T 推导为 int  → T&& = int&&（右值引用）
int x = 10;
foo(x);           // T = int&,  arg 类型 int&
foo(42);          // T = int,   arg 类型 int&&
foo(std::move(x));// T = int,   arg 类型 int&&
```

# std::forward（完美转发）

在函数内部，参数 `arg` 无论如何都是**左值**（有名字）。`std::forward<T>(arg)` 根据 T 的推导结果，将 arg 恢复为原来的值类别：

```cpp
template<typename T>
void wrapper(T&& arg) {
    // arg 在这里是左值（有名字）
    foo(arg);                      // 永远传左值 ❌
    foo(std::move(arg));           // 永远传右值 ❌
    foo(std::forward<T>(arg));     // 保持原值类别 ✅
    // T=int&  → forward<int&>(arg)  → static_cast<int&>(arg)  → 左值
    // T=int   → forward<int>(arg)   → static_cast<int&&>(arg) → 右值
}
```

# 完美转发的实际应用

```cpp
// make_unique 的简化实现
template<typename T, typename... Args>
std::unique_ptr<T> my_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// emplace 系列函数底层原理
template<typename T>
class MyVector {
    template<typename... Args>
    void emplace_back(Args&&... args) {
        new (end_) T(std::forward<decltype(args)>(args)...);
    }
};

// 通用包装函数（日志、计时、缓存装饰器）
template<typename F, typename... Args>
decltype(auto) timed_call(F&& f, Args&&... args) {
    auto t0 = std::chrono::steady_clock::now();
    // 为简洁起见，此版本只示意返回非 void 的调用；void 需单独分支处理。
    decltype(auto) result = std::forward<F>(f)(std::forward<Args>(args)...);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()
              << "us\n";
    return result;
}
```

---

类型推导是完美转发的基础，详见 → [Type Deduction (类型推导)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/01-Type%20Deduction%20(类型推导).md)

# 30 秒回答

转发引用（常称 universal reference）只在 `T&&` 且 `T` 发生推导时成立。传入左值会让 `T` 推导为左值引用，传入右值会让 `T` 推导为非引用类型；函数参数本身有名字，表达式永远是左值，因此用 `std::forward<T>(arg)` 才能把原始值类别交给下游。

# 自测

1. 为什么 `foo(arg)` 与 `foo(std::move(arg))` 都不是通用 wrapper 的正确默认写法？
2. `std::vector<int>&&` 为什么不是转发引用？
3. `std::forward` 后为什么不能假设对象仍保留原来的值？

---

# constexpr and Compile Time Computation (编译期计算)

> [!note] 本节重点：核心考点：constexpr 函数、if constexpr、编译期 vs 运行期的边界

# constexpr 变量

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

# constexpr 类

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

现代 C++ 编译期计算作为核心特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)

---

# optional and variant (新类型工具)

> [!note] 本节重点：核心考点：optional、variant、any 三种新型类型工具的适用场景

# std::optional（C++17）

表示"可能有值也可能没有值"，替代空指针、哨兵值、`bool` + 输出参数：

```cpp
#include <optional>

std::optional<int> parse(const std::string& s) {
    try { return std::stoi(s); }
    catch (...) { return std::nullopt; }  // 无值
}

auto result = parse("42");
if (result) {                        // 检查是否有值
    std::cout << *result;            // 解引用
    std::cout << result.value();     // 同上，但无值时抛 std::bad_optional_access
}
result.value_or(0);                  // 有值返回值，无值返回 0
result.has_value();                  // 显式检查

// 链式操作（C++23 monadic interface）
auto opt = parse("5")
    .transform([](int x){ return x * 2; })    // 有值则转换
    .and_then([](int x) -> std::optional<int> {
        return x > 5 ? std::optional{x} : std::nullopt;
    });
```

**optional 不适合大对象**（optional 总是栈分配，总占用 = sizeof(T) + 对齐字节）。

---

# std::variant（C++17）

类型安全的联合体（Tagged Union），可以存储多种类型之一：

```cpp
#include <variant>

std::variant<int, double, std::string> v;

v = 42;
v = 3.14;
v = std::string("hello");

// 访问
std::get<std::string>(v);         // 若类型不匹配抛 std::bad_variant_access
std::get<2>(v);                   // 按下标
std::get_if<std::string>(&v);     // 返回指针，不匹配返回 nullptr

v.index();                        // 当前持有类型的下标（0-based）
std::holds_alternative<int>(v);   // 检查是否持有某类型

// std::visit：访问当前值（推荐）
std::visit([](auto&& val) {
    using T = std::decay_t<decltype(val)>;
    if      constexpr (std::is_same_v<T, int>)         std::cout << "int: "    << val;
    else if constexpr (std::is_same_v<T, double>)      std::cout << "double: " << val;
    else if constexpr (std::is_same_v<T, std::string>) std::cout << "str: "    << val;
}, v);
```

## 用 variant 实现错误处理

```cpp
using Result = std::variant<std::string, std::error_code>;

Result readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) return std::make_error_code(std::errc::no_such_file_or_directory);
    return std::string{std::istreambuf_iterator<char>(f), {}};
}

auto r = readFile("config.json");
std::visit([](auto&& v) {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::string>)
        std::cout << "content: " << v;
    else
        std::cout << "error: " << v.message();
}, r);
```

---

optional 与 variant 是现代 C++ 新增的重要类型工具，其他特性详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)

---

# string view and Structured Bindings (轻量视图)

> [!note] 本节重点：核心考点：string_view 非拥有视图与生命周期注意事项、结构化绑定的使用场景

# std::string_view（C++17）

对字符串的**非拥有只读视图**，避免不必要的字符串拷贝：

```cpp
#include <string_view>

// 可以从字面量、string、char* 构造，零拷贝
void print(std::string_view sv) {
    std::cout << sv << " len=" << sv.size() << '\n';
}

print("hello");                        // char 字面量，无拷贝
print(std::string("world"));           // std::string，无拷贝（只存指针和长度）

std::string_view sv = "hello world";
sv.substr(0, 5);    // 返回新的 string_view，不分配内存
sv.starts_with("hello");   // C++20
sv.find("world");
```

## string_view 的生命周期陷阱

```cpp
// 危险！string_view 持有临时 string 的引用，函数返回后悬空
std::string_view dangerous() {
    std::string s = "hello";
    return s;   // s 析构后 string_view 悬空！
}

// 安全：被观察的对象必须比 string_view 活得更长
std::string s = "hello world";
std::string_view sv = s;   // OK，sv 的生命周期在 s 内
```

**函数参数用 `string_view` 代替 `const string&`：**

```cpp
// 旧写法：传字面量时会构造临时 string
void old_func(const std::string& s);

// 新写法：零开销，同时接受 string、字面量、char* 等
void new_func(std::string_view sv);
```

---

# Structured Bindings（结构化绑定，C++17）

解包 pair、tuple、struct、数组到多个命名变量：

```cpp
// pair
std::pair<int, std::string> p{1, "Alice"};
auto [id, name] = p;

// tuple
auto [x, y, z] = std::make_tuple(1, 2.0, "three");

// map 遍历（最常用）
std::map<std::string, int> scores{{"Alice",95},{"Bob",87}};
for (auto& [name, score] : scores) {
    std::cout << name << ": " << score << '\n';
}

// struct（聚合类型）
struct Point { double x, y; };
Point pt{3.0, 4.0};
auto [px, py] = pt;

// 数组
int arr[] = {1, 2, 3};
auto [a, b, c] = arr;

// 绑定为引用（可修改）
auto& [rx, ry] = pt;
rx = 10.0;   // 修改 pt.x
```

## 与 if/switch 结合（C++17 init-statement）

```cpp
// 在 if 的初始化语句中使用结构化绑定
if (auto [it, ok] = myMap.insert({key, val}); ok) {
    std::cout << "inserted\n";
} else {
    std::cout << "key already exists\n";
}

// lock_guard + 结构化绑定（C++17 scoped init）
if (auto [lock, data] = acquireData(); data.valid()) {
    process(data);
}
```

---

string_view 与结构化绑定是现代 C++ 的轻量视图特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)

---

# Concepts and SFINAE (概念与模板元编程)

> [!note] 本节重点：核心考点：Concepts (C++20) 约束模板参数、SFINAE 是实现模板重载的传统技法、enable_if 的使用

# Concept 基础（C++20）

```cpp
#include <concepts>

// 定义一个 Concept
template<typename T>
concept Integral = std::is_integral_v<T>;

template<typename T>
concept HasSize = requires(T t) {     // requires 表达式
    { t.size() } -> std::integral;     // 必须有 size() 且返回整数类型
    { t.begin() } -> std::same_as<decltype(t.begin())>;
};

// 使用 Concept 约束模板
template<Integral T>
T add(T a, T b) { return a + b; }

add(1, 2);      // ✅ int 满足 Integral
add(1.0, 2.0);  // ❌ double 不满足 Integral → 编译错误

// 更简洁的写法
void print(const HasSize auto& container) {
    std::cout << "size: " << container.size();
}

// auto 约束
std::integral auto half(std::integral auto x) { return x / 2; }
```

# 常用标准 Concepts

```cpp
// 核心 concepts
std::same_as<T, U>          // T == U
std::derived_from<T, Base>  // T 继承自 Base
std::convertible_to<T, U>   // T 可转换为 U
std::integral<T>             // 整数类型
std::floating_point<T>       // 浮点类型
std::copyable<T>             // 可拷贝
std::movable<T>              // 可移动
std::invocable<F, Args...>   // 可以 F(Args...) 方式调用

// 结合使用
template<typename T>
concept SortableContainer = requires(T c) {
    typename T::value_type;              // 有 value_type
    { c.begin() } -> std::same_as<typename T::iterator>;
    { c.end() }   -> std::same_as<typename T::iterator>;
    requires std::strict_weak_order<decltype(std::less{}), 
                                     typename T::value_type, 
                                     typename T::value_type>;
} && std::regular<T>;
```

# requires 表达式的三种形式

```cpp
// 1. 简单需求：成员存在
concept HasName = requires(T t) {
    t.name;        // T 必须有 name 成员（数据或函数）
};

// 2. 类型需求：类型存在
concept HasValueType = requires {
    typename T::value_type;  // T 必须有嵌套类型 value_type
};

// 3. 复合需求：表达式 + 返回类型
concept Serializable = requires(T t, std::ostream& os) {
    { serialize(t) } -> std::same_as<std::string>;
    { os << t }      -> std::convertible_to<std::ostream&>;
};

// 4. 嵌套需求
concept LargeIntegral = std::integral<T> && requires {
    requires sizeof(T) >= 4;  // 额外的约束
};
```

# SFINAE（Substitution Failure Is Not An Error）

```cpp
// 最基础的 enable_if 用法
template<typename T>
std::enable_if_t<std::is_integral_v<T>, T>
process(T val) { return val + 1; }  // 整数类型版本

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, T>
process(T val) { return val * 2; }  // 浮点类型版本

// 两个模板同时存在，根据 T 的类型选择合适的版本

// 另一种写法（更常见）：
template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void func(T val);
```

# 更多 SFINAE 技法

```cpp
// 检测成员是否存在（传统 SFINAE）
template<typename T, typename = void>
struct has_value_type : std::false_type {};

template<typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> 
    : std::true_type {};

// 使用
static_assert(has_value_type<std::vector<int>>::value);  // ✅
static_assert(!has_value_type<int>::value);               // ✅

// decltype + SFINAE
template<typename T>
auto process(const T& t) -> decltype(t.size(), void(), t[0]) {
    return t[0];  // T 有 size() 和 operator[] 时可用
}
```

# Concepts 如何改进 SFINAE

| 方面 | 传统 SFINAE | C++20 Concepts |
|------|------------|----------------|
| 可读性 | 晦涩（多层 enable_if） | 清晰自然 |
| 错误信息 | 几十行模板错误 | 直接指出哪个 concept 不满足 |
| 重载优先级 | 靠 SFINAE 技巧 | `requires` 子句自然排序 |
| 代码量 | 多（需要定义 trait） | 少 |

```cpp
// SFINAE 版（噪音多）
template<typename T>
std::enable_if_t<
    std::is_same_v<decltype(std::declval<T>().size()), typename T::size_type>
    && std::is_same_v<decltype(std::declval<T>().begin()), typename T::iterator>,
    void
> print(const T& c) { /* ... */ }

// Concepts 版（语义清晰）
template<typename T>
concept Container = requires(T t) {
    { t.size() } -> std::same_as<typename T::size_type>;
    { t.begin() } -> std::same_as<typename T::iterator>;
};

void print(const Container auto& c) { /* ... */ }
```

# 工程建议

```cpp
// ✅ 如果你用 C++20，优先用 Concepts 而非 SFINAE
// ✅ Concepts 提供的错误信息对库使用者友好

// ✅ 编写模板库时同时保留 SFINAE 回退（兼容 C++17）
#if __cplusplus >= 202002L
    template<Integral T>
    T half(T x) { return x / 2; }
#else
    template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    T half(T x) { return x / 2; }
#endif

// ❌ 不要过度约束 — Concept 应根据接口需求设计
// ❌ 不用 concepts 定义运行时多态替代品（用虚函数就好）
```

> **面试重点**：Concepts 最常见的应用是约束模板参数类型。常被问到 "SFINAE 是什么"——回答：替换失败不是错误，编译器在模板参数推导时，如果某个特化失败，不会报错而是继续尝试其他重载。

---

Concepts 是 C++20 约束模板参数的重要特性，详见 → [Modern C++ Overview (现代 C++ 特性总览)](/02-C++%20Backend%20(C++%20后端)/03-Modern%20C++%20(现代%20C++)/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **04-Generic Programming and Type Tools (泛型与类型工具)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
