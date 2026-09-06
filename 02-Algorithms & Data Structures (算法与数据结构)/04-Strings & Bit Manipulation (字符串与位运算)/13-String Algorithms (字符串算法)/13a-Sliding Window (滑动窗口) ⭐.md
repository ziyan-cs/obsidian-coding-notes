---
tags:
  - cs/algorithm
status: 🌱
---

# Sliding Window — 滑动窗口

> [!important] **核心考点**：滑动窗口模板（可变长度/固定长度）、窗口收缩条件判断、哈希表维护字符频次

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

## 经典题型速查

| 题型 | 窗口类型 | 收缩条件 | 结果更新时机 |
|------|---------|---------|------------|
| 无重复最长子串 | 可变 | 出现重复字符 | 每次 right 扩展后 |
| 最小覆盖子串 | 可变 | 已覆盖所有目标字符 | 收缩前（窗口合法时） |
| 长度最小子数组 | 可变 | sum ≥ target | 收缩过程中 |
| 字符串排列 | 固定 | 固定长度 len(t) | 窗口达到固定大小 |
| 找所有字母异位词 | 固定 | 固定长度 len(p) | Windows 符合条件时 |

> [!tip]- **工程要点**：滑动窗口本质是用双指针维护一个"可行解"区间，核心是两步——**右扩**进入新元素、**左缩**排出多余元素。注意更新结果的位置（收缩前还是收缩后）决定求最大值还是最小值。

---

## 关联笔记

- [KMP Pattern Matching (KMP算法)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/04-Strings%20&%20Bit%20Manipulation%20(字符串与位运算)/13-String%20Algorithms%20(字符串算法)/13b-KMP%20Pattern%20Matching%20(KMP算法).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
