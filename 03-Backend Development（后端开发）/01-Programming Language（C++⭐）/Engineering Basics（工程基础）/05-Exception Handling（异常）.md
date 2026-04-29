#cpp #exception #try-catch #engineering-basics #error-handling

## ⚡ TL;DR（快速决策）

- 异常处理本质是：**把错误从出错点传递到能处理它的地方**
- 核心语法：
    - `try`
    - `throw`
    - `catch`
- 一看到这些需求，要优先想到异常：
    - 运行过程中发生错误
    - 想把错误向上层传递
    - 不想每层都手动返回错误码
- 但不是所有错误都适合异常，工程里要看风格和约定

## 🧩 Core Idea（核心本质）

- `throw`：抛出异常
- `try`：保护可能出错的代码
- `catch`：捕获并处理异常
- 一句话理解：
    - **异常就是把错误处理流程从正常流程里分离出来。**
- 栈展开是异常的关键机制之一
- RAII 资源管理和异常安全密切相关

## 🔧 Usage Patterns（可复用代码模板）

1. 基础 try-catch

```cpp
#include <iostream>
using namespace std;

int main() {
    try {
        throw runtime_error("something went wrong");
    } catch (const exception& e) {
        cout << e.what() << '\\n';
    }
}
```

1. 自定义抛出

```cpp
if (x < 0) {
    throw invalid_argument("x must be non-negative");
}
```

1. 多个 catch

```cpp
try {
    // ...
} catch (const invalid_argument& e) {
    // ...
} catch (const exception& e) {
    // ...
}
```

## ⚠️ Pitfalls（高频错误）

- 别用异常处理普通分支逻辑
- `catch (...)` 虽然能兜底，但会丢失具体类型信息
- 资源管理写不好时，异常路径容易泄漏资源
- 抛出和捕获对象时，通常优先按 `const&` 捕获

## 🚀 Performance / Tips（性能优化）

- 异常的重点是代码健壮性，不是性能优化
- 工程里更重要的是异常安全和资源释放
- RAII、智能指针、标准容器能大幅降低异常处理负担
- 有些项目会限制异常使用，要看代码规范

## 🧪 Common Scenarios（常见使用场景）

- 非法输入
- 文件 / 资源操作失败
- 构造过程失败
- 上层统一错误处理

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <exception>
#include <iostream>
#include <stdexcept>
using namespace std;

int main() {
    try {
        throw runtime_error("error");
    } catch (const exception& e) {
        cout << e.what() << '\\n';
    }
}
```

## 📌 One-liner Summary（一句话总结）

- **异常处理就是：通过 `throw/try/catch` 把错误从出错处传递到合适的处理位置。**