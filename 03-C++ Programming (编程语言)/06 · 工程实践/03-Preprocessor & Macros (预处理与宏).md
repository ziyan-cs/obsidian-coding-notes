> **核心考点**：预处理指令、宏的陷阱、条件编译、与 constexpr/模板的取舍

## 预处理指令概览

```cpp
// 文件包含
#include <header>    // 标准库路径
#include "header"    // 用户路径（先搜索当前目录）

// 宏定义
#define PI 3.14159
#define SQUARE(x) ((x)*(x))    // 带参宏

// 条件编译
#ifdef DEBUG
    #define LOG(msg) std::cerr << msg
#else
    #define LOG(msg)
#endif

#if __cplusplus >= 201703L
    // C++17 特性可用
#endif

// 其他
#pragma once         // 头文件守卫
#undef PI            // 取消宏定义
#line 1 "newfile"    // 重置行号和文件名
#error "message"     // 编译错误
```

## 宏的陷阱（务必注意）

```cpp
// ❌ 问题 1：运算符优先级
#define DOUBLE(x) x+x
int result = DOUBLE(2) * 3;  // 期望 12，实际 2+2*3 = 8

// ✅ 每个参数加括号，整个表达式加括号
#define DOUBLE(x) ((x)+(x))

// ❌ 问题 2：多次求值
#define MAX(a,b) ((a) > (b) ? (a) : (b))
int x = 1, y = 2;
int m = MAX(++x, y);  // → ((++x) > (y) ? (++x) : (y))
                       // → x 被递增了两次！

// ✅ C++ 中用模板或 std::max 替代
template<typename T>
const T& max(const T& a, const T& b) { return a > b ? a : b; }

// ❌ 问题 3：分号
#define REQUIRE(cond) if (!(cond)) return false
REQUIRE(x > 0);  // 展开：if (!(x>0)) return false;
                  // 后面再接 else 会出问题

// ✅ 用 do { } while(0) 包裹多语句宏
#define REQUIRE(cond) do { if (!(cond)) return false; } while(0)
```

## 条件编译的典型用途

```cpp
// 调试日志
#ifndef NDEBUG
    #define DBG_LOG(msg) std::cerr << __FILE__ << ":" << __LINE__ \
                                    << " " << msg << std::endl
#else
    #define DBG_LOG(msg)
#endif

// 平台适配
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
#endif

// 编译器检测
#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

// 禁用拷贝
#define DISABLE_COPY(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;
```

## 常用预定义宏

```cpp
__FILE__             // 当前文件名
__LINE__             // 当前行号
__func__             // 当前函数名（C++11 起）
__DATE__             // 编译日期
__TIME__             // 编译时间
__cplusplus          // C++ 标准版本（199711L, 201103L, 201703L, 202002L）

// 断言
assert(ptr != nullptr);         // 运行时断言（NDEBUG 时消除）
static_assert(sizeof(int) == 4, "int must be 4 bytes");  // 编译期断言
```

## 宏 vs C++ 特性

| 目的 | 宏 | C++ 替代 |
|------|-----|---------|
| 常量定义 | `#define PI 3.14` | `constexpr double PI = 3.14;` |
| 类型别名 | `#define BYTE unsigned char` | `using Byte = unsigned char;` |
| 函数式宏 | `#define MIN(a,b) ...` | `std::min` template |
| 条件编译 | `#ifdef DEBUG` | 无可替代（宏仍有此用途）|
| 头文件守卫 | `#ifndef...#define` | `#pragma once`（非标准但广泛支持）|

> **工程建议**：在 C++ 中**尽量减少宏的使用**。宏不遵循 C++ 的作用域规则（全局替换），不参与符号解析，调试困难。唯一无法替代宏的场景是：**条件编译**（`#ifdef`）和 **获取 `__FILE__`/`__LINE__`**（如 `assert` 宏）。
