#cpp #namespace #engineering-basics #scope #name-resolution

## ⚡ TL;DR（快速决策）

- `namespace` 本质是：**给名字分组，避免命名冲突**
- 一看到这些场景，要优先想到命名空间：
    - 不同库里有同名函数 / 类 / 变量
    - 想给项目代码做模块划分
    - 想避免全局名字互相污染
- 最常见写法：
    - `namespace xxx { ... }`
    - `xxx::name`
    - `using std::cout;`
- 工程里能少写 `using namespace std;` 就尽量少写

## 🧩 Core Idea（核心本质）

- C++ 允许不同命名空间里出现同名标识符
- 命名空间解决的是“名字冲突”，不是“访问权限”
- 一句话理解：
    - **命名空间就是给名字加前缀分组。**
- 访问方式：
    - 直接写限定名：`std::vector`
    - 局部引入：`using std::vector;`
    - 全量引入：`using namespace std;`

## 🔧 Usage Patterns（可复用代码模板）

1. 基础定义与访问

```cpp
#include <iostream>
using namespace std;

namespace math_tools {
    int add(int a, int b) {
        return a + b;
    }
}

int main() {
    cout << math_tools::add(2, 3) << '\\n';
    return 0;
}
```

1. 避免命名冲突

```cpp
namespace A { int value = 10; }
namespace B { int value = 20; }

cout << A::value << '\\n';
cout << B::value << '\\n';
```

1. 局部 using

```cpp
using std::cout;
using std::endl;
```

1. 命名空间嵌套

```cpp
namespace project {
    namespace utils {
        void run() {}
    }
}
```

## ⚠️ Pitfalls（高频错误）

- `using namespace std;` 在大型工程或头文件里容易污染名字空间
- 头文件里通常不要随手写全局 `using namespace ...`
- 命名空间不是类，不能当对象用
- 同名函数 / 变量别忘了加限定符 `::`

## 🚀 Performance / Tips（性能优化）

- 命名空间几乎不涉及运行时性能问题
- 它的核心价值是：
    - 提高可维护性
    - 避免命名冲突
    - 让工程结构更清晰
- 实战建议：
    - 小范围用 `using std::xxx`
    - 大型工程优先显式写 `std::`

## 🧪 Common Scenarios（常见使用场景）

- 标准库 `std`
- 自定义工具库分层
- 避免第三方库同名冲突
- 项目模块化组织

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

namespace demo {
    int x = 42;
}

int main() {
    cout << demo::x << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **命名空间就是：给标识符分组并加上作用域前缀，从而避免命名冲突。**