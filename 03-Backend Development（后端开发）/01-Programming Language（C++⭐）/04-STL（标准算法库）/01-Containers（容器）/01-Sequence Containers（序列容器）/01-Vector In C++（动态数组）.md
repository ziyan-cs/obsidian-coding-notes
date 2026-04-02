#stl #vector #dynamic-array #cpp #container

## ⚡ TL;DR（快速决策）

- `vector` 本质是：**可自动扩容的动态数组**
- 一看到这些需求，要优先想到 `vector`：
    - 需要一个长度可变的顺序容器
    - 需要频繁在末尾追加元素
    - 需要像数组一样用下标访问
    - 需要和 STL 算法一起配合使用
- `vector` 的核心优势：
    - 连续内存
    - 支持随机访问
    - `push_back()` 好用
    - 和 `sort`、`lower_bound`、迭代器配合自然
- 如果你只需要“动态 + 顺序 + 下标访问”，默认先想 `vector`

## 🧩 Core Idea（核心本质）

- `vector` 可以理解成“会自动扩容的数组”
- 和普通数组相比：
    - 普通数组长度固定
    - `vector` 长度可变
- 和链表相比：
    - `vector` 底层是**连续空间**
    - 所以支持 `v[i]` 这种随机访问
- `vector` 的几个核心概念：
    - `size()`：当前元素个数
    - `capacity()`：当前底层预留容量
    - 扩容：空间不够时，会申请更大空间并搬迁元素
- 一句话理解：
    - **`vector` = 更现代、更安全、更好用的数组。**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础定义与初始化

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a;
    vector<int> b(5);          // 5 个 0
    vector<int> c(5, 7);       // 5 个 7
    vector<int> d = {1, 2, 3};

    cout << b.size() << '\n';
    cout << c[0] << '\n';
    return 0;
}
```

要点：

- `vector<int> a;` 最常见
- `vector<int> b(n, val)` 是高频初始化方式

2. 末尾插入与删除

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v;

    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    v.pop_back();

    for (int x : v) cout << x << ' ';
    return 0;
}
```

要点：

- `push_back()`：尾插
- `pop_back()`：删除最后一个元素
- 尾部操作是 `vector` 最舒服的使用方式

3. 下标访问 / 遍历

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};

    for (int i = 0; i < (int)v.size(); ++i) {
        cout << v[i] << ' ';
    }
    cout << '\n';

    for (int x : v) {
        cout << x << ' ';
    }
    return 0;
}
```

要点：

- 最常见遍历方式：
    - 下标遍历
    - 范围 for
- `v[i]` 不检查越界，写题时要自己小心

4. 常用成员函数

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {3, 1, 4};

    cout << v.size() << '\n';
    cout << v.empty() << '\n';
    cout << v.front() << '\n';
    cout << v.back() << '\n';

    v.clear();
    cout << v.empty() << '\n';
    return 0;
}
```

要点：

- `size()`：元素个数
- `empty()`：是否为空
- `front()` / `back()`：首尾元素
- `clear()`：清空元素

5. 二维 `vector`

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 3, m = 4;
    vector<vector<int>> grid(n, vector<int>(m, 0));

    grid[1][2] = 5;
    cout << grid[1][2] << '\n';
    return 0;
}
```

要点：

- 图、网格、DP 表里特别常见
- 这是算法题超高频写法

6. 排序 + 去重

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {4, 2, 2, 1, 3, 3};

    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());

    for (int x : v) cout << x << ' ';
    return 0;
}
```

要点：

- `sort(v.begin(), v.end())` 是最常见排序写法
- `unique` 只是把重复元素移到后面
- 真正去重还要配合 `erase`

7. 预留空间优化

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v;
    v.reserve(100000);

    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }

    cout << v.size() << '\n';
    return 0;
}
```

要点：

- `reserve()` 可以提前申请容量
- 当你知道大概要插多少元素时很有用
- 可以减少反复扩容的代价

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：把 `size()` 当成下标上界直接写错
    - 最后一个下标是 `size() - 1`
- 最常见错误 2：空 `vector` 上直接用 `front()` / `back()`
    - 空容器这样做是危险的
- 最常见错误 3：`clear()` 后以为容量也变成 0
    - `clear()` 清的是元素，不一定清容量
- 最常见错误 4：误以为中间插入 / 删除很快
    - `vector` 中间插删通常要搬元素，代价不低
- 最常见错误 5：迭代器 / 引用失效
    - 扩容后，旧地址、旧迭代器、旧引用可能失效
- 最常见错误 6：忘记 `unique` 后还要 `erase`
- 最常见错误 7：二维 `vector` 初始化写错层级

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - 下标访问：$O(1)$
    - 尾插平均：$O(1)$
    - 尾删：$O(1)$
    - 中间插入 / 删除：$O(n)$
- `vector` 为什么常常比你想象中更快：
    - 连续内存，缓存友好
    - STL 优化成熟
- 实战建议：
    - 顺序存储默认先想 `vector`
    - 元素很多且已知数量时可先 `reserve()`
    - 要频繁头插 / 中间删改时，别硬用 `vector`
- 如果只是想要固定长度数组，也可以考虑：
    - `vector<int> v(n)`
    - 或 `array` / 普通数组（看场景）

## 🧪 Common Scenarios（常见使用场景）

- **基础存储**：保存一串整数 / 字符 / 对象
- **图论**：邻接表 `vector<vector<int>>`
- **动态规划**：一维 / 二维 DP 数组
- **排序题**：配合 `sort`、`unique`、二分
- **模拟题**：动态维护序列
- **算法题默认容器**：大多数“先存起来再处理”的题

## 🆚 Vector vs 其他常见容器

- **vs Array（数组）**
    - 数组：长度固定
    - `vector`：长度动态可变
- **vs List（链表）**
    - `vector`：随机访问强，缓存友好
    - `list`：节点分散，结构修改灵活
- **vs Deque（双端队列）**
    - `vector`：更适合尾部追加 + 随机访问
    - `deque`：更适合两端操作

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {5, 2, 4, 2, 1};

    v.push_back(9);
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());

    for (int x : v) cout << x << ' ';
    cout << '\n';

    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`vector` 就是：一个支持自动扩容、连续存储、随机访问的动态数组。**