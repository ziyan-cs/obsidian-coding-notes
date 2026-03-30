#cpp #modern-cpp #variant #type-safe-union #visit

## ⚡ TL;DR（快速决策）

- `std::variant` 本质是：**类型安全的多选一联合体**
- 一看到这些场景，要优先想到 `variant`：
    - 一个变量可能是多种类型之一
    - 不想用不安全的 `union`
    - 想显式表达“这个对象当前是哪一种类型”
- 核心配套：
    - `get<T>()`
    - `holds_alternative<T>()`
    - `visit()`

## 🧩 Core Idea（核心本质）

- `variant<A, B, C>` 在任意时刻只保存其中一种类型的值
- 但它比传统 `union` 更安全，因为会跟踪当前活动类型
- 一句话理解：
    - **`variant` = 安全版“多种类型选一个”。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础定义

```cpp
#include <iostream>
#include <variant>
using namespace std;

int main() {
    variant<int, string> v;
    v = 10;
    v = string("hello");
}
```

1. 判断类型

```cpp
if (holds_alternative<string>(v)) {
    cout << get<string>(v) << '\\n';
}
```

1. `visit`

```cpp
visit([](auto&& x) {
    cout << x << '\\n';
}, v);
```

## ⚠️ Pitfalls（高频错误）

- `get<T>()` 类型不匹配会出问题
- 多个相同类型在 `variant` 里会让访问更复杂
- `variant` 强大但也更偏现代泛型风格，别为了炫技乱用

## 🚀 Performance / Tips（性能优化）

- 适合表达“一个值可能属于若干备选类型之一”的模型
- 比自己手写 type tag + union 更安全
- 配合 `visit` 能写出很整洁的分派逻辑

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <variant>
using namespace std;

int main() {
    variant<int, string> v = 123;
    visit([](auto&& x) { cout << x << '\\n'; }, v);
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`std::variant` 就是：一个在多种类型中安全地保存其中一种值的现代类型容器。**