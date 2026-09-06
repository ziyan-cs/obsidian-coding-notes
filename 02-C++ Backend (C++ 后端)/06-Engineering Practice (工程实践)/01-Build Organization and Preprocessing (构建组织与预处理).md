---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Compilation & Linking (编译与链接)

> [!note] 本节重点：核心考点：编译的四个阶段、符号解析与重定位、静态链接 vs 动态链接、常见链接错误

# 编译的四个阶段

```text
Compilation Pipeline:

source.cpp  ──→  source.ii  ──→  source.s  ──→  source.o  ──→  a.out
(preprocessing)  (compilation)  (assembly)    (linking)    (executable)
    │                                                                  ▲
    │ g++ -E                        g++ -S      g++ -c       ld       │
    ↓                                                                  │
(expand macros,                                                  ┌─────┴──────┐
 include headers)                                                │  libfoo.a  │
                                                                 │  libbar.so │
                                                                 └────────────┘
```

```bash
g++ -E main.cpp -o main.i    # 预处理
g++ -S main.i -o main.s       # 编译到汇编
g++ -c main.s -o main.o       # 汇编到目标文件
g++ main.o -o main            # 链接
g++ main.cpp -o main
```

# 目标文件的节区（Section）

```text
ELF 目标文件结构：
┌──────────────┐
│ ELF Header   │  文件头（魔数、入口地址、节区表位置）
├──────────────┤
│ .text        │  代码段（只读，机器指令）
├──────────────┤
│ .rodata      │  只读数据（字符串常量、const 变量）
├──────────────┤
│ .data        │  已初始化全局/静态变量
├──────────────┤
│ .bss         │  未初始化全局/静态变量（不占文件空间）
├──────────────┤
│ .symtab      │  符号表（函数名、全局变量名）
├──────────────┤
│ .rel.text    │  重定位表（需要修改的地址）
├──────────────┤
│ .debug       │  调试信息（-g 时生成）
└──────────────┘
```

# 符号解析与重定位

```cpp
// a.cpp
extern int global;       // 引用外部符号
void foo();              // 引用外部符号
int main() {
    foo();
    return global;
}

// b.cpp
int global = 42;         // 定义符号
void foo() { /* ... */ } // 定义符号
```

**链接过程**：
1. **符号解析**：收集所有目标文件的符号表，将引用与定义匹配
2. **重定位**：把符号引用替换为实际地址

**常见链接错误**：

```text
// 未定义引用（undefined reference）
undefined reference to `foo()'
→ 忘记链接 foo 所在的目标文件或库

// 多重定义（multiple definition）
multiple definition of `global'
→ 在头文件中定义了全局变量（应在 .h 中 extern，.cpp 中定义）

// 解决：头文件中应只声明，不定义
// ❌ common.h: int counter = 0;
// ✅ common.h: extern int counter;
// ✅ common.cpp: int counter = 0;
```

# 静态链接 vs 动态链接

| | 静态链接 (.a) | 动态链接 (.so / .dll) |
|--|-------------|-------------------|
| 链接时机 | 编译时 | 运行时（加载时链接）|
| 可执行文件大小 | 大（包含库代码） | 小（只记录依赖）|
| 内存占用 | 不同进程各自一份 | **共享**同一份 .so |
| 更新库 | 需重新链接 | 替换 .so 即可 |
| 部署 | 无外部依赖 | 需确保 .so 存在 |
| 启动速度 | 取决于二进制大小、加载器与系统缓存 | 取决于依赖数量、加载器与系统缓存 |
| 性能 | 取决于编译优化、调用边界与实际工作负载 | 取决于符号解析、调用边界与实际工作负载 |

```bash
g++ -c lib.cpp -o lib.o
ar rcs libfoo.a lib.o
g++ main.cpp -L. -lfoo -o main

g++ -fPIC -shared lib.cpp -o libfoo.so
g++ main.cpp -L. -lfoo -o main
```

# 动态链接的细节：PLT & GOT

```text
调用共享库函数时的跳转流程：
main() 调用 foo():
  → call foo@PLT      (PLT: 过程链接表)
    → jmp *(foo@GOT)  (GOT: 全局偏移表)
      → 首次: 跳转到动态链接器，解析 foo 地址，更新 GOT
      → 之后: 直接跳转到 foo
```

**延迟绑定（Lazy Binding）**：函数地址只在第一次调用时才解析，提高启动速度。

# 工程最佳实践

```cpp
// ✅ 头文件守卫
#pragma once  // 或 #ifndef...#define...#endif

// ✅ 尽量减少头文件依赖（前向声明代替 #include）
class Foo;  // 前向声明，在头文件中只需要指针/引用时使用

// ✅ 内联函数放在头文件中
inline int square(int x) { return x * x; }

// ❌ 不要在头文件中定义全局变量
// ❌ 不要在两个 .cpp 中定义同名全局函数
// ❌ 不要在头文件中 using namespace std;
```

> **面试重点**：**声明 vs 定义**的区别——声明引入名字，定义提供实体（对对象而言通常也提供存储）。头文件通常放声明；需要放在头文件的 inline 函数、模板、`inline` 变量等是例外。避免在普通头文件定义具有外部链接的全局变量。

# 30 秒回答

编译把源文件分别变成目标文件，链接器再解析跨文件符号并重定位地址。`undefined reference` 先查“声明有了但定义/库没有参与链接”，`multiple definition` 先查“同一外部符号被定义多次”。静态/动态链接是部署、更新、隔离与启动成本的权衡，不能只用“谁更快”概括。

