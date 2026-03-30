#cpp #preprocessor #macro #engineering-basics #include

## ⚡ TL;DR（快速决策）

- 预处理本质是：**在真正编译前，先对源码做文本级处理**
- 宏本质是：**简单粗暴的文本替换**
- 一看到这些内容，要优先想到预处理：
    - `#include`
    - `#define`
    - 条件编译 `#if/#ifdef`
    - include guard
- 工程里宏很常见，但也很容易埋坑
- 能用 `const`、`constexpr`、`inline`、函数模板替代宏时，通常优先替代

## 🧩 Core Idea（核心本质）

- 预处理发生在编译前
- 它不是 C++ 语法层面的计算，而更像“先改文本，再交给编译器”
- 常见预处理指令：
    - `#include`：把别的文件内容贴进来
    - `#define`：定义宏
    - `#ifdef/#ifndef/#if`：条件编译
- 一句话理解：
    - **预处理器先改代码文本，编译器再理解代码含义。**

## 🔧 Usage Patterns（可复用代码模板）

1. 宏常量

```cpp
#define MOD 1000000007
```

1. 带参数宏

```cpp
#define SQUARE(x) ((x) * (x))
```

1. 条件编译

```cpp
#ifdef DEBUG
cout << "debug mode\\n";
#endif
```

1. include guard

```cpp
#ifndef MY_HEADER_H
#define MY_HEADER_H

void run();

#endif
```

## ⚠️ Pitfalls（高频错误）

- 宏只是文本替换，容易出现优先级问题
- 带参数宏最好加足够多的括号
- 不要把宏当真正函数
- 宏没有作用域概念
- 头文件防重包含别忘写 include guard 或 `#pragma once`

## 🚀 Performance / Tips（性能优化）

- 预处理本身不是运行时优化工具
- 宏常用于：
    - 条件编译
    - 平台兼容
    - 头文件防重
- 实战建议：
    - 普通常量优先 `constexpr`
    - 简单功能优先函数 / `inline`
    - 宏只在确实需要文本替换时使用

## 🧪 Common Scenarios（常见使用场景）

- include guard
- 调试开关
- 平台差异编译
- 历史代码和竞赛代码中的常量宏

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

#define SQUARE(x) ((x) * (x))

int main() {
    cout << SQUARE(3 + 1) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **预处理与宏就是：在编译前先按规则改写源码文本的机制。**