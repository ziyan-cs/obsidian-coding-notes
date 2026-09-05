---
tags:
  - cpp/syntax
status: 🌱
---

> [!important] **核心考点**：iostream 格式化、文件读写、输入缓冲机制、性能注意事项

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

输入输出与字符串处理密切相关，详见 → [Array & String(数组与字符串)](/03-C++%20Programming%20(编程语言)/01-Basic%20Syntax%20(基础语法)/04-Array%20&%20String%20(数组与字符串).md)
