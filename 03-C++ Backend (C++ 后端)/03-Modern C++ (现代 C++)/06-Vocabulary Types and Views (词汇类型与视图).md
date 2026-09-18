---
status: stable
confidence: high
content_verified: 2026-09-17
---

> [!abstract] 学习目标：用 optional、variant、string_view 和结构化绑定表达状态、联合值与非拥有视图。

> [!note] 本节重点：optional、variant、any 三种新型类型工具的适用场景

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

`optional<T>` 在自身存储中容纳 `T`，不会为所含对象单独动态分配；是否适合大对象取决于对象体积、复制移动成本和外围对象的存储位置。

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

# string view and Structured Bindings (轻量视图)

> [!note] 本节重点：string_view 非拥有视图与生命周期注意事项、结构化绑定的使用场景

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

// 新写法：通常无需复制字符，同时接受 string、字面量、char* 等；调用方必须保证底层字符仍有效
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

# 先判断所有权，再选词汇类型

| 类型 | 是否拥有值 | 关键边界 |
| --- | --- | --- |
| `optional<T>` | 内嵌一个 `T` 或空状态 | 体积至少能容纳 `T`；optional 本身不为所含对象单独动态分配 |
| `variant<Ts...>` | 内嵌其中一种备选 | 访问需匹配类型；异常情况下可能 `valueless_by_exception` |
| `string_view` | 不拥有字符 | 底层字符必须存活且未失效；不保证 `\0` 结尾 |
| `span<T>` | 不拥有连续元素 | 容器扩容、销毁后视图失效 |

`optional` 适合“可能没有结果”，但不能同时表达多种失败原因；需要诊断时使用 `expected<T,E>`（C++23）或领域结果类型。大对象是否按值存入应根据复制、移动和对象布局决定，而不是笼统说“不能用 optional”。

`variant` 是封闭集合的 tagged union。visitor 应覆盖所有备选，新增类型能触发编译期审查。不要用 `variant<any,string>` 之类逃避领域建模；`any` 丢失封闭集合与穷尽处理能力。

`string_view` 作为参数很常用，但返回值尤其危险：不能返回局部 `string` 的视图，也不能在原 `string` 扩容后继续使用。传给要求 C 字符串的 API 前必须确认终止符或构造拥有的 `string`。

结构化绑定默认 `auto [x,y]` 产生按值绑定；需要修改原对象用 `auto&`，只读借用用 `const auto&`。在 map 遍历中误用按值会复制 key/value。

参考：[C++ reference library](https://en.cppreference.com/w/cpp/utility.html)。
