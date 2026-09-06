---
tags:
  - cs/algorithm
status: 🌱
---

# 06-Hash Table (哈希表)

> [!abstract] 核心考点：哈希原理、冲突解决、Python dict / C++ unordered_map 的特性

## 哈希表原理

将 key 通过哈希函数映射到数组下标，实现 O(1) 平均查找。

**冲突解决：**

- **链地址法（Chaining）**：每个槽保存一组冲突元素；Java `HashMap` 属于这一类（桶内结构是实现细节）
- **开放寻址法（Open Addressing）**：冲突时按探测序列寻找槽位；CPython 的 `dict` 采用开放寻址思路，具体探测与内存布局属于实现细节

**负载因子（Load Factor）= 已存元素 / 总槽数**。过高会使冲突增多；扩容阈值和 rehash 策略由具体容器实现决定，不能把某一实现的 `0.75` 当作通用规则。

> [!warning] 哈希表是平均 `O(1)`，不是严格保证 `O(1)`：冲突严重或遭遇恶意键时会退化。对外部可控的键和性能敏感路径，要考虑哈希质量、容量预留或抗攻击策略。

## 常见使用模式

```cpp
#include <unordered_map>
#include <unordered_set>

// 两数之和
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> seen;  // value → index
    for (int i = 0; i < (int)nums.size(); i++) {
        int need = target - nums[i];
        if (seen.count(need)) return {seen[need], i};
        seen[nums[i]] = i;
    }
    return {};
}

// 字母异位词分组
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;
    for (auto& s : strs) {
        string key = s;
        sort(key.begin(), key.end());
        groups[key].push_back(s);
    }
    vector<vector<string>> res;
    for (auto& [k, v] : groups) res.push_back(v);
    return res;
}

// 自定义哈希（pair 作 key）
struct PairHash {
    size_t operator()(const pair<int,int>& p) const {
        size_t h1 = hash<int>{}(p.first);
        size_t h2 = hash<int>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
unordered_map<pair<int,int>, int, PairHash> mp;
```

## C++ unordered_map

```cpp
#include <unordered_map>
unordered_map<string, int> freq;
freq["hello"]++;
freq.count("hello");      // 是否存在（0 或 1）
freq.find("world");       // 返回迭代器
freq.erase("hello");

// 自定义哈希（对 pair 等非内置类型）
struct PairHash {
    size_t operator()(const pair<int,int>& p) const {
        size_t h1 = hash<int>{}(p.first);
        size_t h2 = hash<int>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
unordered_map<pair<int,int>, int, PairHash> mp;
```

---

## 30 秒回答

**哈希表为什么快、又为什么不保证绝对快？** 哈希函数把键分散到桶或槽位，查找平均只需常数次探测；冲突会拉长探测链路，极端输入可能退化。使用 `unordered_map` 时，迭代顺序无保证；插入导致 rehash 后，迭代器可能失效。

**自测：** 链地址法与开放寻址法分别怎样处理冲突？什么时候应先 `reserve()`？

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
