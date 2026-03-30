#stl #set #map #cpp #tree-structure

## ⚡ TL;DR（快速决策）

- `set`：**自动去重 + 自动有序**
- `map`：**键值映射 + 按 key 自动有序**
- 一看到这些需求，要优先想到它们：
    - 需要自动去重
    - 需要有序存储
    - 需要“值 -> 信息”的映射
    - 需要按 key 查找、统计、维护顺序
- 如果要无序但更快，通常改想 `unordered_set` / `unordered_map`
- 如果你需要“有序 + 去重 / 映射”，默认先想 `set` / `map`

## 🧩 Core Idea（核心本质）

- `set` 存的是值，本质像“自动排序的去重集合”
- `map` 存的是键值对，本质像“自动排序的字典”
- 它们底层通常是平衡二叉搜索树
- 所以特点是：
    - 自动有序
    - 查找、插入、删除都比较稳定
- 一句话理解：
    - **`set` 管值，`map` 管键值对。**

## 🔧 Usage Patterns（可复用代码模板）

1. `set` 基础使用

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
    set<int> s;
    s.insert(3);
    s.insert(1);
    s.insert(3);

    for (int x : s) cout << x << ' ';
    return 0;
}
```

1. `map` 基础使用

```cpp
#include <iostream>
#include <map>
using namespace std;

int main() {
    map<string, int> mp;
    mp["alice"] = 90;
    mp["bob"] = 85;

    cout << mp["alice"] << '\\n';
    return 0;
}
```

1. 查找与计数

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
    set<int> s = {1, 3, 5};
    cout << s.count(3) << '\\n';
    cout << s.count(4) << '\\n';
    return 0;
}
```

1. 遍历 `map`

```cpp
#include <iostream>
#include <map>
using namespace std;

int main() {
    map<string, int> mp;
    mp["a"] = 1;
    mp["b"] = 2;

    for (auto [k, v] : mp) {
        cout << k << ' ' << v << '\\n';
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- `set` 会自动去重
- `map[key]` 如果 key 不存在，可能自动创建默认值
- 它们默认是有序的，但不是按插入顺序
- 复杂度通常不是 $O(1)$，而是 $O(\log n)$
- 如果只想查是否存在且不关心顺序，可能更适合哈希容器

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - 插入：$O(log n)$
    - 删除：$O(log n)$
    - 查找：$O(log n)$
- 实战建议：
    - 要有序就先想 `set` / `map`
    - 要更快平均复杂度就想 `unordered_*`
    - `map` 很适合做离散映射、频次统计、有序查询

## 🧪 Common Scenarios（常见使用场景）

- 去重并保持有序
- 字典映射
- 频次统计
- 按 key 排序输出
- 区间 / 前驱后继类树结构题基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <map>
#include <set>
using namespace std;

int main() {
    set<int> s = {3, 1, 2, 2};
    map<string, int> mp;
    mp["x"] = 10;

    for (int x : s) cout << x << ' ';
    cout << '\\n' << mp["x"] << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`set` 和 `map` 就是：基于有序树结构实现的集合与映射容器。**