---
tags:
  - cs/algorithm
status: 🌱
---

> **核心考点**：KMP next 数组构建、匹配过程 O(n+m)、next 数组含义（最长公共前后缀）

## KMP 核心思想

KMP 解决字符串匹配问题：在主串 `s` 中查找模式串 `p` 的所有出现位置。

**暴力法的缺陷：** 匹配失败时只右移一位，重复比较已匹配过的字符。KMP 利用**已匹配部分**的信息，让模式串跳过不可能的位置，直接从失配点继续匹配。

**核心：** 当 `s[i] != p[j]` 时，将 `j` 回退到 `next[j]`（前 `j` 个字符的最长公共前后缀长度），而不是从头开始。

## next 数组构建

`next[j]` 表示 `p[0..j-1]`（即前 j 个字符）中**最长相等前后缀的长度**。

```cpp
vector<int> buildNext(string& p) {
    int m = p.size();
    vector<int> next(m, 0);
    int j = 0;  // 前缀指针
    
    for (int i = 1; i < m; i++) {  // i 是后缀指针
        while (j > 0 && p[i] != p[j])
            j = next[j-1];         // 前后缀不匹配，前缀回退
        
        if (p[i] == p[j])
            j++;                   // 匹配成功，前缀长度+1
        
        next[i] = j;               // 记录前 i+1 个字符的最长公共前后缀
    }
    return next;
}
```

**理解 next 数组：**
- `next[0] = 0`：一个字符没有真前后缀
- `next[i] = k` 表示 `p[0..k-1] == p[i-k+1..i]`
- 回退时 `j = next[j-1]` 利用了已算好的前缀信息

## 匹配过程

```cpp
vector<int> kmp(string& s, string& p) {
    vector<int> next = buildNext(p);
    vector<int> res;
    int j = 0;
    
    for (int i = 0; i < s.size(); i++) {
        while (j > 0 && s[i] != p[j])
            j = next[j-1];         // 失配时利用 next 跳过
        
        if (s[i] == p[j])
            j++;                   // 当前字符匹配，继续
        
        if (j == p.size()) {       // 完全匹配
            res.push_back(i - j + 1);
            j = next[j-1];         // 继续找下一个匹配
        }
    }
    return res;
}
```

时间复杂度 O(n+m)，空间复杂度 O(m)。

## 手动推演示例

模式串 `p = "ABABAC"` 的 next 数组：

| i | p[i] | 前 i+1 个字符 | 最长公共前后缀 | next[i] |
|---|------|--------------|--------------|---------|
| 0 | A | A | 0 | 0 |
| 1 | B | AB | 0 | 0 |
| 2 | A | ABA | A | 1 |
| 3 | B | ABAB | AB | 2 |
| 4 | A | ABABA | ABA | 3 |
| 5 | C | ABABAC | 0 | 0 |

## 求最短循环节

**定理：** 如果字符串 `s` 由某个循环节重复构成，那么最小循环节长度 = `n - next[n-1]`，其中 n = s.length()。

```cpp
// 求最小循环节长度
int minCycle(string& s) {
    int n = s.size();
    vector<int> next = buildNext(s);
    int cycle = n - next[n-1];
    if (n % cycle == 0) return cycle;  // 完整周期
    return n;  // 不是周期性字符串
}
```

## 经典题型

| 题型 | 思路 |
|------|------|
| 在文本中查找模式串 | 标准 KMP |
| 最短循环节 | `n - next[n-1]` |
| 重复子串构成 | `next[n-1] > 0 && n % (n - next[n-1]) == 0` |
| 两个字符串的最长公共前后缀 | 拼接后求 next |
| 最短回文串（前面补） | 反转后求最长匹配前缀 |

> **工程要点**：KMP 的难点在理解 next 数组的含义——它是**模式串自身的匹配信息**。next 数组的构建过程本质上是模式串自己和自己做匹配。实际工程中字符串匹配多直接用语言内置 API 或 Boyer-Moore，KMP 更常见于算法面试和特定场景（如流式匹配）。

---

## 关联笔记

- [Sliding Window (滑动窗口)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/04%20·%20字符串与位运算/13-String%20Algorithms%20(字符串算法)/13a-Sliding%20Window%20(滑动窗口)%20⭐.md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
