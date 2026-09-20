---
study_stage: learn
review_due: 2026-09-24
---

> [!abstract] 学习定位
头文件声明接口，源文件提供定义，命名空间隔离名称；编译器分别生成目标文件，链接器再解析跨文件符号。

> [!summary] 核心摘要
>
> 分离编译让每个 `.cpp` 独立编译，只通过头文件看到依赖接口。头文件必须可重复包含且尽量稳定；普通函数或变量的定义不要放进头文件，否则多个翻译单元可能触发 ODR 冲突。模板和 `inline` 实体通常需要让使用点看到定义。

# 翻译单元与构建过程

一个 `.cpp` 经过预处理后形成一个 translation unit（翻译单元）。典型过程是：

```text
源文件 + 头文件
      |
      v
预处理 -> 编译 -> 汇编 -> 目标文件
                          |
                          v
                    链接 -> 可执行文件
```

- 编译错误：当前翻译单元无法通过语法或类型检查。
- 链接错误：声明存在，但最终找不到唯一可用的定义。
- 运行错误：构建成功，执行时才违反程序约束。

# 声明与定义

```cpp
// math.hpp
#pragma once
namespace app {
int add(int lhs, int rhs);  // 声明
}

// math.cpp
#include "math.hpp"
int app::add(int lhs, int rhs) {  // 定义
    return lhs + rhs;
}
```

声明告诉编译器“名称和类型是什么”，定义提供实体或实现。一个程序中，同一非 `inline` 实体通常只能有一个定义，这就是 One Definition Rule（ODR）的核心约束。

# 头文件边界

## 应当放入

- 对外类型、函数和常量的声明。
- 模板定义，以及必须在使用点可见的 `inline` 定义。
- 接口所需的最小依赖。

## 不应轻易放入

- `using namespace ...;`，它会污染所有包含者。
- 具有外部链接的普通变量定义。
- 与接口无关的大量实现细节和重量级依赖。

能前置声明时可以降低耦合，但按值成员、继承关系以及访问类型成员时通常需要完整定义。

# 命名空间与名称查找

命名空间解决大型程序中的重名问题，不负责访问控制。优先写清限定名，在局部作用域使用 `using` 声明；不要在公共头文件使用 `using namespace`。

```cpp
namespace storage {
class Client;
}

void connect(storage::Client& client);
```

# 最小验证

- [ ] 建立 `main.cpp`、`math.hpp`、`math.cpp`，分别编译后链接。
- [ ] 故意只声明不定义，观察 undefined reference / unresolved external symbol。
- [ ] 把普通函数定义放进头文件并由两个 `.cpp` 包含，观察 ODR 问题，再用正确结构修复。

> [!question]- 自测：先回答再展开
> 1. 为什么 `#include` 更接近文本展开，而不是导入已编译模块？
> 2. 哪些错误发生在编译期，哪些发生在链接期？
> 3. 前置声明为什么不能支持按值成员？

> [!info]- 延伸阅读
> - 模块验收：[00-C++ Backend Standard (C++ 后端标准与自测)](/03-C++%20Backend%20(C++%20后端)/00-C++%20Backend%20Standard%20(C++%20后端标准与自测).md)
> - 前置：[01-Core Syntax and Functions (核心语法与函数)](/03-C++%20Backend%20(C++%20后端)/01-Language%20Basics%20(语言基础)/01-Core%20Syntax%20and%20Functions%20(核心语法与函数).md)
> - 延伸：[01-CMake Project and Targets (CMake 项目与目标)](/02-Engineering%20Fundamentals%20(工程基础)/02-Build%20and%20Dependencies%20(构建与依赖)/01-CMake%20Project%20and%20Targets%20(CMake%20项目与目标).md)
