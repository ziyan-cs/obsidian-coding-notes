#stl #unordered-set #unordered-map #hash #cpp

## ⚡ TL;DR（快速决策）

- `unordered_set` 和 `unordered_map` 本质是：**基于哈希表实现的无序容器**
- 一看到这些需求，要优先想到它们：
    - 判重 / 去重
    - 查某个值是否存在
    - 统计出现次数
    - 做“值 -> 信息”的快速映射
- 它们和 `set` / `map` 的核心区别：
    - `set` / `map`：**有序**，通常 $O(\log n)$
    - `unordered_set` / `unordered_map`：**无序**，平均 $O(1)$
- 如果题目重点是“快速查找”，而不是“有序输出”，优先先想哈希容器
- 高频使用：
    - `unordered_set`：只关心值是否出现
    - `unordered_map`：关心 key 对应的 value

## 🧩 Core Idea（核心本质）

- 哈希容器的核心思想是：
    - 把 key 通过哈希函数映射到某个桶里
    - 从而实现平均意义上的快速查找、插入、删除
- `unordered_set`：
    - 只存 key
    - 自动去重
- `unordered_map`：
    - 存 key-value
    - key 唯一，value 可修改
- 一句话理解：
    - **`unordered_set` 管“有没有”，`unordered_map` 管“对应是什么”。**
- 关键特征：
    - 无序
    - 平均快
    - 不支持按大小顺序遍历

## 🔧 Usage Patterns（可复用代码模板）

1. `unordered_set` 基础使用

```cpp
#include <iostream>
#include <unordered_set>
using namespace std;

int main() {
    unordered_set<int> st;
    st.insert(3);
    st.insert(1);
    st.insert(3);

    cout << st.count(3) << '\\n';
    cout << st.count(2) << '\\n';
    return 0;
}
```

要点：

- `insert()` 插入元素
- `count(x)` 判断是否存在
- 自动去重

1. `unordered_map` 基础使用

```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    unordered_map<string, int> mp;
    mp["alice"] = 90;
    mp["bob"] = 85;

    cout << mp["alice"] << '\\n';
    return 0;
}
```

要点：

- `mp[key]` 可以访问和修改 value
- 很适合做“名字 -> 分数”“数字 -> 次数”这类映射

1. 统计频次

```cpp
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {1, 2, 2, 3, 3, 3};
    unordered_map<int, int> freq;

    for (int x : nums) {
        ++freq[x];
    }

    for (auto [k, v] : freq) {
        cout << k << ' ' << v << '\\n';
    }
    return 0;
}
```

要点：

- 频次统计是 `unordered_map` 超高频场景
- `++freq[x]` 是常见写法

1. 判重问题

```cpp
#include <iostream>
#include <unordered_set>
#include <vector>
using namespace std;

bool containsDuplicate(const vector<int>& nums) {
    unordered_set<int> st;
    for (int x : nums) {
        if (st.count(x)) return true;
        st.insert(x);
    }
    return false;
}
```

要点：

- 一边遍历一边判断是否已出现
- 这是哈希容器最经典的题型之一

1. Two Sum 模板

```cpp
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

vector<int> twoSum(const vector<int>& nums, int target) {
    unordered_map<int, int> pos;

    for (int i = 0; i < (int)nums.size(); ++i) {
        int need = target - nums[i];
        if (pos.count(need)) {
            return {pos[need], i};
        }
        pos[nums[i]] = i;
    }

    return {};
}
```

要点：

- 核心是把“找另一半”变成哈希查询
- `值 -> 下标` 是极高频模型

1. 遍历 `unordered_map`

```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    unordered_map<string, int> mp;
    mp["x"] = 1;
    mp["y"] = 2;

    for (auto [k, v] : mp) {
        cout << k << ' ' << v << '\\n';
    }
    return 0;
}
```

要点：

- 遍历顺序**不固定**
- 不要对输出顺序有假设

1. 预留空间优化

```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    unordered_map<int, int> mp;
    mp.reserve(100000);

    for (int i = 0; i < 10; ++i) {
        mp[i] = i * i;
    }

    cout << mp[3] << '\\n';
    return 0;
}
```

要点：

- 元素很多时，`reserve()` 可以减少 rehash 次数
- 在数据量大时比较常见

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：误以为它们是有序的
    - `unordered_*` 默认**无序**
    - 不能拿它们做有序输出
- 最常见错误 2：把 `mp[key]` 当纯查询
    - 如果 key 不存在，`mp[key]` 会自动创建默认值
- 最常见错误 3：复杂度理解绝对化
    - 平均是 $O(1)$
    - 但不是任何情况都严格常数
- 最常见错误 4：遍历顺序写死
    - 不同环境下顺序都可能不同
- 最常见错误 5：该用 `set/map` 时硬用哈希容器
    - 如果题目需要有序，哈希容器就不合适

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    
    - 插入：平均 $O(1)$
    - 删除：平均 $O(1)$
    - 查找：平均 $O(1)$
- 实战建议：
    
    - 只关心“是否存在”时优先 `unordered_set`
    - 需要“key -> value”映射时优先 `unordered_map`
    - 数据量大时可考虑 `reserve()`
- 如果题目需要：
    
    - 排序输出
    - 前驱 / 后继
    - 有序遍历
    
    那就该考虑 `set` / `map`
    

## 🧪 Common Scenarios（常见使用场景）

- 判重
- 去重
- 频次统计
- 哈希映射
- Two Sum
- 字符串 / 数字统计
- 快速存在性查询

## 🆚 Unordered Set & Unordered Map vs 其他常见容器

- **vs Set / Map**
    - `set/map`：有序，通常 $O(\log n)$
    - `unordered_*`：无序，平均 $O(1)$
- **vs Vector**
    - `vector`：顺序存储，查找常常要线性扫
    - `unordered_*`：更适合快速查找
- **vs Array 计数**
    - 数组适合值域小且连续
    - 哈希容器适合值域大、离散、不连续

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <unordered_map>
#include <unordered_set>
using namespace std;

int main() {
    unordered_set<int> st = {1, 2, 3};
    unordered_map<string, int> mp;
    mp["a"] = 10;

    cout << st.count(2) << '\\n';
    cout << mp["a"] << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`unordered_set` 和 `unordered_map` 就是：基于哈希表实现、平均查找很快但默认无序的 STL 容器。**