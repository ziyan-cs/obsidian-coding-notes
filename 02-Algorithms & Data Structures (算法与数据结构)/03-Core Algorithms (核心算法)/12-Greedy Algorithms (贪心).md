---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：贪心局部最优推全局最优、排序预处理、区间调度、经典贪心问题证明思路

## 贪心核心思想

每一步都做出当前看起来最优的选择，希望最终结果全局最优。

**适用条件：**
- **贪心选择性质**：全局最优可以通过一系列局部最优得到
- **最优子结构**：子问题的最优解可以推导出原问题的最优解

**与 DP 的关系：** 贪心是 DP 的特例——每个子问题只做一个选择（无需考虑多种可能性）。能用贪心解决的问题一定可以用 DP 解决，但反之不一定。

---

## 区间调度类

### 无重叠区间（选最多不重叠区间）

按**结束时间**排序，优先选结束早的：

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
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
    return jumps;
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

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
