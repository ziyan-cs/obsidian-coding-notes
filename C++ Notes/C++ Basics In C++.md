#cpp
## 1. 程序结构与命名空间

- `main` 必须写成 `int main()`，不要写 `void main()`。
- `return 0;` 可以省略，但显式写出更规范。
- `using namespace std;`
    - `.cpp`：可少量使用。
    - `.h`：不要使用，避免命名污染。
- 头文件要做防重包含：

```cpp
#ifndef MY_HEADER_H
#define MY_HEADER_H

// ...

#endif
```

- 如果能用 `#pragma once`，也很常见，但要知道考试/规范里更经典的是 include guard。

## 2. 类型、变量与推导

### 2.1 整数除法

- 两个整数相除，结果仍是整数，小数部分直接截断。

```cpp
int a = 5, b = 2;
cout << a / b; // 2
cout << static_cast<double>(a) / b; // 2.5
```

### 2.2 `const` 与指针

```cpp
int x = 10;

const int* p1 = &x;      // 不能通过 p1 改值
int* const p2 = &x;      // p2 不能改指向
const int* const p3 = &x; // 都不能改
```

- 看谁离 `const` 最近：
    - 靠近类型：值不能改
    - 靠近指针名：指针本身不能改

### 2.3 `auto` 使用习惯

- `auto` 适合简化复杂类型。
- 但会忽略顶层 `const`、引用，必要时手动补：`const auto&`、`auto&`。

```cpp
vector<int> v = {1, 2, 3};
for (auto x : v) cout << x;          // 拷贝
for (auto& x : v) x *= 2;            // 可修改
for (const auto& x : v) cout << x;   // 推荐只读遍历
```

### 2.4 统一初始化 `{}`（新标准必记）

- 推荐优先用花括号初始化，风格统一。
- 它还能防止窄化转换。

```cpp
int a{10};
// int b{3.14}; // ❌ 窄化，报错
```

### 2.5 `nullptr` 替代 `NULL` / `0`

- C++11 起，空指针请优先写 `nullptr`。

```cpp
int* p = nullptr;
```

- 原因：类型更明确，重载时更安全。

## 3. 输入输出与字符串

### 3.1 `cin` 和 `getline` 混用

- `cin >>` 后若直接 `getline`，常读到残留换行。

```cpp
int num;
cin >> num;
cin.ignore(numeric_limits<streamsize>::max(), '\\n');

string line;
getline(cin, line);
```

### 3.2 输出效率

- `endl` = 换行 + 刷新缓冲区，效率低。
- 大多数场景优先用 `\\n`。

### 3.3 格式控制

```cpp
#include <iomanip>
double pi = 3.14159;
cout << fixed << setprecision(2) << pi; // 3.14
```

## 4. 运算符与流程控制

### 4.1 自增

```cpp
int i = 5;
cout << i++; // 5
cout << ++i; // 7
```

- 前置先变再用，后置先用再变。
- 迭代器场景下，优先 `++it`。

### 4.2 短路求值

- `a && b`：`a` 为假，`b` 不执行。
- `a || b`：`a` 为真，`b` 不执行。

### 4.3 位运算

- `x << 1` 常可理解为乘 2。
- `x >> 1` 常可理解为除 2。
- 但对负数、符号位不要机械套用。

### 4.4 `switch`

- 表达式通常是整型、字符、枚举。
- `case` 后必须是常量。
- 别漏 `break`，除非你就是要贯穿。

```cpp
char c = 'a';
switch (c) {
	case 'a':
	case 'A':
		cout << "Alpha";
		break;
	default:
		cout << "Other";
}
```

## 5. 函数基础

### 5.1 重载

- 函数名相同，但参数列表不同，才叫重载。
- 仅返回值不同，不算重载。

### 5.2 默认参数

- 必须从右往左连续给默认值。

```cpp
void func(int a, int b = 10, int c = 20);
```

### 5.3 `inline`

- 适合短小频繁调用的函数。
- 它只是“建议”，编译器可不采纳。

## 6. 入门后必须补上的现代 C++ 习惯

### 6.1 范围 `for`（C++11）

- 能不用下标就别硬写下标。

```cpp
for (const auto& x : v) {
	cout << x << '\\n';
}
```

### 6.2 `using` 替代 `typedef`

```cpp
using ll = long long;
using Vec = vector<int>;
```

- 更直观，模板别名也更方便。

### 6.3 `enum class` 替代普通枚举

```cpp
enum class Color { Red, Green, Blue };
Color c = Color::Red;
```

- 优点：作用域更清晰，不易污染命名空间。

### 6.4 `constexpr`（比 `const` 更进一步）

- 表示值或函数可在编译期求值。

```cpp
constexpr int square(int x) {
	return x * x;
}
```

- 记忆：`const` 强调“不能改”，`constexpr` 强调“编译期可算”。

### 6.5 结构化绑定（C++17）

```cpp
pair<int, string> p = {1, "Tom"};
auto [id, name] = p;
```

- 读 `pair` / `tuple` / map 元素时很方便。

### 6.6 `if` / `switch` 初始化器（C++17）

```cpp
if (int x = foo(); x > 0) {
	cout << x;
}
```

- 变量作用域更小，代码更整洁。

### 6.7 `override` 必写（面向对象常考习惯）

```cpp
class Base {
public:
	virtual void show();
};

class Derived : public Base {
public:
	void show() override;
};
```

- 防止“以为重写了，实际没重写”的低级错误。

## 7. 高频避坑速记

1. 整数相除默认还是整数。
2. `cin >>` 后接 `getline` 记得 `cin.ignore()`。
3. 指针的 `const` 看位置。
4. `switch` 写完 `case` 先想 `break`。
5. 输出大量数据优先 `\\n`，少用 `endl`。
6. 空指针用 `nullptr`，不要再依赖 `NULL`。
7. 遍历容器优先 `const auto&`。
8. 初始化优先 `{}`。
9. 类型别名优先 `using`。
10. 继承重写函数时优先加 `override`。

## 8. 现阶段学习优先级建议

- 先熟：`const`、引用、指针、函数参数传递。
- 再熟：`vector`、`string`、范围 `for`、`auto`。
- 接着补：类与对象、构造函数、拷贝控制、继承多态。
- 写题时坚持现代写法：`nullptr`、`using`、`enum class`、`override`、`const auto&`。
