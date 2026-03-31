#stl #function #cpp #callable #lambda

## ⚡ TL;DR（快速决策）

- `std::function` 本质是：**统一包装“可调用对象”的通用函数容器**
- 它能装：
    - 普通函数
    - Lambda
    - 仿函数对象
    - `bind` 结果
- 一看到这些需求，要优先想到 `std::function`：
    - 需要把函数当参数传来传去
    - 需要保存回调函数
    - 需要写通用接口接收不同 callable
- 如果你想“把函数像变量一样存起来”，优先先想 `std::function`

## 🧩 Core Idea（核心本质）

- C++ 里“能调用的东西”不只有普通函数
- Lambda、重载了 `operator()` 的对象也都能调用
- `std::function` 的价值就是：
    - **把不同种类的可调用对象统一成同一种类型**
- 一句话理解：
    - **`std::function` = callable 的统一包装器。**
- 常见形式：
    - `function<int(int, int)>`
    - 表示“接收两个 `int`，返回一个 `int` 的可调用对象”

## 🔧 Usage Patterns（可复用代码模板）

1. 包装普通函数

```cpp
#include <functional>
#include <iostream>
using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    function<int(int, int)> f = add;
    cout << f(2, 3) << '\\n';
    return 0;
}
```

1. 包装 Lambda

```cpp
function<int(int, int)> f = [](int a, int b) {
    return a * b;
};
cout << f(3, 4) << '\\n';
```

1. 作为参数传递

```cpp
#include <functional>
#include <iostream>
using namespace std;

int calc(int a, int b, function<int(int, int)> op) {
    return op(a, b);
}
```

1. 递归 Lambda 常见写法

```cpp
#include <functional>
#include <iostream>
using namespace std;

int main() {
    function<int(int)> fib = [&](int n) {
        if (n <= 1) return n;
        return fib(n - 1) + fib(n - 2);
    };
    cout << fib(5) << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- 模板签名要写对，比如参数类型和返回值别写错
- 空的 `function` 直接调用会出问题
- `std::function` 很方便，但不是零开销
- 在极致性能场景下，直接模板或直接 lambda 可能更快

## 🚀 Performance / Tips（性能优化）

- `std::function` 重点是通用性和可读性，不是极致性能
- 回调、策略模式、递归 lambda 包装时很常用
- 如果接口只在编译期确定，模板有时更合适
- 如果你需要统一保存不同 callable，`std::function` 非常顺手

## 🧪 Common Scenarios（常见使用场景）

- 回调函数
- 自定义比较 / 策略传递
- 保存 lambda
- 递归 lambda
- 通用函数接口设计

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <functional>
#include <iostream>
using namespace std;

int main() {
    function<int(int, int)> op = [](int a, int b) {
        return a + b;
    };

    cout << op(10, 20) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`std::function` 就是：把普通函数、lambda 和仿函数统一包装起来的函数对象容器。**