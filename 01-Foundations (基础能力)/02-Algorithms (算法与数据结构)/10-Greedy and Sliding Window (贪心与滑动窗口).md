---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 10-Greedy and Sliding Window (贪心与滑动窗口)

> [!abstract] 阅读定位
>
> 本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

## Greedy and Interval Algorithms (贪心与区间算法)

> [!note] 本节重点心考点：贪心局部最优推全局最优、排序预处理、区间调度、经典贪心问题证明思路

## 贪心核心思想

每一步都做出当前看起来最优的选择，希望最终结果全局最优。

**适用条件：**
- **贪心选择性质**：全局最优可以通过一系列局部最优得到
- **最优子结构**：子问题的最优解可以推导出原问题的最优解

**与 DP 的关系：** DP 会比较并保留多个状态；贪心只保留当前选择。两者都利用子问题结构，但贪心必须额外证明局部选择不会排除全局最优，不能仅凭“看起来更省事”替代 DP。

---

## 区间调度类

### 无重叠区间（选最多不重叠区间）

按**结束时间**排序，优先选结束早的：

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;
    sort(intervals.begin(), intervals.end(),
         [](auto& a, auto& b) { return a[1] < b[1]; });
    int count = 1, end = intervals[0][1];
    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] >= end) {   // 不重叠
            count++;
            end = intervals[i][1];
        }
    }
    return intervals.size() - count;    // 最少删除数
}
```

**选择策略：** 按结束时间最早 → 为后面留更多空间 → 能选最多区间。

### 合并区间

按**开始时间**排序，合并重叠区间：

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> res;
    for (auto& interval : intervals) {
        if (res.empty() || interval[0] > res.back()[1])
            res.push_back(interval);                     // 新区间
        else
            res.back()[1] = max(res.back()[1], interval[1]);  // 合并
    }
    return res;
}
```

### 插入区间

```cpp
vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
    vector<vector<int>> res;
    int i = 0, n = intervals.size();
    while (i < n && intervals[i][1] < newInterval[0])
        res.push_back(intervals[i++]);               // 左边不重叠部分
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = min(newInterval[0], intervals[i][0]);
        newInterval[1] = max(newInterval[1], intervals[i][1]);
        i++;
    }
    res.push_back(newInterval);                      // 合并后的新区间
    while (i < n) res.push_back(intervals[i++]);     // 右边不重叠部分
    return res;
}
```

---

## 跳跃游戏类

### 跳跃游戏（能否到达末尾）

```cpp
bool canJump(vector<int>& nums) {
    int maxReach = 0;
    for (int i = 0; i <= maxReach && i < nums.size(); i++) {
        maxReach = max(maxReach, i + nums[i]);
        if (maxReach >= nums.size() - 1) return true;
    }
    return false;
}
```

### 跳跃游戏 II（最少步数）

```cpp
int jump(vector<int>& nums) {
    int jumps = 0, curEnd = 0, farthest = 0;
    for (int i = 0; i < nums.size() - 1; i++) {
        farthest = max(farthest, i + nums[i]);
        if (i == curEnd) {             // 到达当前跳的最远边界
            jumps++;
            curEnd = farthest;
        }
    }
    return curEnd >= (int)nums.size() - 1 ? jumps : -1;  // 输入可能不可达时显式返回
}
```

---

## 分配问题类

### 分发饼干

```cpp
int findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(), g.end());
    sort(s.begin(), s.end());
    int i = 0;
    for (int j = 0; i < g.size() && j < s.size(); j++) {
        if (s[j] >= g[i]) i++;   // 最小饼干喂给胃口最小的孩子
    }
    return i;
}
```

### 加油站

```cpp
int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
    int total = 0, cur = 0, start = 0;
    for (int i = 0; i < gas.size(); i++) {
        total += gas[i] - cost[i];
        cur += gas[i] - cost[i];
        if (cur < 0) { cur = 0; start = i + 1; }  // 无法到达下一站
    }
    return total >= 0 ? start : -1;
}
```

---

## 经典题型速查

| 题型 | 贪心策略 | 排序方式 |
|------|---------|---------|
| 无重叠区间 | 优先选结束早的 | 按 end 升序 |
| 合并区间 | 有重叠就合并 | 按 start 升序 |
| 跳跃游戏 | 维护最远可达位置 | 无需排序 |
| 分发饼干 | 最小饼干喂最小胃口 | 两数组排序 |
| 加油站 | 总油量不足必无解 | 一次遍历 |
| 最大子数组和 | cur < 0 时就抛弃 | 无需排序 |
| 任务调度器 | 优先安排频率最高的 | 按频率排序 |
| 哈夫曼编码 | 每次合并最小的两个 | 最小堆 |
| 最少箭矢戳气球 | 按结束位置射箭 | 按 end 升序 |

> [!tip]- **工程要点**：面试中的贪心题通常需要先尝试**排序**或**堆**来辅助决策。证明贪心正确性常用**交换论证法**——假设最优解与贪心解不同，交换后不会使解变差。如果不确定能否贪心，先想 DP 再找贪心特征。

