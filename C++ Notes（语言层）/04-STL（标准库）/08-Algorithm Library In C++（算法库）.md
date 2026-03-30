#stl #algorithm #cpp #sort #binary-search

## ⚡ TL;DR（快速决策）

- `<algorithm>` 本质是：**STL 提供的一套通用算法工具箱**
- 高频函数要优先会这些：
    - `sort`
    - `reverse`
    - `max` / `min`
    - `swap`
    - `find`
    - `count`
    - `lower_bound` / `upper_bound`
    - `unique`
- 如果题目是“已有容器，接下来要处理它”，先想 STL 算法库
- 会用算法库，代码会更短、更稳、更不容易写错

## 🧩 Core Idea（核心本质）

- STL 算法不依赖具体容器，而依赖**迭代器区间**
- 常见形式：`[begin, end)`
- 一句话理解：
    - **算法库就是“拿区间做事”的现成工具。**
- 最大优势：
    - 少手写
    - 不易错
    - 可读性高

## 🔧 Usage Patterns（可复用代码模板）

1. 排序

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {4, 1, 3, 2};
    sort(v.begin(), v.end());
    for (int x : v) cout << x << ' ';
    return 0;
}
```

1. 反转

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3};
    reverse(v.begin(), v.end());
    for (int x : v) cout << x << ' ';
    return 0;
}
```

1. 查找和计数

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 2, 3};
    cout << count(v.begin(), v.end(), 2) << '\\n';
    cout << (find(v.begin(), v.end(), 3) != v.end()) << '\\n';
    return 0;
}
```

1. 二分查找

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 2, 4, 5};
    cout << lower_bound(v.begin(), v.end(), 2) - v.begin() << '\\n';
    cout << upper_bound(v.begin(), v.end(), 2) - v.begin() << '\\n';
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- `lower_bound` / `upper_bound` 前提通常是有序区间
- `unique` 不会真正删元素，要配合 `erase`
- 区间一般是左闭右开 `[begin, end)`
- 自定义排序比较器写错会全盘出错

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `sort`：$O(n log n)$
    - `find` / `count`：$O(n)$
    - `lower_bound`：$O(log n)$
- 实战建议：
    - 能用库函数就少手写
    - 学会区间和比较器，算法库会好用很多

## 🧪 Common Scenarios（常见使用场景）

- 排序与去重
- 二分查找
- 查找最大 / 最小
- 统计元素
- 翻转区间

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {3, 1, 2, 2};
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    for (int x : v) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **算法库就是：STL 提供的一组基于迭代器区间的现成算法工具。**