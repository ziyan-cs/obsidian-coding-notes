---
tags:
  - cs/algorithm
status: 🌱
---

# Array & Two Pointers — 数组与双指针

> [!important] **核心考点**：双指针的四种模式、滑动窗口、前缀和

## 双指针四种模式

### 模式一：对撞指针（左右夹逼）

两指针从两端向中间移动，适合**有序数组**的搜索问题：

```cpp
// 有序数组两数之和
vector<int> twoSum(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;
    while (l < r) {
        int s = nums[l] + nums[r];
        if      (s == target) return {l, r};
        else if (s < target)  l++;
        else                  r--;
    }
    return {};
}
```

典型题：Two Sum II、三数之和、盛最多水的容器、回文判断。

### 模式二：快慢指针

两指针同向，速度不同，用于**链表环检测、找中点**（详见链表章节）。

### 模式三：滑动窗口

维护一个可变长度的窗口 `[l, r]`，右指针扩张，左指针收缩：

```cpp
// 长度最小的子数组（子数组和 >= target）
int minSubArrayLen(int target, vector<int>& nums) {
    int l = 0, total = 0, res = INT_MAX;
    for (int r = 0; r < (int)nums.size(); r++) {
        total += nums[r];
        while (total >= target) {
            res = min(res, r - l + 1);
            total -= nums[l++];
        }
    }
    return res == INT_MAX ? 0 : res;
}
```

**无重复字符的最长子串（变长窗口 + 哈希）：**

```cpp
// 无重复字符的最长子串
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> seen;
    int l = 0, res = 0;
    for (int r = 0; r < (int)s.size(); r++) {
        if (seen.count(s[r]) && seen[s[r]] >= l)
            l = seen[s[r]] + 1;
        seen[s[r]] = r;
        res = max(res, r - l + 1);
    }
    return res;
}
```

### 模式四：快速分区（原地操作）

```cpp
// 颜色分类 0/1/2，O(n) 时间 O(1) 空间
void sortColors(vector<int>& nums) {
    int lo = 0, mid = 0, hi = (int)nums.size() - 1;
    while (mid <= hi) {
        if      (nums[mid] == 0) swap(nums[lo++], nums[mid++]);
        else if (nums[mid] == 1) mid++;
        else                     swap(nums[mid], nums[hi--]);
    }
}
```

## 前缀和

```cpp
// 构建前缀和，O(1) 区间查询
vector<int> prefix(nums.size() + 1, 0);
for (int i = 0; i < (int)nums.size(); i++)
    prefix[i + 1] = prefix[i] + nums[i];

// 区间 [l, r] 的和
int rangeSum = prefix[r + 1] - prefix[l];
```

二维前缀和、差分数组是进阶变体，用于范围加减操作。

---

## 关联笔记

- [Hash Table (哈希表)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
