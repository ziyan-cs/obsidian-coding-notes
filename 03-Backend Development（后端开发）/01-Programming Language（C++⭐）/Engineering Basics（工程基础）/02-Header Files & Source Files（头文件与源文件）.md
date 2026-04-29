#cpp #header-file #source-file #engineering-basics #declaration

## ⚡ TL;DR（快速决策）

- 头文件 `.h/.hpp` 主要放：**声明**
- 源文件 `.cpp` 主要放：**定义 / 实现**
- 一看到这些需求，要优先按“声明和实现分离”来组织：
    - 类定义
    - 函数声明
    - 多文件工程
    - 复用模块代码
- 最核心理解：
    - `.h` 给别人看接口
    - `.cpp` 真正写实现

## 🧩 Core Idea（核心本质）

- 头文件的职责通常是“告诉编译器有什么”
- 源文件的职责通常是“把这些东西真正实现出来”
- 常见分工：
    - 头文件：函数声明、类声明、模板、常量声明
    - 源文件：函数定义、成员函数实现
- 一句话理解：
    - **头文件管接口，源文件管实现。**

## 🔧 Usage Patterns（可复用代码模板）

1. 头文件示例 `math.h`

```cpp
#ifndef MATH_H
#define MATH_H

int add(int a, int b);

#endif
```

1. 源文件示例 `math.cpp`

```cpp
#include "math.h"

int add(int a, int b) {
    return a + b;
}
```

1. 使用方 `main.cpp`

```cpp
#include <iostream>
#include "math.h"
using namespace std;

int main() {
    cout << add(2, 3) << '\\n';
    return 0;
}
```

1. 类的常见拆分

```cpp
// Person.h
class Person {
public:
    void sayHello();
};

// Person.cpp
#include <iostream>
#include "Person.h"
using namespace std;

void Person::sayHello() {
    cout << "hello\\n";
}
```

## ⚠️ Pitfalls（高频错误）

- 头文件重复包含会出问题，所以常用 include guard
- 头文件里不要乱写非 `inline` 的全局定义
- 声明和定义别写得对不上
- `#include "..."` 和 `#include <...>` 用法别混乱
- 模板通常不能像普通函数那样简单拆进 `.cpp`

## 🚀 Performance / Tips（性能优化）

- 合理拆文件能提升工程可维护性，不是直接提升运行时性能
- 头文件尽量只放必要内容，减少编译依赖
- 能前向声明时，可以减少 include 压力
- 公共接口尽量稳定，减少大范围重新编译

## 🧪 Common Scenarios（常见使用场景）

- 多文件项目组织
- 类的声明与实现拆分
- 工具模块复用
- 库的接口暴露

## 🧾 Minimal Template（最小可运行模板）

```cpp
// add.h
#ifndef ADD_H
#define ADD_H
int add(int a, int b);
#endif

// add.cpp
#include "add.h"
int add(int a, int b) { return a + b; }
```

## 📌 One-liner Summary（一句话总结）

- **头文件与源文件的核心就是：把接口声明和具体实现分开管理。**