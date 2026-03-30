#stl #iterator #cpp #container #algorithm

## ⚡ TL;DR（快速决策）

- 迭代器（iterator）本质是：**用统一方式遍历和访问容器元素的“位置指针”**
- 一看到这些需求，要优先想到迭代器：
    - 想遍历 STL 容器
    - 想和 `sort`、`find`、`erase` 等算法配合
    - 想在容器中表示“当前位置”
- 最常见写法：`begin()`、`end()`
- 可以把迭代器理解成“广义指针”，但它不一定真的是原始指针
- 如果题目在 STL 容器上做操作，迭代器几乎绕不开

## 🧩 Core Idea（核心本质）

- 不同容器底层结构不同，但 STL 希望提供统一遍历接口
- 所以引入迭代器：
    - `begin()` 指向第一个元素
    - `end()` 指向最后一个元素后面的位置
- 一句话理解：
    - **迭代器就是容器里的“当前位置”。**
- 常见操作：
    - `*it`：取值
    - `++it`：后移
    - `it != c.end()`：判断是否结束

## 🔧 Usage Patterns（可复用代码模板）

1. 基础遍历

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3};
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << ' ';
    }
    return 0;
}
```

1. 配合算法

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {3, 1, 4};
    sort(v.begin(), v.end());
    auto it = find(v.begin(), v.end(), 3);
    if (it != v.end()) cout << *it << '\\n';
    return 0;
}
```

1. `erase` 返回值

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4};
    for (auto it = v.begin(); it != v.end();) {
        if (*it % 2 == 0) it = v.erase(it);
        else ++it;
    }
    for (int x : v) cout << x << ' ';
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- `end()` 不能解引用
- `erase()` 后旧迭代器可能失效
- 不是所有容器都支持 `it + 1`
- 修改容器后，原迭代器可能失效

## 🚀 Performance / Tips（性能优化）

- `vector` / `string` 迭代器支持随机访问
- `set` / `map` 迭代器更像树上的位置
- 实战里：不会迭代器，很多 STL 写法就不顺手

## 🧪 Common Scenarios（常见使用场景）

- 遍历容器
- 配合 STL 算法
- 删除满足条件元素
- 二分、查找、区间操作

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3};
    for (auto it = v.begin(); it != v.end(); ++it) {
        cout << *it << ' ';
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **迭代器就是：STL 容器中用来统一表示当前位置和遍历方式的访问工具。**