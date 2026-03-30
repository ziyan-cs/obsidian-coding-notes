#stl #pair #tuple #cpp #utility

## ⚡ TL;DR（快速决策）

- `pair` 本质是：**把两个值打包在一起**
- `tuple` 本质是：**把多个值打包在一起**
- 一看到这些需求，要优先想到它们：
    - 需要临时返回多个值
    - 需要把两个 / 多个字段作为一个整体传来传去
    - 需要在排序、映射、图论里保存复合信息
- 高频场景：
    - `pair<int, int>` 存坐标、边、键值对
    - `tuple` 存 3 个及以上字段
    - 配合 `sort`、`priority_queue`、函数返回值使用
- 如果只有两个值，优先先想 `pair`

## 🧩 Core Idea（核心本质）

- `pair` 有两个成员：`first` 和 `second`
- `tuple` 可以存任意多个、任意类型的值
- 它们的本质不是“新容器”，而是**打包数据的工具**
- 一句话理解：
    - **`pair` 管两个值，`tuple` 管多个值。**

## 🔧 Usage Patterns（可复用代码模板）

1. `pair` 基础使用

```cpp
#include <iostream>
#include <utility>
using namespace std;

int main() {
    pair<int, string> p = {18, "Alice"};
    cout << p.first << ' ' << p.second << '\\n';
    return 0;
}
```

1. `make_pair`

```cpp
auto p = make_pair(3, 5);
cout << p.first << ' ' << p.second << '\\n';
```

1. `pair` 排序

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<pair<int, int>> v = {{2, 3}, {1, 5}, {1, 2}};
    sort(v.begin(), v.end());
    for (auto [x, y] : v) cout << x << ',' << y << '\\n';
}
```

1. `tuple` 基础使用

```cpp
#include <iostream>
#include <tuple>
using namespace std;

int main() {
    tuple<int, string, double> t = {1, "Tom", 95.5};
    cout << get<0>(t) << ' ' << get<1>(t) << ' ' << get<2>(t) << '\\n';
    return 0;
}
```

1. `tie` / 结构化绑定

```cpp
auto [a, b] = make_pair(10, 20);
auto [x, y, z] = make_tuple(1, 2, 3);
```

## ⚠️ Pitfalls（高频错误）

- `pair` 成员名固定是 `first` / `second`
- `tuple` 取值要用 `get<下标>()`
- `get<>()` 的下标是编译期常量，不能写变量
- `pair` / `tuple` 默认比较是**字典序**
- 当字段变多时，`tuple` 可读性会下降，别乱堆太多层

## 🚀 Performance / Tips（性能优化）

- `pair` 在竞赛和算法题里极高频，写法短、传参方便
- `tuple` 适合字段较多但又不想单独写结构体时使用
- 如果语义很强、字段很多，直接写 `struct` 往往更清楚

## 🧪 Common Scenarios（常见使用场景）

- 坐标：`pair<int, int>`
- 边信息、区间信息
- 函数返回多个值
- 排序复合关键字
- 堆、图、BFS 状态打包

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <tuple>
using namespace std;

int main() {
    pair<int, int> p = {3, 4};
    auto t = make_tuple(1, 2, 3);

    cout << p.first << ' ' << p.second << '\\n';
    cout << get<0>(t) << ' ' << get<1>(t) << ' ' << get<2>(t) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`pair` 和 `tuple` 就是：把多个值打包成一个整体来传递和处理的工具。**