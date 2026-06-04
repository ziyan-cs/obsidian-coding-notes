---
tags:
  - cpp
  - syntax
---

> **核心考点**：iostream 格式化、文件读写、输入缓冲机制、性能注意事项

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

## 关联笔记

- [Variables, Types & Operators (变量、类型与运算符)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/01-Variables,%20Types%20&%20Operators%20(变量、类型与运算符).md)
- [Control Flow(流程控制)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/02-Control%20Flow(流程控制).md)
- [Functions(函数)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/03-Functions(函数).md)
- [Array & String(数组与字符串)](/03-C++%20Programming%20(编程语言)/01%20·%20基础语法/04-Array%20&%20String(数组与字符串).md)
- [Const, Typedef & Enum (类型系统基础)](/03-C++%20Programming%20(编程语言)/02%20·%20核心机制/01-Const,%20Typedef%20&%20Enum%20(类型系统基础).md)