## 30 秒回答

**怎样判断能不能贪心？** 先写出每一步的局部选择，再用交换论证或不变式证明：任一最优解都能替换成该选择而不变差。区间调度按最早结束时间就是典型；若无法证明，保留多个可能状态的 DP 往往更可靠。

**自测：** 为什么区间调度按开始时间早并不保证最优？跳跃游戏 II 的线性写法为什么需要题目保证可达，或显式处理不可达？

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)

---

## Sliding Window (滑动窗口)

> [!note] 本节重点心考点：滑动窗口模板（可变长度/固定长度）、窗口收缩条件判断、哈希表维护字符频次

## 滑动窗口核心思想

维护一个窗口 `[left, right)`，右指针扩展窗口，左指针收缩窗口。每个元素最多进出窗口各一次，**时间复杂度 O(n)**。

**适用场景：** 连续子数组/子串问题、需要维护一个动态集合的问题。

## 可变长度窗口（求最值）

```cpp
// 求最长/最小满足条件的子数组/子串
int slidingWindow(vector<int>& nums) {
    int left = 0, right = 0;
    int res = 0;  // 或 INT_MAX（求最小值时）
    unordered_map<int, int> window;  // 根据问题选择容器
    
    while (right < nums.size()) {
        // 1. 扩展右边界
        int add = nums[right];
        window[add]++;
        right++;
        
        // 2. 收缩左边界（条件不满足时）
        while (/* 窗口不再满足条件 */) {
            int remove = nums[left];
            window[remove]--;
            left++;
        }
        
        // 3. 更新结果（收缩后窗口合法）
        res = max(res, right - left);  // 或 min
    }
    return res;
}
```

## 固定长度窗口（滑动）

```cpp
// 固定窗口大小 k，求窗口内的最大值/平均值等
int fixedWindow(vector<int>& nums, int k) {
    int left = 0, right = 0;
    int sum = 0, res = 0;
    
    while (right < nums.size()) {
        sum += nums[right];
        right++;
        
        // 窗口达到固定大小后，移动
        if (right - left == k) {
            res = max(res, sum);
            sum -= nums[left];
            left++;
        }
    }
    return res;
}
```

## 无重复字符的最长子串

```cpp
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> freq;
    int left = 0, res = 0;
    
    for (int right = 0; right < s.size(); right++) {
        freq[s[right]]++;
        
        while (freq[s[right]] > 1) {        // 出现重复字符
            freq[s[left]]--;
            left++;
        }
        
        res = max(res, right - left + 1);
    }
    return res;
}
```

## 最小覆盖子串

```cpp
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    
    int left = 0, right = 0;
    int valid = 0;                         // 窗口中已满足的字符数
    int start = 0, len = INT_MAX;
    
    while (right < s.size()) {
        char add = s[right];
        right++;
        if (need.count(add)) {
            window[add]++;
            if (window[add] == need[add]) valid++;
        }
        
        while (valid == need.size()) {     // 窗口已覆盖 t
            if (right - left < len) {      // 更新最优解
                start = left;
                len = right - left;
            }
            char remove = s[left];
            left++;
            if (need.count(remove)) {
                if (window[remove] == need[remove]) valid--;
                window[remove]--;
            }
        }
    }
    return len == INT_MAX ? "" : s.substr(start, len);
}
```

## 长度最小的子数组

```cpp
int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, sum = 0, res = INT_MAX;
    
    for (int right = 0; right < nums.size(); right++) {
        sum += nums[right];
        
        while (sum >= target) {            // 满足条件时尽量缩
            res = min(res, right - left + 1);
            sum -= nums[left];
            left++;
        }
    }
    return res == INT_MAX ? 0 : res;
}
```

## 经典题型速查 · 延伸要点 2
| 题型 | 窗口类型 | 收缩条件 | 结果更新时机 |
|------|---------|---------|------------|
| 无重复最长子串 | 可变 | 出现重复字符 | 每次 right 扩展后 |
| 最小覆盖子串 | 可变 | 已覆盖所有目标字符 | 收缩前（窗口合法时） |
| 长度最小子数组 | 可变 | sum ≥ target | 收缩过程中 |
| 字符串排列 | 固定 | 固定长度 len(t) | 窗口达到固定大小 |
| 找所有字母异位词 | 固定 | 固定长度 len(p) | Windows 符合条件时 |

> [!tip]- **工程要点**：滑动窗口本质是用双指针维护一个"可行解"区间，核心是两步——**右扩**进入新元素、**左缩**排出多余元素。注意更新结果的位置（收缩前还是收缩后）决定求最大值还是最小值。

---

## 关联笔记 · 延伸要点 2
- [KMP Pattern Matching (KMP算法)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/04-Strings%20&%20Bit%20Manipulation%20(字符串与位运算)/13-String%20Algorithms%20(字符串算法)/13b-KMP%20Pattern%20Matching%20(KMP算法).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)



## 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

## 常见误区

- 把 **10-Greedy and Sliding Window (贪心与滑动窗口)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **10-Greedy and Sliding Window (贪心与滑动窗口)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
