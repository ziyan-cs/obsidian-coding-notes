---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：哈希原理、冲突解决、Python dict / C++ unordered_map 的特性

## 哈希表原理

将 key 通过哈希函数映射到数组下标，实现 O(1) 平均查找。

**冲突解决：**

- **链地址法（Chaining）**：每个槽存链表，冲突元素追加到链表（Python dict、Java HashMap）
- **开放寻址法（Open Addressing）**：冲突时线性探测/二次探测找下一个空槽（Python 3.6+ dict 使用紧凑实现）

**负载因子（Load Factor）= 已存元素 / 总槽数**，超过阈值（通常 0.75）触发扩容（rehash）。

## 常见使用模式

python

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
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 16);
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
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 16);
    }
};
unordered_map<pair<int,int>, int, PairHash> mp;
```

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
