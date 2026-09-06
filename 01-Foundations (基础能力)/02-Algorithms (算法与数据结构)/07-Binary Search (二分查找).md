---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# 30 秒回答

**核心结论**：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# Binary Search Boundaries (二分边界)

> [!note] 本节重点：核心考点：边界条件、三种模板的使用场景、二分答案

# 二分的本质

在**有序**（或具有单调性）的搜索空间中，每次排除一半，将 O(n) 降到 O(log n)。

---

# 三种模板

## 模板一：精确查找（找到即返回）

```cpp
int binarySearch(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;      // 防溢出写法
        if      (nums[mid] == target) return mid;
        else if (nums[mid] < target)  l = mid + 1;
        else                          r = mid - 1;
    }
    return -1;
}
```

循环条件 `l <= r`，退出时 `l > r`，搜索空间为空时停止。

---

## 模板二：查找左边界（第一个 ≥ target 的位置）

```cpp
// 第一个 >= target 的位置（lower_bound）
int lowerBound(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size();      // [l, r)
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] < target) l = mid + 1;
        else                    r = mid;   // 保留 mid，向左收缩
    }
    return l;
}
```

## 模板三：查找右边界（最后一个 ≤ target 的位置）

```cpp
// 最后一个 <= target 的位置（upper_bound - 1）
int upperBound(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] <= target) l = mid + 1;  // target 在 mid 右边
        else                     r = mid;
    }
    return l - 1;   // 最后一个 <= target 的下标
}
```

**C++ STL 对应：**

```cpp
#include <algorithm>
auto it = lower_bound(nums.begin(), nums.end(), target);  // 第一个 >= target 的迭代器
auto it = upper_bound(nums.begin(), nums.end(), target);  // 第一个 > target 的迭代器
// 返回值：it - nums.begin() 即为下标
// 若未找到，返回 nums.size()（越界下标）
```

> `lower_bound`/`upper_bound` 要求序列**有序**，二分查找，O(log n)。

---

# 二分答案（最重要的应用）

**适用场景：** 答案在某个范围内，且答案越大（或越小）越容易满足条件（单调性），可以二分答案，把"求最值"转化为"验证是否可行"。

**套路：**

1. 确定答案范围 `[lo, hi]`
2. 定义 `check(mid)` 判断 mid 是否可行
3. 二分找到满足/不满足条件的边界

```cpp
// 二分答案通用框架（求最小值）
int lo = minPossible, hi = maxPossible;
while (lo < hi) {
    int mid = lo + (hi - lo) / 2;
    if (check(mid)) hi = mid;        // mid 可行，尝试更小
    else            lo = mid + 1;    // mid 不可行，需要更大
}
return lo;
```

## 经典例题

**木材切割（二分最大长度）：**

```cpp
// 木材切割（求最大值：mid 上取整防死循环）
int maxPiece(vector<int>& lengths, int k) {
    auto check = [&](int mid) {
        int cnt = 0;
        for (int l : lengths) cnt += l / mid;
        return cnt >= k;
    };
    int lo = 1, hi = *max_element(lengths.begin(), lengths.end());
    while (lo < hi) {
        int mid = lo + (hi - lo + 1) / 2;  // 上取整
        if (check(mid)) lo = mid;
        else            hi = mid - 1;
    }
    return lo;
}
```

**爱吃香蕉的珂珂（最小速度）：**

```cpp
// 爱吃香蕉的珂珂（求最小值）
int minEatingSpeed(vector<int>& piles, int h) {
    auto check = [&](int speed) {
        long long cnt = 0;
        for (int p : piles) cnt += (p + speed - 1) / speed;  // 上取整
        return cnt <= h;
    };
    int lo = 1, hi = *max_element(piles.begin(), piles.end());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (check(mid)) hi = mid;        // 速度可以更小
        else            lo = mid + 1;
    }
    return lo;
}
```

**在 D 天内送达包裹的能力（最小运载能力）：**

```cpp
// 在 D 天内送达包裹的能力（求最小值）
int shipWithinDays(vector<int>& weights, int days) {
    auto check = [&](int cap) {
        int d = 1, cur = 0;
        for (int w : weights) {
            if (cur + w > cap) { d++; cur = 0; }
            cur += w;
        }
        return d <= days;
    };
    int lo = *max_element(weights.begin(), weights.end());
    int hi = accumulate(weights.begin(), weights.end(), 0);
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (check(mid)) hi = mid;
        else            lo = mid + 1;
    }
    return lo;
}
```

---

# 旋转数组中的二分

```cpp
// 搜索旋转排序数组（无重复）
int searchRotated(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[l] <= nums[mid]) {         // 左半段有序
            if (nums[l] <= target && target < nums[mid]) r = mid - 1;
            else                                         l = mid + 1;
        } else {                            // 右半段有序
            if (nums[mid] < target && target <= nums[r]) l = mid + 1;
            else                                         r = mid - 1;
        }
    }
    return -1;
}
```

---

# 二分常见陷阱

|陷阱|说明|解决|
|---|---|---|
|死循环|求最大值时 `mid=(lo+hi)//2`，`lo=mid` 可能不动|改为 `mid=(lo+hi+1)//2`|
|溢出（C++）|`(l+r)/2` 在 l、r 很大时溢出|用 `l + (r-l)/2`|
|边界判断|`<` 还是 `<=`，`+1` 还是不加|根据搜索空间开闭区间决定|

---

# 关联笔记

- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)

# 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

# 常见误区

- 把 **07-Binary Search (二分查找)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **07-Binary Search (二分查找)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
