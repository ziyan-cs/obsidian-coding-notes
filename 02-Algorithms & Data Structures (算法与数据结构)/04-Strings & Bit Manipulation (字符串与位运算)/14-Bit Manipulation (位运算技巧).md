---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：位运算基本操作、常用位技巧、Brian Kernighan 算法、^ 异或的妙用、掩码与状态压缩

## 位运算基础

| 操作 | 运算符 | 说明 |
|------|--------|------|
| 按位与 | `a & b` | 同 1 为 1 |
| 按位或 | `a \| b` | 有 1 为 1 |
| 按位异或 | `a ^ b` | 不同为 1 |
| 按位取反 | `~a` | 0 变 1，1 变 0 |
| 左移 | `a << k` | 相当于乘以 2^k |
| 右移 | `a >> k` | 相当于除以 2^k（向下取整） |

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
// 交换两个数（无需临时变量）
a ^= b; b ^= a; a ^= b;

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

// 大写转小写
ch |= ' ';           // 'A' | ' ' = 'a'

// 小写转大写
ch &= '_';           // 'a' & '_' = 'A'

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

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
