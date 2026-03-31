#cpp #basics #syntax #variables #types

## ⚡ TL;DR（快速决策）

- C++ 基础语法的核心是：**变量、类型、表达式、语句**
- 一开始必须熟悉：
    - 常见类型：`int`、`long long`、`double`、`char`、`bool`
    - 变量定义
    - 运算符
    - 注释
- 算法题里最常用的基础语法，不是花哨特性，而是稳定基本功
- 如果连变量、类型、作用域都不清楚，后面很多题都会乱

## 🧩 Core Idea（核心本质）

- 语法本质上是在描述：
    - 数据怎么存
    - 操作怎么写
    - 语句怎么组织
- 一句话理解：
    - **基础语法就是写程序的最小语言规则。**
- 常见内容：
    - 变量与常量
    - 基本数据类型
    - 表达式与运算符
    - 作用域与代码块

## 🔧 Usage Patterns（可复用代码模板）

1. 变量定义

```cpp
int a = 10;
long long b = 10000000000LL;
double x = 3.14;
char c = 'A';
bool ok = true;
```

1. 常量

```cpp
const int MOD = 1000000007;
```

1. 基本运算

```cpp
int a = 5, b = 2;
cout << a + b << '\\n';
cout << a - b << '\\n';
cout << a * b << '\\n';
cout << a / b << '\\n';
cout << a % b << '\\n';
```

1. 注释

```cpp
// 单行注释
/* 多行注释 */
```

## ⚠️ Pitfalls（高频错误）

- `int` 范围不够时会溢出，要考虑 `long long`
- `=` 和 `==` 别混
- 字符 `'a'` 和字符串 `"a"` 不一样
- 整数除法会直接截断
- 变量作用域别搞混

## 🚀 Performance / Tips（性能优化）

- 算法题里优先追求正确、清晰
- 大数先想 `long long`
- 表达式复杂时多加括号
- 基础语法稳，后面 STL / 算法才不容易出低级错

## 🧪 Common Scenarios（常见使用场景）

- 定义变量保存输入
- 做数值运算
- 写判断和循环前的基础准备
- 后续所有 C++ 代码的底座

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int a = 5;
    long long b = 10;
    double c = 2.5;
    cout << a << ' ' << b << ' ' << c << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **C++ 基础语法就是：定义数据、写表达式、组织语句的基本规则。**