---

头文件与源文件组织详见 → [Header & Source Organization (头文件与源文件组织)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/02-Header%20&%20Source%20Organization%20(头文件与源文件组织).md)

---

# Header & Source Organization (头文件与源文件组织)

> [!note] 本节重点：核心考点：头文件职责、源文件职责、include 顺序、模块化设计

# 头文件职责

```cpp
// foo.h — 接口声明
#pragma once

#include <string>  // 必要的标准库
#include <memory>  // 前向声明不足以替代时才 include

// ✅ 前向声明减少依赖
class Bar;  // 只需要指针/引用时不要 #include "Bar.h"

class Foo {
public:
    explicit Foo(std::string name);
    ~Foo();
    
    void process(const Bar& bar);  // 引用，只需要前向声明
    std::string name() const;

private:
    struct Impl;                    // Pimpl 惯用法（不透明指针）
    std::unique_ptr<Impl> pImpl_;
};
```

**头文件放什么**：
- 函数声明（非 inline 函数）
- 类定义
- inline 函数/模板定义
- const/constexpr 常量
- extern 声明

# 源文件职责

```cpp
// foo.cpp — 实现
#include "foo.h"     // 首先包含自己的头文件（检查接口是否自洽）
#include "bar.h"     // 其他依赖
#include <iostream>  // 标准库

struct Foo::Impl {   // Pimpl 实现
    std::string name_;
};

Foo::Foo(std::string name) : pImpl_(std::make_unique<Impl>()) {
    pImpl_->name_ = std::move(name);
}

Foo::~Foo() = default;  // 必须在此处定义（Impl 完整类型）
```

# Include 顺序规范

```cpp
// Google C++ Style Guide 推荐顺序：
#include "foo.h"        // 1. 关联头文件（检查自洽性）
#include <vector>       // 2. C++ 标准库
#include <string.h>     // 3. C 标准库
#include "bar.h"        // 4. 项目内其他模块
```

**为什么关联头文件放在第一个**：
如果 `foo.h` 缺少某个 `#include`，编译 `foo.cpp` 时第一个报错，而不是在其他文件中报出难以定位的错误。

# Forward Declaration vs Include

```cpp
// ✅ 只需要前向声明：
class Bar;              // 声明但不定义
void func(Bar bar);     // 传值？需要完整类型！（调用拷贝构造）
void func(Bar* bar);    // 指针：前向声明就够
void func(Bar& bar);    // 引用：前向声明就够
Bar* createBar();       // 返回指针：前向声明就够

// ❌ 需要完整类型（需要 #include）：
Bar bar;                // 创建对象
bar.someMethod();       // 调用成员函数
sizeof(Bar);            // 获取大小
```

# 模块化组织

```text
project/
├── include/             # 公共头文件（给外部用）
│   └── module/
│       ├── public_a.h
│       └── public_b.h
├── src/                 # 实现 + 私有头文件
│   ├── module/
│   │   ├── impl.cpp
│   │   └── internal.h   # 模块内私有的头文件
│   └── main.cpp
└── test/                # 测试
    └── module/
        └── test_impl.cpp
```

# 常见陷阱

```cpp
// ❌ 循环 include（A.h include B.h, B.h include A.h）
// → 用前向声明打破循环

// ❌ include 爆炸（间接包含大量头文件）
// → 使用前向声明 / Pimpl 惯用法

// ❌ 在头文件中写 using namespace std;
// → 污染所有包含者的命名空间

// ❌ 在头文件中定义非内联函数
// → 多个 .cpp 包含该头文件 → multiple definition 错误
```

> [!tip]- **工程要点**：编译时间是大型 C++ 项目的重要成本。头文件之间的依赖关系直接影响增量编译速度。优先用 **前向声明**，其次用 **Pimpl 惯用法**（将实现细节对用户隐藏），最后才考虑重构成模块。

---

编译与链接过程详见 → [Compilation & Linking (编译与链接)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/01-Compilation%20&%20Linking%20(编译与链接)%20⭐.md)

---

# Preprocessor & Macros (预处理与宏)

> [!note] 本节重点：核心考点：预处理指令、宏的陷阱、条件编译、与 constexpr/模板的取舍

# 预处理指令概览

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

# 宏的陷阱（务必注意）

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

# 条件编译的典型用途

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

# 常用预定义宏

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

# 宏 vs C++ 特性

| 目的 | 宏 | C++ 替代 |
|------|-----|---------|
| 常量定义 | `#define PI 3.14` | `constexpr double PI = 3.14;` |
| 类型别名 | `#define BYTE unsigned char` | `using Byte = unsigned char;` |
| 函数式宏 | `#define MIN(a,b) ...` | `std::min` template |
| 条件编译 | `#ifdef DEBUG` | 无可替代（宏仍有此用途）|
| 头文件守卫 | `#ifndef...#define` | `#pragma once`（非标准但广泛支持）|

> **工程建议**：在 C++ 中**尽量减少宏的使用**。宏不遵循 C++ 的作用域规则（全局替换），不参与符号解析，调试困难。唯一无法替代宏的场景是：**条件编译**（`#ifdef`）和 **获取 `__FILE__`/`__LINE__`**（如 `assert` 宏）。

---

编译过程中预处理阶段详见 → [Compilation & Linking (编译与链接)](/02-C++%20Backend%20(C++%20后端)/06-Engineering%20Practice%20(工程实践)/01-Compilation%20&%20Linking%20(编译与链接)%20⭐.md)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Build Organization and Preprocessing (构建组织与预处理)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
