#algorithm #hashing #hash-table #unordered-set #unordered-map

## ⚡ TL;DR（快速决策）

- 哈希的核心不是“排序”，而是：**把值映射到一个位置 / 桶里，从而快速判断、查找、统计**
- 一看到这些关键词，要优先想到哈希：
    - **判重 / 去重**
    - **查是否存在**
    - **统计出现次数**
    - **两数之和 / 配对问题**
    - **字符串频次 / 字符匹配**
    - **前缀和 + 哈希优化**
- 哈希常见容器：
    - `unordered_set`：存值，适合判重 / 查存在
    - `unordered_map`：存键值对，适合计数 / 建映射
- 哈希平均查询、插入、删除复杂度通常是 $O(1)$
- 但它**不是绝对 O(1)**，极端冲突情况下可能退化
- 如果题目目标是“快速知道某个东西以前有没有出现过”，优先考虑哈希

## 🧩 Core Idea（核心本质）

- 本质：把一个数据通过某种映射规则，快速定位到一个“位置”
- 这个“位置”不一定是真实数组下标，在 C++ 题目里通常由哈希表容器帮你维护
- 哈希解决的问题，本质上往往是：
    - **存在性查询**
    - **频次统计**
    - **元素到信息的映射**
- 和排序的区别：
    - 排序是把数据排成有序结构，再做查找
    - 哈希是直接建立“值 → 信息”的映射
- 所以哈希的优势通常在于：
    - 不要求有序
    - 写法直接
    - 查找速度快
- 常见思维模型：
    - `值 -> 是否出现过`
    - `值 -> 出现次数`
    - `值 -> 下标`
    - `前缀状态 -> 出现次数 / 最早位置`

## 🔧 Usage Patterns（可复用代码模板）

1. 判重 / 去重：`unordered_set`

```cpp
#include <iostream>
#include <unordered_set>
#include <vector>
using namespace std;

bool containsDuplicate(const vector<int>& nums) {
    unordered_set<int> seen;

    for (int x : nums) {
        if (seen.count(x)) return true;
        seen.insert(x);
    }

    return false;
}
```

要点：

- `count(x)` 非 0 表示存在
- 适合“是否出现过”这类题
- 如果只关心存在性，不需要 `map`

1. 统计频次：`unordered_map`

```cpp
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<int, int> countFreq(const vector<int>& nums) {
    unordered_map<int, int> freq;

    for (int x : nums) {
        ++freq[x];
    }

    return freq;
}
```

要点：

- `freq[x]` 表示数字 `x` 的出现次数
- 非常适合众数、统计、分类计数问题

1. Two Sum（哈希经典题）

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
- `值 -> 下标` 是非常高频的哈希模型
- 先查再插，能避免同一个元素被重复使用

1. 字符串字符统计

```cpp
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

unordered_map<char, int> charFrequency(const string& s) {
    unordered_map<char, int> freq;

    for (char c : s) {
        ++freq[c];
    }

    return freq;
}
```

要点：

- 适合统计字符频率、构造判断、异位词问题
- 如果字符范围很小，也可以直接用数组代替哈希

1. 有效字母异位词（哈希 / 数组都能做）

```cpp
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

bool isAnagram(const string& s, const string& t) {
    if (s.size() != t.size()) return false;

    unordered_map<char, int> cnt;
    for (char c : s) ++cnt[c];
    for (char c : t) {
        --cnt[c];
        if (cnt[c] < 0) return false;
    }

    return true;
}
```

要点：

- 哈希思想本质仍然是“统计后比较”
- 小写字母题常可优化成长度 26 的数组

1. 前缀和 + 哈希：子数组和为 k

```cpp
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> cnt;
    cnt[0] = 1;

    int prefix = 0;
    int ans = 0;

    for (int x : nums) {
        prefix += x;
        if (cnt.count(prefix - k)) {
            ans += cnt[prefix - k];
        }
        ++cnt[prefix];
    }

    return ans;
}
```

要点：

- 单纯前缀和只能快速算区间和
- **前缀和 + 哈希** 可以进一步快速统计满足条件的区间数量
- 这是面试和算法题里的超级高频组合

1. 最长连续序列

