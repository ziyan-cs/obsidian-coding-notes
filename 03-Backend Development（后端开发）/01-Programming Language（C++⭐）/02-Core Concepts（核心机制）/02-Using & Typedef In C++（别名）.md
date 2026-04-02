#cpp #using #typedef #alias #type-alias

## ⚡ TL;DR（快速决策）

- 别名本质是：**给已有类型起一个更好用、更清晰的名字**
- C++ 里常见两种写法：
    - `typedef`
    - `using`
- 现代 C++ 一般更推荐 `using`
- 一看到这些场景，要优先想到别名：
    - 类型太长
    - 模板类型可读性差
    - 想统一表达业务含义

## 🧩 Core Idea（核心本质）

- 别名不会创建新类型，只是换个名字
- 一句话理解：
    - **别名就是给复杂类型贴一个清晰标签。**
- 例子：
    - `vector<pair<int,int>>` 很长
    - 起别名后读写都更舒服

## 🔧 Usage Patterns（可复用代码模板）

1. `typedef` 基础

```cpp
typedef long long ll;
typedef pair<int, int> PII;
```

2. `using` 基础

```cpp
using ll = long long;
using PII = pair<int, int>;
```

3. 模板别名

```cpp
template <typename T>
using Vec = vector<T>;
```

3. 提升可读性

```cpp
using Graph = vector<vector<int>>;
Graph g(n + 1);
```

## ⚠️ Pitfalls（高频错误）

- 以为别名会创建全新独立类型
- `typedef` 对复杂模板可读性差
- 别名名字起得太随意，反而更难懂

## 🚀 Performance / Tips（性能优化）

- 别名主要提升的是可读性和维护性
- 模板代码里 `using` 的优势很明显
- 在现代 C++ 中，通常优先用 `using`

## 🧪 Common Scenarios（常见使用场景）

- `long long` 简写
- 复杂 STL 类型简化
- 图、树、表等类型命名
- 模板别名封装

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;
using ll = long long;
using VI = vector<int>;

int main() {
    ll x = 123456789;
    VI a = {1, 2, 3};
    cout << x << ' ' << a.size() << '\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`using` 与 `typedef` 的核心就是：给已有类型起更清晰、更短的别名。**