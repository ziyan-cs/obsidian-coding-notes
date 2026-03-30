#cpp #modern-cpp #optional #nullable #value-wrapper

## ⚡ TL;DR（快速决策）

- `std::optional` 本质是：**一个“可能有值，也可能没值”的对象包装器**
- 一看到这些场景，要优先想到 `optional`：
    - 函数可能返回结果，也可能没有
    - 不想用特殊值表示失败
    - 想比裸指针 / 布尔标记更清晰
- `optional<T>` 比“返回 `-1` 代表不存在”更语义化

## 🧩 Core Idea（核心本质）

- `optional<T>` 要么装着一个 `T`
- 要么是空状态 `nullopt`
- 一句话理解：
    - **`optional` = 明确表达“这个值可能不存在”。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础返回值

```cpp
#include <iostream>
#include <optional>
using namespace std;

optional<int> findPositive(int x) {
    if (x > 0) return x;
    return nullopt;
}
```

1. 判断是否有值

```cpp
auto res = findPositive(5);
if (res.has_value()) cout << *res << '\\n';
```

1. `value_or`

```cpp
cout << res.value_or(0) << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- 空 `optional` 上不能直接乱解引用
- `value()` 在没值时会出错
- `optional` 表达的是“可能没有值”，不是异常机制替代品

## 🚀 Performance / Tips（性能优化）

- 语义清晰通常比微小开销更重要
- 函数返回“可能失败但不算异常”的结果时特别合适
- 比魔法值、双返回标志更现代

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <optional>
using namespace std;

int main() {
    optional<int> x = 42;
    if (x) cout << x.value() << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`std::optional` 就是：一个明确表示“值可能存在，也可能不存在”的现代包装器。**