---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 11-String and Bit Algorithms (字符串与位运算)

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

## KMP Pattern Matching (KMP 模式匹配)

> [!note] 本节重点：核心考点：KMP next 数组构建、匹配过程 O(n+m)、next 数组含义（最长公共前后缀）

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

> [!tip]- **工程要点**：KMP 的难点在理解 next 数组的含义——它是**模式串自身的匹配信息**。next 数组的构建过程本质上是模式串自己和自己做匹配。实际工程中字符串匹配多直接用语言内置 API 或 Boyer-Moore，KMP 更常见于算法面试和特定场景（如流式匹配）。

---

## 关联笔记

- [Sliding Window (滑动窗口)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/04-Strings%20&%20Bit%20Manipulation%20(字符串与位运算)/13-String%20Algorithms%20(字符串算法)/13a-Sliding%20Window%20(滑动窗口)%20⭐.md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)

---

## Bit Manipulation (位运算技巧)

> [!note] 本节重点：核心考点：位运算基本操作、常用位技巧、Brian Kernighan 算法、^ 异或的妙用、掩码与状态压缩

## 位运算基础

| 操作 | 运算符 | 说明 |
|------|--------|------|
| 按位与 | `a & b` | 同 1 为 1 |
| 按位或 | `a \| b` | 有 1 为 1 |
| 按位异或 | `a ^ b` | 不同为 1 |
| 按位取反 | `~a` | 0 变 1，1 变 0 |
| 左移 | `a << k` | 对无符号整数可理解为乘以 2^k（未溢出时） |
| 右移 | `a >> k` | 无符号整数右移为逻辑右移；有符号负数的结果不要依赖 |

**优先级注意：** 位运算优先级低于比较运算符，使用时多写括号。

---

## 常用位操作技巧

### 基本操作

```cpp
// 取出最低位的 1
int lowbit = x & -x;              // 常用于树状数组

// 将最低位的 1 置 0
x = x & (x - 1);                  // Brian Kernighan 算法核心

// 判断某一位是否为 1
bool bit = (x >> k) & 1;

// 将第 k 位设为 1
x = x | (1 << k);

// 将第 k 位设为 0
x = x & ~(1 << k);

// 翻转第 k 位
x = x ^ (1 << k);

// 判断是否为 2 的幂
bool isPowerOfTwo = (x > 0) && (x & (x - 1)) == 0;

// 判断奇偶
bool odd = x & 1;
```

### 异或的妙用

```cpp
// 交换两个数：生产代码优先 std::swap(a, b)。
// XOR 交换可读性差，且 a、b 指向同一对象时会出错。

// 数组中唯一出现一次的数（其他出现两次）
int singleNumber(vector<int>& nums) {
    int res = 0;
    for (int x : nums) res ^= x;
    return res;
}

// 找两个唯一出现一次的数（其他出现两次）
vector<int> singleNumberII(vector<int>& nums) {
    int xorAll = 0;
    for (int x : nums) xorAll ^= x;
    int lowbit = xorAll & -xorAll;     // 找到第一个不同的位
    int a = 0, b = 0;
    for (int x : nums) {
        if (x & lowbit) a ^= x;
        else            b ^= x;
    }
    return {a, b};
}

// 缺失的数字（0..n 中缺一个）
int missingNumber(vector<int>& nums) {
    int res = nums.size();
    for (int i = 0; i < nums.size(); i++)
        res ^= i ^ nums[i];
    return res;
}
```

### 位图与子集枚举

```cpp
// 用 bitset 统计出现次数（每个数出现次数 mod 3）
int singleNumberIII(vector<int>& nums) {
    int ones = 0, twos = 0;
    for (int x : nums) {
        ones = (ones ^ x) & ~twos;
        twos = (twos ^ x) & ~ones;
    }
    return ones;
}

// 枚举集合的所有子集（状态压缩）
vector<vector<int>> subsets(vector<int>& nums) {
    int n = nums.size();
    vector<vector<int>> res;
    // n 不能达到 int 的位宽；大集合的子集数本身已是指数级。
    for (int mask = 0; mask < (1 << n); mask++) {
        vector<int> sub;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) sub.push_back(nums[i]);
        res.push_back(sub);
    }
    return res;
}

// 枚举子集的子集（DP 优化常用）
for (int sub = mask; sub; sub = (sub - 1) & mask) {
    // 处理 sub
}
```

---

## Brian Kernighan 算法

用于统计二进制中 1 的个数，每次将最低位的 1 置零：

```cpp
int countBits(int x) {
    int cnt = 0;
    while (x) {
        x &= x - 1;      // 去掉最低位的 1
        cnt++;
    }
    return cnt;
}
// 时间复杂度 O(number of 1 bits)，而非 O(total bits)
```

---

## 常用掩码

```cpp
// 取低 k 位
x & ((1 << k) - 1)

// ASCII 英文字母可用位技巧转换；通用文本请用合适的字符处理方案。
if ('A' <= ch && ch <= 'Z') ch = ch | ' ';
if ('a' <= ch && ch <= 'z') ch = ch & '_';

// 英文字母位图（检查是否出现过）
int bitmap = 0;
for (char c : s)
    bitmap |= 1 << (c - 'a');

// 只保留字母数字
if (isalnum(ch)) ...
```

---

## 经典题型速查

| 题型 | 解法 |
|------|------|
| 只出现一次的数 | 全部异或 |
| 两个唯一出现数 | 异或全部 → lowbit 分组 |
| 缺失数字 | 异或下标 + 值 |
| 2 的幂 | `x > 0 && (x & (x-1)) == 0` |
| 4 的幂 | 2 的幂 + `x % 3 == 1` |
| 二进制 1 的个数 | `x & (x-1)` 循环 |
| 反转二进制位 | 逐位移动或分治 |
| 子集枚举 | `mask` 从 0 到 (1<<n)-1 |
| 两数之和（不用 +/-） | 位运算模拟加法 |
| 数组中每个元素出现 3 次 | `ones/twos` 有限状态机 |

> [!tip]- **工程要点**：位运算极其高效，面试中解"出现次数"类问题的利器是**异或**（出现两次）和**有限状态自动机**（出现三次）。`x & (-x)` 取最低位 1 是树状数组和许多位运算题的基础。

## 30 秒回答

**位运算最容易错在哪里？** 先明确类型与位宽：涉及移位、掩码和 `lowbit` 时优先使用无符号整数，避免移位位数越界和依赖负数右移。异或适合成对抵消；它不是“无临时变量交换”的工程推荐方案。

**自测：** `x & (x - 1)` 对正整数做了什么？为什么 `(1 << n)` 不适合 `n` 接近 `int` 位宽的场景？

---

## 关联笔记 · 延伸要点 2
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)



## 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

## 常见误区

- 把 **11-String and Bit Algorithms (字符串与位运算)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **11-String and Bit Algorithms (字符串与位运算)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
