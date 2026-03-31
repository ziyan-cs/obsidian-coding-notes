#cpp #compilation #linking #engineering-basics #build-process

## ⚡ TL;DR（快速决策）

- 编译与链接本质是：**先把每个源文件变成目标文件，再把它们拼成可执行程序**
- 一看到这些错误，要优先想到编译 / 链接问题：
    - `undefined reference`
    - 重定义
    - 头文件找不到
    - 声明了但没实现
- 最核心流程：
    1. 预处理
    2. 编译
    3. 汇编
    4. 链接
- 很多“代码明明写了却不能运行”的问题，本质都在这个流程里

## 🧩 Core Idea（核心本质）

- 编译不是一步完成的黑盒
- 对每个 `.cpp` 文件，编译器会先分别处理
- 最后链接器再把多个目标文件、库文件拼起来
- 一句话理解：
    - **编译看单文件，链接看全工程。**
- 所以常见现象：
    - 编译能过，但链接失败
    - 声明没问题，但实现找不到

## 🔧 Usage Patterns（可复用代码模板）

1. 单文件编译

```bash
g++ main.cpp -o main
```

1. 多文件编译链接

```bash
g++ main.cpp math.cpp -o main
```

1. 先分别编译，再链接

```bash
g++ -c main.cpp
g++ -c math.cpp
g++ main.o math.o -o main
```

1. 典型链接错误理解

```
undefined reference to `add(int, int)`
```

要点：

- 往往表示：声明看到了，但链接阶段没找到定义

## ⚠️ Pitfalls（高频错误）

- 只写了声明，没写定义
- 写了定义，但对应 `.cpp` 没参与编译 / 链接
- 函数签名不一致导致“看似定义了，实际不是同一个函数”
- 头文件里写了不该重复定义的内容，导致重定义
- C 和 C++ 混编时还可能涉及名字修饰问题

## 🚀 Performance / Tips（性能优化）

- 理解编译与链接，能极大提升排错效率
- 编译慢时常和头文件依赖过重有关
- 多文件工程里，增量编译能避免每次全部重编
- 报错时先看：
    - 是编译错误？
    - 还是链接错误？

## 🧪 Common Scenarios（常见使用场景）

- 多文件项目构建
- 第三方库接入
- 链接报错排查
- Makefile / CMake 基础理解

## 🧾 Minimal Template（最小可运行模板）

```cpp
// add.h
int add(int a, int b);

// add.cpp
int add(int a, int b) { return a + b; }

// main.cpp
#include <iostream>
#include "add.h"
using namespace std;
int main() { cout << add(1, 2) << '\\n'; }
```

## 📌 One-liner Summary（一句话总结）

- **编译与链接就是：先把每个源文件单独处理，再把所有实现拼成最终程序。**