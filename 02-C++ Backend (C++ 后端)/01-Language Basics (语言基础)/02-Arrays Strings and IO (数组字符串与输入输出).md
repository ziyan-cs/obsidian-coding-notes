---
status: stable
confidence: high
verified: 2026-09-06
review_due: 2026-09-08
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# 30 秒回答

**核心结论**：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# Array & String (数组与字符串)

> [!note] 本节重点：核心考点：C 风格数组与指针的关系、std::string 的操作与性能、C 字符串函数

# 数组

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

# C 字符串 vs std::string

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

数组类型的底层机制详见 → Variables, Types & Operators (变量、类型与运算符)

---

# IO Basics (标准输入输出)

> [!note] 本节重点：核心考点：iostream 格式化、文件读写、输入缓冲机制、性能注意事项

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

输入输出与字符串处理密切相关，详见 → Array & String(数组与字符串)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 复述

- 不看正文，说明 02-Arrays Strings and IO (数组字符串与输入输出) 的问题、核心机制与边界。

## 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

## 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

# 关联学习

- 导航：[00-Language Basics Map (语言基础导航)](/02-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/00-Language%20Basics%20Map%20(语言基础导航).md)
- 下一步：[01-Core Syntax and Functions (核心语法与函数)](/02-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)
