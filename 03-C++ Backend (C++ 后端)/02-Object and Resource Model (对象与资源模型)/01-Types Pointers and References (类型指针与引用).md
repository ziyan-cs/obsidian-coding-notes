---
study_stage: D3
review_due: 2026-10-18
---

> [!abstract] 学习目标
> 本篇依次回答三个问题：怎样用 `const`、类型别名和枚举表达接口约束；怎样选择显式类型转换；怎样用指针和引用表达借用关系。指针和引用通常不表达所有权；引用直接绑定临时对象时有特殊的生命周期延长规则。

## 类型声明与约束

### const

```cpp
#include <string>

const int x = 42;        // 不可修改
int const y = 42;         // 同上，等价写法

// 指针与 const（从右往左读）
int z = 7;
int* const p1 = &z;       // 常量指针：指针本身不可改，可经 p1 修改 z
const int* p2 = &x;       // 指向常量的指针：指针可改，指向的值不可改
const int* const p3 = &x; // 双 const：指针和值都不可改

// 函数中的 const
void foo(const std::string& s);   // 传常量引用，避免拷贝且不可修改
struct Example {
    int getValue() const { return value_; } // const 成员函数不修改可观察状态
    mutable int cache_ = 0;                 // 允许修改缓存等逻辑常量状态
private:
    int value_ = 0;
};
```

### typedef 与 using

```cpp
#include <vector>

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

### enum 与 enum class

```cpp
// 传统 enum：值会污染外围作用域，隐式转换为 int
enum Color { RED, GREEN, BLUE };   // RED=0, GREEN=1, BLUE=2
int x = RED;                       // 隐式转为 int

// enum class（C++11，强类型枚举，推荐）
enum class Direction { NORTH, SOUTH, EAST, WEST };
Direction d = Direction::NORTH;     // 必须加作用域
// int x = d;   // 错误！不隐式转换

// 精确宽度整数类型来自 <cstdint>；std::uint8_t 在少数实现上可能不存在
#include <cstdint>
enum class Status : std::uint8_t { OK = 0, ERROR = 1, TIMEOUT = 2 };

// 使用
switch (d) {
    case Direction::NORTH: break;
    case Direction::SOUTH: break;
    default: break;
}
```

## 类型转换与转换选择

### static_cast

静态检查转换形式，但不保证运行时值域或对象动态类型安全：

```cpp
struct Base {};
struct Derived : Base {};
enum class Direction { north };

void convert() {
    double d = 3.14;
    int i = static_cast<int>(d); // 截断小数部分

    // 父类指针向下转型：没有运行时检查，调用前必须有可靠证明
    Derived owned;
    Base* base = &owned;
    Derived* downcast = static_cast<Derived*>(base);

    int n = static_cast<int>(Direction::north);
}
```

### dynamic_cast

运行时类型检查（RTTI），常用于多态对象的安全下行转换或交叉转换：

```cpp
#include <typeinfo>

struct Base { virtual ~Base() = default; };
struct Derived : Base {
    void derivedMethod() {}
};

void inspect(Base* base) {
    // 指针形式：检查失败返回 nullptr
    if (auto* checked = dynamic_cast<Derived*>(base)) {
        checked->derivedMethod();
    }

    // 引用形式：检查失败抛 std::bad_cast；先确保 base 非空，避免空指针解引用。
    if (base) {
        try {
            Derived& checked = dynamic_cast<Derived&>(*base);
            checked.derivedMethod();
        } catch (const std::bad_cast&) {
            // 处理动态类型不匹配
        }
    }
}

// 运行时下行或交叉检查要求源类型是多态类（至少有一个虚函数）。
// 向上转型通常直接隐式转换，不需要 dynamic_cast。
```

### const_cast

在四种命名转换中，只有 `const_cast` 用于调整 cv 限定。去掉 `const` 不会让原本的常量对象变成可修改对象：

```cpp
void adjust_cv() {
    const char* cs = "hello";
    char* s = const_cast<char*>(cs);
    // *s = 'H'; // 未定义行为：字符串字面量不可修改

    // 原对象本身不是 const，通过去限定后的别名写入才有定义。
    int value = 3;
    const int& read_only_view = value;
    int& writable = const_cast<int&>(read_only_view);
    writable = 4;
}
```

### reinterpret_cast

执行低层显式转换，但不负责创建对象、不保证对齐，也不会豁免别名和对象生命周期规则：

```cpp
int x = 42;
int* p = &x;
char* cp = reinterpret_cast<char*>(p);  // 按字节访问 int 的内存

// 通过 char* 观察对象表示时仍须遵守对象边界和生命周期规则。
// 不要假定指针总能无损装入 uint64_t 或转换后即可任意解引用。

