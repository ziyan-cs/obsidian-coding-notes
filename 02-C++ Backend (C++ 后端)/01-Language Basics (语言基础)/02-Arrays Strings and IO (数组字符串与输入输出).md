---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Arrays Strings and IO (数组字符串与输入输出)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 30 秒回答

**02-Arrays Strings and IO (数组字符串与输入输出)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## Array & String (数组与字符串)

> [!abstract] 核心考点：C 风格数组与指针的关系、std::string 的操作与性能、C 字符串函数

## 数组

```cpp
int arr[5] = {1, 2, 3, 4, 5};
int arr2[]  = {1, 2, 3};       // 大小自动推导 = 3
int mat[3][4] = {};             // 二维数组，零初始化

// 数组退化为指针（传参时丢失大小信息！）
void foo(int* arr, int n);      // 必须额外传大小

// 推荐：用 std::array（固定大小，不退化，有 size()）
#include <array>
std::array<int, 5> a = {1,2,3,4,5};
a.size();    // 5
a.at(2);     // 带越界检查
a.data();    // 获取裸指针

// std::vector（动态大小）
std::vector<int> v = {1,2,3};
v.push_back(4);
v.size();  v.capacity();
v.reserve(100);   // 预分配，避免多次扩容
v.shrink_to_fit();
```

## C 字符串 vs std::string

```cpp
// C 风格字符串（以 '\0' 结尾的 char 数组，避免使用）
char s[] = "hello";
strlen(s);  strcpy(dst, src);  strcat(dst, src);  strcmp(a, b);

// std::string（推荐）
#include <string>
std::string s = "hello";
s.size();   s.length();
s.empty();
s += " world";            // 拼接
s.substr(0, 5);           // 子串
s.find("world");          // 查找，返回下标或 std::string::npos
s.replace(6, 5, "C++");   // 替换
s.c_str();                // 转 const char*（生命周期与 s 绑定）

// 数值转换
std::to_string(42);       // int → string
std::stoi("42");          // string → int
std::stod("3.14");        // string → double

// string_view（C++17，零拷贝只读视图）
std::string_view sv = s;
```

---

数组类型的底层机制详见 → [Variables, Types & Operators (变量、类型与运算符)](/02-C++%20Backend%20(C++%20后端)/01-Basic%20Syntax%20(基础语法)/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)

---

## IO Basics (标准输入输出)

> [!abstract] 核心考点：iostream 格式化、文件读写、输入缓冲机制、性能注意事项

```cpp
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

// 标准 IO
std::cout << "hello " << 42 << '\n';
std::cerr << "error\n";           // 不缓冲，立即输出
std::cin  >> x >> y;              // 以空白符分隔
std::getline(std::cin, line);     // 读整行（含空格）

// 格式化输出
std::cout << std::fixed << std::setprecision(2) << 3.14159;  // 3.14
std::cout << std::setw(10) << std::left << "hi";             // 左对齐宽度10
std::cout << std::hex << 255;                                 // ff

// stringstream（字符串 ↔ 数值转换）
std::ostringstream oss;
oss << "val=" << 42;
std::string s = oss.str();

std::istringstream iss("1 2 3");
int a, b, c;
iss >> a >> b >> c;

// 文件 IO
std::ifstream fin("input.txt");
std::ofstream fout("output.txt");
if (!fin) { std::cerr << "open failed\n"; }
std::string line;
while (std::getline(fin, line)) { ... }
fout << "result: " << 42 << '\n';
// RAII：fin/fout 析构时自动关闭
```

---

输入输出与字符串处理密切相关，详见 → [Array & String(数组与字符串)](/02-C++%20Backend%20(C++%20后端)/01-Basic%20Syntax%20(基础语法)/04-Array%20&%20String%20(数组与字符串).md)

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 02-Arrays Strings and IO (数组字符串与输入输出) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？