```cpp
#include <iostream>
#include <unordered_set>
#include <vector>
using namespace std;

int longestConsecutive(vector<int>& nums) {
    unordered_set<int> st(nums.begin(), nums.end());
    int ans = 0;

    for (int x : st) {
        if (st.count(x - 1)) continue;  // 不是起点

        int cur = x;
        int len = 1;
        while (st.count(cur + 1)) {
            ++cur;
            ++len;
        }

        ans = max(ans, len);
    }

    return ans;
}
```

要点：

- 这题的高效做法依赖哈希集合
- 关键优化：只从连续段起点开始扩展

## ⚠️ Pitfalls（高频错误）

- 不要把哈希理解成“自动有序”
    - `unordered_map` / `unordered_set` **默认无序**
    - 如果你需要有序遍历，应该考虑 `map` / `set`
- 不要混淆：
    - `set` / `map`：底层通常是红黑树，复杂度常为 $O(\log n)$
    - `unordered_set` / `unordered_map`：底层是哈希表，平均复杂度常为 $O(1)$
- `mp[x]` 和 `mp.count(x)` 作用不同：
    - `mp.count(x)`：查是否存在
    - `mp[x]`：如果不存在，可能会**自动创建默认值**
- 写 Two Sum 这类题时，常见错误是：
    - 先插入再查询，导致自己匹配自己
    - 忘记存的是下标不是次数
- 频次统计题常见错误：
    - 忘记初始化前缀状态，如 `cnt[0] = 1`
    - 更新顺序写反
- 字符串类题目常见误区：
    - 字符范围很小时，其实数组比哈希更快更简单
- 哈希不适合做什么：
    - 需要顺序
    - 需要区间最值
    - 需要按大小找前驱 / 后继

## 🚀 Performance / Tips（性能优化）

- 哈希表平均时间复杂度：
    - 插入：$O(1)$
    - 查询：$O(1)$
    - 删除：$O(1)$
- 但注意：
    - 这是**平均情况**
    - 冲突严重时可能退化
- 常见实战技巧：
    - 只做“存在性判断”时优先 `unordered_set`
    - 需要“值 -> 信息”映射时用 `unordered_map`
    - 字符范围固定时优先数组，不一定非要哈希
- 如果数据量较大，可以考虑预留空间减少 rehash：

```cpp
unordered_map<int, int> mp;
mp.reserve(100000);
```

- 为什么有时哈希没你想的快：
    - 常数不小
    - rehash 有开销
    - 某些场景数组 / 排序更稳
- 经验判断：
    - 要判重、查存在、做映射：先想哈希
    - 要有序、要排名、要区间：别先想哈希

## 🧪 Common Scenarios（常见使用场景）

- **判重问题**：数组是否有重复元素
- **查存在问题**：某个数 / 某个字符串是否出现过
- **频次统计问题**：字符统计、数字统计、众数
- **配对问题**：Two Sum、差值 / 和值匹配
- **字符串问题**：异位词、模式匹配、窗口计数
- **前缀状态问题**：
    - 子数组和为 k
    - 相同前缀状态出现次数统计
- **离散映射问题**：
    - 值到编号
    - 字符到次数
    - 元素到首次出现位置

## 🆚 Hashing vs 其他常见方法

- **vs Array（数组计数）**
    - 数组：范围小、连续时更快更省
    - 哈希：范围大、值离散时更灵活
- **vs Sorting（排序）**
    - 排序：适合有序处理、双指针、相邻比较
    - 哈希：适合快速存在性查询和映射
- **vs Tree Map / Tree Set**
    - 树：有序，支持前驱后继、范围操作
    - 哈希：无序，但平均更快

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {1, 2, 2, 3, 5, 3};

    unordered_set<int> st;
    unordered_map<int, int> freq;

    for (int x : nums) {
        st.insert(x);
        ++freq[x];
    }

    cout << "去重后元素个数: " << st.size() << '\\n';
    cout << "数字 2 出现次数: " << freq[2] << '\\n';
    cout << "数字 4 是否出现: " << (st.count(4) ? "yes" : "no") << '\\n';

    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **哈希思想就是：把“查找 / 判重 / 统计 / 映射”这些操作，转化成对哈希表的快速访问。**