// 函数指针转换（某些插件/JIT 场景）
```

### 转换选择

| 需求 | 首选 | 需要留意 |
| --- | --- | --- |
| 数值转换、明确的基类/派生类转换、枚举与整数转换 | `static_cast` | 语法可行不等于值域或动态类型一定安全；向下转型前必须有可靠证明 |
| 多态对象的运行时下行或交叉检查 | `dynamic_cast` | 检查失败需处理空指针或 `std::bad_cast` |
| 调整 cv 限定 | `const_cast` | 只有原对象本身可修改时才可通过结果写入 |
| 平台接口、底层对象表示等低层互操作 | `reinterpret_cast` | 不保证可移植性、对齐、对象存在或可解引用 |
| 旧式 C 风格转换 | 尽量避免 | 转换意图不清晰，审查时难看出是否涉及去限定或低层重解释 |

## 指针与引用

> [!warning] 地址运算必须受对象边界约束
> 指针算术只在同一数组对象（含末尾后一位置）范围内才有定义；“指针就是整数地址”是有用的直觉，但不是可以随意加减、转换和解引用的许可证。

### 指针

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

### 引用

```cpp
int x = 42;
int& ref = x;    // 引用：ref 是 x 的别名，必须初始化，不能重新绑定
ref = 100;       // 修改 x

// 对 ref 取地址得到被引用对象的地址；引用不是可重新绑定的指针对象。
// 不能声明指向引用的指针或引用数组；由类型别名/模板形成的引用会发生引用折叠。
using IntRef = int&;
IntRef& same_ref = x; // 折叠后仍为 int&

// 常量引用：可以绑定临时对象（延长其生命周期）
const int& cr = 42;   // 临时 int 对象生命周期延长至 cr 的作用域

// 右值引用（见移动语义章节）：std::move 只转换表达式类别，后续操作才可能实际移动。
int&& rr = std::move(x);
```

局部 `const T&` 直接绑定到临时对象时，临时对象生命周期延长到该引用的生命周期。临时对象绑定到函数引用形参时，只保证存活到包含函数调用的完整表达式结束，不能把它保存下来供调用后使用。

### 指针与引用如何选择

| 判断维度 | 指针 | 引用 |
| --- | --- | --- |
| 可为空 | 可以，使用 `nullptr` 并检查 | 没有空引用语义，但仍可能悬垂 |
| 可重新绑定 | 可以改为指向其他对象 | 初始化后绑定不变 |
| 使用方式 | 需通过 `*p` 或 `p->` 访问 | 像原对象一样直接使用 |
| 地址运算 | 仅在同一数组范围内；尾后一位不可解引用 | 不支持 |
| 参数含义 | 允许缺省、可选或需要地址运算 | 调用方必须提供有效对象 |

### 常见内存错误

```cpp
#include <memory>

int main() {
// 以下危险操作均刻意注释：取消注释后才会触发对应错误。
{
    int* dangling = new int(42);
    delete dangling;
    // *dangling = 100;  // 未定义行为：对象生命周期已结束
}

{
    int* uninitialized; // 值不确定
    // *uninitialized = 1; // 未定义行为：先读取了不确定的指针值
}

{
    int* released = new int(42);
    delete released;
    // delete released; // 未定义行为：重复释放
}

// 推荐：让 RAII 类型管理所有权与释放。
auto owned = std::make_unique<int>(42);
}
```

> [!summary] 核心摘要
>
> 类型声明表达约束，命名转换表达意图，指针和引用表达借用。指针可为空、可重新指向；引用绑定后不变，但二者都不能替代所有权设计。访问借用对象前要确认其生命周期仍有效；资源所有权优先交给值类型、RAII 容器或智能指针。

> [!question]- 自测：先回答再展开
> 1. 指向一个 `int` 对象时，`++p` 可以得到什么位置？为什么尾后一位不能解引用，也不能再继续递增？
> 2. `&ref` 得到的是什么？这能否证明引用本身是一个独立对象？
> 3. 何时函数参数该使用 `T*`，何时使用 `T&` 或 `const T&`？

> [!info]- 延伸阅读
> - 下一步：[02-Memory Layout and Allocation (内存布局与分配)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/02-Object%20and%20Resource%20Model%20(对象与资源模型)/02-Memory%20Layout%20and%20Allocation%20(内存布局与分配).md)
> - 标准草案：[指针算术](https://eel.is/c++draft/expr.add)、[dynamic_cast](https://eel.is/c++draft/expr.dynamic.cast)、[引用与引用折叠](https://eel.is/c++draft/dcl.ref)、[临时对象生命周期](https://eel.is/c++draft/class.temporary)、[const_cast](https://eel.is/c++draft/expr.const.cast)
