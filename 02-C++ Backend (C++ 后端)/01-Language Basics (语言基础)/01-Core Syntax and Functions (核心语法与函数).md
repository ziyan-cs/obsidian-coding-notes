---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 01-Core Syntax and Functions (核心语法与函数)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 01-Variables, Types & Operators (变量、类型与运算符)

****
> [!abstract] 核心考点：C++ 基本类型的大小与范围、类型转换规则、const 与引用

## 基本类型与大小

```cpp
// 整型（大小与平台相关的，以下为 64 位 Linux 典型值）
bool        b  = true;          // 1 字节
char        c  = 'A';           // 1 字节，有符号或无符号（实现定义）
short       s  = 100;           // 2 字节
int         i  = 42;            // 4 字节
long        l  = 100L;          // 8 字节（Linux 64位）
long long   ll = 100LL;         // 8 字节

// 浮点
float       f  = 3.14f;         // 4 字节，约 7 位有效数字
double      d  = 3.14;          // 8 字节，约 15 位有效数字
long double ld = 3.14L;         // 16 字节（x86-64）

// 固定宽度整型（推荐在跨平台代码中使用）
#include <cstdint>
int8_t  / uint8_t
int32_t / uint32_t
int64_t / uint64_t

// 查询大小
sizeof(int);      // 4
sizeof(double);   // 8
```

## 字面量

```cpp
42        // int
42u       // unsigned int
42L       // long
42LL      // long long
42ULL     // unsigned long long
0x2A      // 十六进制 = 42
052       // 八进制  = 42
0b101010  // 二进制  = 42（C++14）
1'000'000 // 数字分隔符（C++14），等于 1000000
3.14f     // float
3.14      // double
'A'       // char（ASCII 65）
"hello"   // const char*
u"hello"  // char16_t*（UTF-16）
U"hello"  // char32_t*（UTF-32）
R"(raw\nstring)"  // 原始字符串，\n 不转义
```

## 运算符优先级（易混淆部分）

```cpp
// 位运算优先级低于比较运算符！
if (x & 0xFF == 0)     // 错误：等价于 x & (0xFF == 0) = x & 0
if ((x & 0xFF) == 0)   // 正确

// 前置 vs 后置 ++
int a = 5;
int b = a++;   // b=5, a=6（后置：先用再加）
int c = ++a;   // c=7, a=7（前置：先加再用）

// 短路求值
false && foo();   // foo() 不会被调用
true  || foo();   // foo() 不会被调用

// 三目运算符
int max = (a > b) ? a : b;

// 逗号运算符（返回最后一个表达式的值）
int x = (1, 2, 3);   // x = 3
```

## 整型提升与隐式转换

```cpp
char a = 200;
char b = 100;
int c = a + b;   // a、b 提升为 int 再运算，不溢出

unsigned int u = 10;
int          i = -1;
u + i;   // i 被转换为 unsigned，结果是巨大的正数！

// 算术转换层次（低 → 高自动转换）
// char → short → int → long → long long
//                          ↓
//                       float → double → long double
```

---

类型系统深化详见 → [Const, Typedef & Enum (类型系统基础)](/02-C++%20Backend%20(C++%20后端)/02-Core%20Mechanisms%20(核心机制)/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)

---

## 02-Control Flow (流程控制)

> [!abstract] 核心考点：条件分支（if/switch）、循环（for/while/do-while）、break/continue 控制流

```cpp
// if-else
if (x > 0) { ... }
else if (x < 0) { ... }
else { ... }

// C++17：if 初始化语句
if (auto it = map.find(key); it != map.end()) {
    use(it->second);
}

// switch（只能用整型/枚举，注意 fallthrough）
switch (c) {
    case 'a': case 'e': case 'i':
        std::cout << "vowel\n"; break;
    case 'b': case 'c':
        std::cout << "consonant\n"; break;
    default:
        break;
}

// for
for (int i = 0; i < n; i++) { ... }
for (auto& x : container) { ... }   // 范围 for（C++11）

// while / do-while
while (cond) { ... }
do { ... } while (cond);

// break / continue / goto（goto 只在状态机或跳出多层循环时考虑）
```

---

函数设计中的控制流应用详见 → [Functions (函数)](/02-C++%20Backend%20(C++%20后端)/01-Basic%20Syntax%20(基础语法)/03-Functions%20(函数).md)

---

## 03-Functions (函数)

> [!abstract] 核心考点：函数重载、默认参数、值传递/引用传递、inline 函数

```cpp
// 函数重载（编译期，根据参数类型/数量区分）
int    abs(int x)    { return x < 0 ? -x : x; }
double abs(double x) { return x < 0 ? -x : x; }

// 默认参数（只能在声明中写，且必须从右向左）
void log(std::string msg, int level = 1, bool flush = false);

// 内联函数（建议编译器内联展开，消除函数调用开销）
inline int square(int x) { return x * x; }

// 函数指针
int (*fp)(int, int) = add;   // 指向 add 函数
fp(1, 2);                    // 调用

// 可变参数（C++11 variadic template，取代 C 的 va_list）
template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << '\n';  // 折叠表达式（C++17）
}

// 递归：注意栈溢出，深度过大改用迭代或尾递归
int fib(int n) { return n <= 1 ? n : fib(n-1) + fib(n-2); }
```

---

变量与类型系统是函数的基础，详见 → [Variables, Types & Operators (变量、类型与运算符)](/02-C++%20Backend%20(C++%20后端)/01-Basic%20Syntax%20(基础语法)/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)