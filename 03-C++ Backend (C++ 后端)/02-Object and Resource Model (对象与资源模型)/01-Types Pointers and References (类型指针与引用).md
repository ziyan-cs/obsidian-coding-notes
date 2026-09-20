---
study_stage: learn
review_due: 2026-09-24
---


# Type System Basics (类型系统基础)

> [!note] 本节重点：const 的多种用法、typedef/using 类型别名、enum 与 enum class 区别

## const

```cpp
const int x = 42;        // 不可修改
int const y = 42;         // 同上，等价写法

// 指针与 const（从右往左读）
int z = 7;
int* const p1 = &z;       // 常量指针：指针本身不可改，可经 p1 修改 z
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

---

# Type Conversion and Casting (类型转换)

> [!note] 本节重点：四种命名的 C++ 类型转换（static/dynamic/const/reinterpret）、隐式转换规则

## 四种命名转换（Named Casts）

### static_cast

静态检查转换形式，但不保证运行时值域或对象动态类型安全：

```cpp
double d = 3.14;
int i = static_cast<int>(d);         // 截断小数

// 父子类指针转换（下行转换不安全，没有运行时检查）
Derived owned;
Base* base = &owned;
Derived* downcast = static_cast<Derived*>(base); // 需证明动态对象确为 Derived

// 枚举 ↔ 整型
int n = static_cast<int>(Direction::NORTH);
```

### dynamic_cast

运行时类型检查（RTTI），用于多态类的安全下行转换：

```cpp
Derived owned;
Base* base = &owned;
Derived* checked = dynamic_cast<Derived*>(base); // 失败返回 nullptr
if (checked) { checked->derivedMethod(); }

// 引用版本：失败抛 std::bad_cast
try {
Derived& dr = dynamic_cast<Derived&>(*base);
} catch (const std::bad_cast&) { ... }

// 要求：基类必须有至少一个虚函数（才有 RTTI 信息）
```

### const_cast

添加或移除 `const`，是唯一能移除 const 的转换：

```cpp
const char* cs = "hello";
char* s = const_cast<char*>(cs);   // 危险！修改字符串字面量是未定义行为

// 只有原对象本身可修改时，才可能安全地去 const 并写入。
int value = 3;
const int& read_only_view = value;
int& writable = const_cast<int&>(read_only_view);
writable = 4; // 修改的是原本非 const 的 value；不要据此设计普通接口
```

### reinterpret_cast

重新解释内存，最危险，几乎不带任何转换：

```cpp
int x = 42;
int* p = &x;
char* cp = reinterpret_cast<char*>(p);  // 按字节访问 int 的内存

// 观察对象表示时仍须遵守别名、对齐和生命周期规则。
// 不要假定指针总能无损装入 uint64_t 或转换后即可任意解引用。

// 函数指针转换（某些插件/JIT 场景）
```

## 转换选择原则

```
需要运行时安全检查（多态向下转型）  → dynamic_cast
数值类型转换 / 亲缘类型            → static_cast
只去掉 const                       → const_cast
纯内存重新解释                     → reinterpret_cast
永远不用 C 风格 (int)x             → 代码难维护、无类型安全
```

---

# Pointers and References (指针与引用)

> [!note] 本节重点：指针与引用的本质区别、函数指针、智能指针底层原理的关系

> [!warning] 地址运算必须受对象边界约束
> 指针算术只在同一数组对象（含末尾后一位置）范围内才有定义；“指针就是整数地址”是有用的直觉，但不是可以随意加减、转换和解引用的许可证。

## 指针详解

```cpp
int  x = 42;
int* p = &x;      // p 存储 x 的地址
*p = 100;         // 解引用，修改 x
// 对单个 int，p + 1 只能表示尾后一位置，不可解引用。

// 空指针
int* null1 = nullptr;   // C++11 推荐（类型安全）
// int* null2 = NULL;   // C 风格宏，具体展开由实现决定；重载决议可能混淆
// int* null3 = 0;      // 同上

// 指针 vs 数组
int arr[] = {1,2,3};
int* first = arr; // 此表达式中数组转为首元素指针
*(first + 1) == arr[1];   // true
first[2] == *(first + 2); // true

// 函数指针
void (*fp)(int) = &myFunc;
(*fp)(42);   // 或直接 fp(42)
```

## 引用详解

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

## 指针 vs 引用

| |指针|引用|
|---|---|---|
|可为空|✅（nullptr）|语言层面没有空引用；但绑定对象仍可能先于引用销毁，形成悬空引用|
|可重新指向|✅|❌（一旦绑定不可改）|
|需要解引用|`*p`|直接用|
|可做算术|✅|❌|
|传参惯用法|可为 null 或需要算术|不为 null 且不需要重新绑定|

## 常见内存错误

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


> [!summary] 核心摘要
>
> 指针是可为空、可重新指向的对象，适合表达可选对象、数组遍历或低层接口；引用是已绑定对象的别名，适合表达“这里必须有一个有效对象”的参数契约。两者都不管理生命周期：裸指针/引用指向的对象是否还活着，仍由所有权模型决定。优先用值、RAII 容器和智能指针表达所有权。

> [!question]- 自测：先回答再展开
> 1. 为什么 `int* p = arr; ++p` 与对任意对象地址做 `++p` 的安全性不同？
> 2. `&ref` 得到的是什么？这能否证明引用本身是一个独立对象？
> 3. 何时函数参数该使用 `T*`，何时使用 `T&` 或 `const T&`？

> [!info]- 延伸阅读
> - 下一步：[02-Memory Layout and Allocation (内存布局与分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)
