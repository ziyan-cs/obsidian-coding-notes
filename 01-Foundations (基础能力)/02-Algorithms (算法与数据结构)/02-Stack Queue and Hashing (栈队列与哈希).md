---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# Monotonic Stack (单调栈)

> [!note] 本节重点：核心考点：单调递增/递减栈维护、下一个更大/更小元素 O(n) 模板、每日温度/接雨水等经典问题


单调栈维护一个**严格单调递增或递减**的栈，用于解决"下一个更大/更小元素"类问题，时间复杂度 O(n)（每个元素最多入栈出栈一次）。

# 模板：下一个更大元素

```cpp
vector<int> nextGreaterElement(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;  // 存下标
    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[i] > nums[st.top()]) {
            res[st.top()] = nums[i];
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```

# 典型应用

## 接雨水

```cpp
int trap(vector<int>& height) {
    stack<int> st;
    int water = 0;
    for (int i = 0; i < (int)height.size(); i++) {
        while (!st.empty() && height[i] > height[st.top()]) {
            int top = st.top(); st.pop();
            if (st.empty()) break;
            int width   = i - st.top() - 1;
            int bounded = min(height[i], height[st.top()]) - height[top];
            water += width * bounded;
        }
        st.push(i);
    }
    return water;
}
```

## 柱状图中最大矩形（单调递增栈）

```cpp
int largestRectangleArea(vector<int>& heights) {
    vector<int> h = heights;
    h.push_back(0);             // 副本上的哨兵，避免修改调用者输入
    stack<int> st;
    st.push(-1);
    int res = 0;
    for (int i = 0; i < (int)h.size(); i++) {
        while (st.top() != -1 && h[i] < h[st.top()]) {
            int height = h[st.top()]; st.pop();
            int w = i - st.top() - 1;
            res = max(res, height * w);
        }
        st.push(i);
    }
    return res;
}
```

---

# 30 秒回答

**单调栈为什么是 O(n)？** 每个下标最多入栈一次、弹栈一次，虽然有嵌套 `while`，总弹栈次数仍不超过 `n`。先决定栈内维持递增还是递减，再明确“当前元素到来时，谁的答案被确定”。

**自测：** 下一个更大元素为何在弹栈时确定答案？柱状图题为什么要在末尾放哨兵？

# 关联笔记

- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)

---

# Queue and Breadth First Search (队列与广度优先搜索)

> [!note] 本节重点：核心考点：BFS 按层扩展、队列实现、无权图最短路径、visited 数组防重复


BFS 用队列实现，**按层扩展**，保证找到的路径是最短路径（无权图）。

## 模板

```cpp
int bfs(vector<vector<int>>& graph, int start, int target) {
    queue<int> q;
    unordered_set<int> visited;
    q.push(start);
    visited.insert(start);
    int steps = 0;
    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            int node = q.front(); q.pop();
            if (node == target) return steps;
            for (int nb : graph[node])
                if (!visited.count(nb)) { visited.insert(nb); q.push(nb); }
        }
        steps++;
    }
    return -1;
}
```

## 二叉树层序遍历

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    queue<TreeNode*> q;
    q.push(root);
    vector<vector<int>> res;
    while (!q.empty()) {
        int sz = q.size();
        vector<int> level;
        for (int i = 0; i < sz; i++) {
            auto node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
        res.push_back(level);
    }
    return res;
}
```

## 多源 BFS

从多个起点同时出发（如矩阵中所有 0 同时扩散），初始化时将所有起点入队：

```cpp
vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    int m = mat.size(), n = mat[0].size();
    vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
    queue<pair<int,int>> q;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (mat[i][j] == 0) { dist[i][j] = 0; q.push({i, j}); }
    int dirs[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (auto& d : dirs) {
            int nx = x+d[0], ny = y+d[1];
            if (nx>=0 && nx<m && ny>=0 && ny<n && dist[nx][ny] > dist[x][y]+1) {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }
    return dist;
}
```

---

# 关联笔记 · 延伸要点 2
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)

---

# Hash Table (哈希表)

> [!note] 本节重点：核心考点：哈希原理、冲突解决、Python dict / C++ unordered_map 的特性

# 哈希表原理

将 key 通过哈希函数映射到数组下标，实现 O(1) 平均查找。

**冲突解决：**

- **链地址法（Chaining）**：每个槽保存一组冲突元素；Java `HashMap` 属于这一类（桶内结构是实现细节）
- **开放寻址法（Open Addressing）**：冲突时按探测序列寻找槽位；CPython 的 `dict` 采用开放寻址思路，具体探测与内存布局属于实现细节

**负载因子（Load Factor）= 已存元素 / 总槽数**。过高会使冲突增多；扩容阈值和 rehash 策略由具体容器实现决定，不能把某一实现的 `0.75` 当作通用规则。

> [!warning] 哈希表是平均 `O(1)`，不是严格保证 `O(1)`：冲突严重或遭遇恶意键时会退化。对外部可控的键和性能敏感路径，要考虑哈希质量、容量预留或抗攻击策略。

# 常见使用模式

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

# C++ unordered_map

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

# 关联笔记 · 延伸要点 3
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)



# 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

# 常见误区

- 把 **02-Stack Queue and Hashing (栈队列与哈希)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **02-Stack Queue and Hashing (栈队列与哈希)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
