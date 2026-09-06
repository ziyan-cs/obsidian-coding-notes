---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Type System Basics (类型系统基础)

> [!note] 本节重点：核心考点：const 的多种用法、typedef/using 类型别名、enum 与 enum class 区别

# const

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

# typedef & using

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

# enum & enum class

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

---

类型转换规则详见 → [Type Conversion & Casting (类型转换)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/02-Type%20Conversion%20&%20Casting%20(类型转换).md)

---

# Type Conversion and Casting (类型转换)

> [!note] 本节重点：核心考点：四种命名的 C++ 类型转换（static/dynamic/const/reinterpret）、隐式转换规则

# 四种命名转换（Named Casts）

## static_cast

编译期类型转换，用于相关类型之间的安全转换：

```cpp
double d = 3.14;
int i = static_cast<int>(d);         // 截断小数

// 父子类指针转换（下行转换不安全，没有运行时检查）
Base* b = new Derived();
Derived* d = static_cast<Derived*>(b);  // 需确保 b 确实指向 Derived

// 枚举 ↔ 整型
int n = static_cast<int>(Direction::NORTH);
```

## dynamic_cast

运行时类型检查（RTTI），用于多态类的安全下行转换：

```cpp
Base* b = new Derived();
Derived* d = dynamic_cast<Derived*>(b);   // 成功返回指针，失败返回 nullptr
if (d) { d->derivedMethod(); }

// 引用版本：失败抛 std::bad_cast
try {
    Derived& dr = dynamic_cast<Derived&>(*b);
} catch (const std::bad_cast&) { ... }

// 要求：基类必须有至少一个虚函数（才有 RTTI 信息）
```

## const_cast

添加或移除 `const`，是唯一能移除 const 的转换：

```cpp
const char* cs = "hello";
char* s = const_cast<char*>(cs);   // 危险！修改字符串字面量是未定义行为

// 合法场景：函数参数是 const，但确知底层对象非 const
void legacyFunc(char* p);
void wrapper(const char* p) {
    legacyFunc(const_cast<char*>(p));  // 若 p 指向非 const 对象则安全
}
```

## reinterpret_cast

重新解释内存，最危险，几乎不带任何转换：

```cpp
int x = 42;
int* p = &x;
char* cp = reinterpret_cast<char*>(p);  // 按字节访问 int 的内存

// 序列化/反序列化、与硬件寄存器交互时使用
uint64_t addr = reinterpret_cast<uint64_t>(p);

// 函数指针转换（某些插件/JIT 场景）
```

# 转换选择原则

```
需要运行时安全检查（多态向下转型）  → dynamic_cast
数值类型转换 / 亲缘类型            → static_cast
只去掉 const                       → const_cast
纯内存重新解释                     → reinterpret_cast
永远不用 C 风格 (int)x             → 代码难维护、无类型安全
```

---

指针与引用的转换操作详见 → [Pointers & References In Depth (指针与引用深入)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/03-Pointers%20&%20References%20In%20Depth%20(指针与引用深入)%20⭐.md)

---

# Pointers and References (指针与引用)

> [!note] 本节重点：核心考点：指针与引用的本质区别、函数指针、智能指针底层原理的关系

> [!warning] 地址运算必须受对象边界约束
> 指针算术只在同一数组对象（含末尾后一位置）范围内才有定义；“指针就是整数地址”是有用的直觉，但不是可以随意加减、转换和解引用的许可证。

# 指针详解

```cpp
int  x = 42;
int* p = &x;      // p 存储 x 的地址
*p = 100;         // 解引用，修改 x
p++;              // 仅在同一数组范围内才可安全移动到下一个元素

// 空指针
int* null1 = nullptr;   // C++11 推荐（类型安全）
// int* null2 = NULL;   // C 风格，NULL = 0，可能与 int 重载混淆
// int* null3 = 0;      // 同上

// 指针 vs 数组
int arr[] = {1,2,3};
int* p = arr;     // 数组名退化为首元素指针
*(p + 1) == arr[1];      // true，等价
p[2]     == *(p + 2);    // true

// 函数指针
void (*fp)(int) = &myFunc;
(*fp)(42);   // 或直接 fp(42)
```

# 引用详解

```cpp
int x = 42;
int& ref = x;    // 引用：ref 是 x 的别名，必须初始化，不能重新绑定
ref = 100;       // 修改 x

// 引用不是独立对象；对 ref 取地址得到所绑定对象的地址
// 引用常被实现为指针，但这是实现细节，不能据此推导语言规则
// 不能有引用的引用、引用的指针、引用的数组

// 常量引用：可以绑定临时对象（延长其生命周期）
const int& cr = 42;   // 临时 int 对象生命周期延长至 cr 的作用域

// 右值引用（见移动语义章节）
int&& rr = std::move(x);
```

# 指针 vs 引用

| |指针|引用|
|---|---|---|
|可为空|✅（nullptr）|❌（必须绑定有效对象）|
|可重新指向|✅|❌（一旦绑定不可改）|
|需要解引用|`*p`|直接用|
|可做算术|✅|❌|
|传参惯用法|可为 null 或需要算术|不为 null 且不需要重新绑定|

# 常见内存错误

```cpp
// 1. 悬空指针（Dangling Pointer）
int* p = new int(42);
delete p;
*p = 100;   // 未定义行为！p 已悬空

// 2. 野指针（未初始化）
int* p;     // p 指向随机地址
*p = 1;     // 未定义行为

// 3. 双重释放
delete p;
delete p;   // 未定义行为

// 4. 内存泄漏
int* p = new int(42);
// 忘记 delete p;

// 解决：智能指针
auto p = std::make_unique<int>(42);   // 自动管理生命周期
```

---

指针类型转换详见 → [Type Conversion & Casting (类型转换)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/02-Type%20Conversion%20&%20Casting%20(类型转换).md)

# 30 秒回答

指针是可为空、可重新指向的对象，适合表达可选对象、数组遍历或低层接口；引用是已绑定对象的别名，适合表达“这里必须有一个有效对象”的参数契约。两者都不管理生命周期：裸指针/引用指向的对象是否还活着，仍由所有权模型决定。优先用值、RAII 容器和智能指针表达所有权。

# 自测

1. 为什么 `int* p = arr; ++p` 与对任意对象地址做 `++p` 的安全性不同？
2. `&ref` 得到的是什么？这能否证明引用本身是一个独立对象？
3. 何时函数参数该使用 `T*`，何时使用 `T&` 或 `const T&`？

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 复述

- 不看正文，说明 01-Types Pointers and References (类型指针与引用) 的问题、核心机制与边界。

## 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

## 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？
