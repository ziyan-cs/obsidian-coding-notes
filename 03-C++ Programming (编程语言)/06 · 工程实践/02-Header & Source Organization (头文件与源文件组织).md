---
tags:
  - cpp/engineering
status: 🌱
---

> [!important] **核心考点**：头文件职责、源文件职责、include 顺序、模块化设计

## 头文件职责

```cpp
// foo.h — 接口声明
#pragma once

#include <string>  // 必要的标准库
#include <memory>  // 前向声明不足以替代时才 include

// ✅ 前向声明减少依赖
class Bar;  // 只需要指针/引用时不要 #include "Bar.h"

class Foo {
public:
    explicit Foo(std::string name);
    ~Foo();
    
    void process(const Bar& bar);  // 引用，只需要前向声明
    std::string name() const;

private:
    struct Impl;                    // Pimpl 惯用法（不透明指针）
    std::unique_ptr<Impl> pImpl_;
};
```

**头文件放什么**：
- 函数声明（非 inline 函数）
- 类定义
- inline 函数/模板定义
- const/constexpr 常量
- extern 声明

## 源文件职责

```cpp
// foo.cpp — 实现
#include "foo.h"     // 首先包含自己的头文件（检查接口是否自洽）
#include "bar.h"     // 其他依赖
#include <iostream>  // 标准库

struct Foo::Impl {   // Pimpl 实现
    std::string name_;
};

Foo::Foo(std::string name) : pImpl_(std::make_unique<Impl>()) {
    pImpl_->name_ = std::move(name);
}

Foo::~Foo() = default;  // 必须在此处定义（Impl 完整类型）
```

## Include 顺序规范

```cpp
// Google C++ Style Guide 推荐顺序：
#include "foo.h"        // 1. 关联头文件（检查自洽性）
#include <vector>       // 2. C++ 标准库
#include <string.h>     // 3. C 标准库
#include "bar.h"        // 4. 项目内其他模块
```

**为什么关联头文件放在第一个**：
如果 `foo.h` 缺少某个 `#include`，编译 `foo.cpp` 时第一个报错，而不是在其他文件中报出难以定位的错误。

## Forward Declaration vs Include

```cpp
// ✅ 只需要前向声明：
class Bar;              // 声明但不定义
void func(Bar bar);     // 传值？需要完整类型！（调用拷贝构造）
void func(Bar* bar);    // 指针：前向声明就够
void func(Bar& bar);    // 引用：前向声明就够
Bar* createBar();       // 返回指针：前向声明就够

// ❌ 需要完整类型（需要 #include）：
Bar bar;                // 创建对象
bar.someMethod();       // 调用成员函数
sizeof(Bar);            // 获取大小
```

## 模块化组织

```text
project/
├── include/             # 公共头文件（给外部用）
│   └── module/
│       ├── public_a.h
│       └── public_b.h
├── src/                 # 实现 + 私有头文件
│   ├── module/
│   │   ├── impl.cpp
│   │   └── internal.h   # 模块内私有的头文件
│   └── main.cpp
└── test/                # 测试
    └── module/
        └── test_impl.cpp
```

## 常见陷阱

```cpp
// ❌ 循环 include（A.h include B.h, B.h include A.h）
// → 用前向声明打破循环

// ❌ include 爆炸（间接包含大量头文件）
// → 使用前向声明 / Pimpl 惯用法

// ❌ 在头文件中写 using namespace std;
// → 污染所有包含者的命名空间

// ❌ 在头文件中定义非内联函数
// → 多个 .cpp 包含该头文件 → multiple definition 错误
```

> [!tip]- **工程要点**：编译时间是大型 C++ 项目的重要成本。头文件之间的依赖关系直接影响增量编译速度。优先用 **前向声明**，其次用 **Pimpl 惯用法**（将实现细节对用户隐藏），最后才考虑重构成模块。

---

编译与链接过程详见 → [Compilation & Linking (编译与链接)](/03-C++%20Programming%20(编程语言)/06%20·%20工程实践/01-Compilation%20&%20Linking%20(编译与链接)%20⭐.md)
