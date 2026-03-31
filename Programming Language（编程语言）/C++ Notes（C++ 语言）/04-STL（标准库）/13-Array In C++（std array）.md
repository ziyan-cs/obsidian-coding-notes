#stl #array #std-array #cpp #container

## ⚡ TL;DR（快速决策）

- `std::array` 本质是：**带 STL 接口的定长数组**
- 一看到这些需求，要优先想到 `array`：
    - 长度固定
    - 还想用 STL 风格接口和迭代器
    - 想比原生数组更安全、更现代
- 它和 `vector` 的核心区别：
    - `array` 长度固定
    - `vector` 长度可变
- 如果长度在编译期就确定，`array` 往往比原生数组更舒服

## 🧩 Core Idea（核心本质）

- `std::array<T, N>` 本质还是定长数组
- 但它是一个标准容器，能更自然地配合 STL 算法使用
- 支持：
    - `size()`
    - `front()` / `back()`
    - 迭代器
    - 范围 for
- 一句话理解：
    - **`array` = 容器化的普通数组。**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础定义

```cpp
#include <array>
#include <iostream>
using namespace std;

int main() {
    array<int, 5> a = {1, 2, 3, 4, 5};
    cout << a[0] << '\\n';
    cout << a.size() << '\\n';
    return 0;
}
```

1. 遍历

```cpp
for (int x : a) cout << x << ' ';
```

1. 常用成员函数

```cpp
cout << a.front() << '\\n';
cout << a.back() << '\\n';
a.fill(7);
```

1. 排序

```cpp
#include <algorithm>
sort(a.begin(), a.end());
```

## ⚠️ Pitfalls（高频错误）

- `array` 长度固定，不能 `push_back()`
- 模板参数里的 `N` 必须编译期确定
- 和原生数组一样，下标访问仍要注意越界
- 别把 `array` 和 `vector` 的能力混在一起

## 🚀 Performance / Tips（性能优化）

- 连续内存，随机访问 $O(1)$
- 长度固定场景下很轻量
- 比原生数组更适合和 STL 算法搭配
- 如果长度不固定，就该用 `vector`

## 🧪 Common Scenarios（常见使用场景）

- 固定长度数据
- 小型表、坐标、常量容器
- 需要排序 / 遍历的定长数组
- 替代原生数组的现代写法

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <array>
#include <iostream>
using namespace std;

int main() {
    array<int, 5> a = {5, 1, 4, 2, 3};
    sort(a.begin(), a.end());
    for (int x : a) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`std::array` 就是：一个长度固定、但拥有 STL 容器接口的数组。**