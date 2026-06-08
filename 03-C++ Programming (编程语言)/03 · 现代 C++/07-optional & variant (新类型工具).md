---
tags:
  - cpp
  - modern-cpp
---

> **核心考点**：optional、variant、any 三种新型类型工具的适用场景

## std::optional（C++17）

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

## std::variant（C++17）

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

### 用 variant 实现错误处理

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

optional 与 variant 是现代 C++ 新增的重要类型工具，其他特性详见 → [Modern C++ Overview (现代 C++ 特性总览)](/03-C++%20Programming%20(编程语言)/03%20·%20现代%20C++/00-Modern%20C++%20Overview%20(现代%20C++%20特性总览).md)
