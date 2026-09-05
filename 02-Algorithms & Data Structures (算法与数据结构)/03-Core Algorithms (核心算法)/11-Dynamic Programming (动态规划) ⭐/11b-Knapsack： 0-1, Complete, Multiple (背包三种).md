---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：0-1 背包逆序遍历容量、完全背包正序遍历、多重背包二进制优化、恰好装满初始化

## 背包问题核心框架

背包问题的本质：给定一组物品（每个有重量和价值），在容量限制下最大化总价值。

**核心三要素：**
1. 物品数量（n）
2. 背包容量（cap）
3. 每种物品可选几次（0-1次 / 无限次 / 有限次）

---

## 0-1 背包（每种物品最多选一次）

```cpp
int knapsack01(vector<int>& w, vector<int>& v, int cap) {
    int n = w.size();
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < n; i++) {
        for (int j = cap; j >= w[i]; j--) {     // 逆序：确保每个物品只用一次
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
        }
    }
    return dp[cap];
}
```

**为什么逆序？** `dp[j - w[i]]` 必须是**未考虑当前物品 i** 的状态。正序遍历时，容量小的位置已经被当前物品更新过，可能导致同一物品被多次使用。

**恰好装满：** 初始化 `dp[0] = 0`，其余 `dp[j] = -INF`，只有能从 0 转移过来的状态才有效。

---

## 完全背包（每种物品无限次）

```cpp
int knapsackComplete(vector<int>& w, vector<int>& v, int cap) {
    int n = w.size();
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < n; i++) {
        for (int j = w[i]; j <= cap; j++) {     // 正序：允许同一物品多次使用
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
        }
    }
    return dp[cap];
}
```

**完全背包与 0-1 的唯一区别：** 容量遍历方向相反。

**零钱兑换系列：**

```cpp
// 零钱兑换 I（求最少硬币数）
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, amount + 1);
    dp[0] = 0;
    for (int coin : coins)
        for (int j = coin; j <= amount; j++)
            dp[j] = min(dp[j], dp[j - coin] + 1);
    return dp[amount] > amount ? -1 : dp[amount];
}

// 零钱兑换 II（求组合数）
int change(int amount, vector<int>& coins) {
    vector<int> dp(amount + 1, 0);
    dp[0] = 1;
    for (int coin : coins)                     // 先遍历物品 — 组合（不考虑顺序）
        for (int j = coin; j <= amount; j++)
            dp[j] += dp[j - coin];
    return dp[amount];
}
// 若先遍历容量再遍历物品，结果为排列数（考虑顺序）
```

---

## 多重背包（每种物品有限次）

### 解法一：暴力拆成 0-1（容易超时）

```cpp
int knapsackMultiple(vector<int>& w, vector<int>& v, vector<int>& cnt, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < w.size(); i++) {
        for (int k = 0; k < cnt[i]; k++) {             // 拆成 cnt[i] 个独立物品
            for (int j = cap; j >= w[i]; j--) {
                dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
            }
        }
    }
    return dp[cap];
}
// 时间复杂度 O(cap * sum(cnt))，数据量大时会超时
```

### 解法二：二进制优化（推荐）

将 cnt 个物品拆成 **1, 2, 4, ..., 2^k, 剩余** 个一组，每组作为一个 0-1 物品，覆盖 0..cnt 的所有组合：

```cpp
int knapsackMultipleOpt(vector<int>& w, vector<int>& v, vector<int>& cnt, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < w.size(); i++) {
        int num = cnt[i];
        // 二进制拆分
        for (int k = 1; num > 0; k <<= 1) {
            int take = min(k, num);
            int weight = take * w[i];
            int value  = take * v[i];
            for (int j = cap; j >= weight; j--)
                dp[j] = max(dp[j], dp[j - weight] + value);
            num -= take;
        }
    }
    return dp[cap];
}
```

---

## 背包问题对比

| 类型 | 容量遍历 | 物品遍历 | 时间复杂度 | 空间优化后 |
|------|---------|---------|-----------|-----------|
| 0-1 背包 | 逆序 | 外循环物品 | O(n·cap) | O(cap) |
| 完全背包 | 正序 | 外循环物品 | O(n·cap) | O(cap) |
| 多重背包（暴力） | 逆序 | 三层循环 | O(cap·Σcnt) | O(cap) |
| 多重背包（二进制） | 逆序 | 拆后 0-1 | O(cap·Σlog(cnt)) | O(cap) |

## 背包问题常见变体

| 变体 | 解法要点 |
|------|---------|
| 求方案数 | `dp[j] += dp[j - w[i]]` |
| 求能否装满 | `dp[j] = dp[j] \|\| dp[j - w[i]]` |
| 恰好装满 | dp 初始化为 -INF，`dp[0] = 0` |
| 二维费用 | `dp[j][k]` 两个维度都遍历容量 |
| 分组背包 | 每组选一个，组内遍历时容量在外、物品在内 |
| 依赖背包 | 转化为树形 DP + 分组背包 |

> [!tip]- **工程要点**：背包问题的核心就两点——**物品在外还是容量在外**决定是组合还是排列，**容量正序还是逆序**决定是完全还是 0-1。面试时先写出暴力二维版本再优化为一维，不易出错。

---

## 关联笔记

- [1D DP： Climbing Stairs, House Robber (线性DP)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11a-1D%20DP：%20Climbing%20Stairs,%20House%20Robber%20(线性DP).md)
- [Interval DP (区间DP)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11c-Interval%20DP%20(区间DP).md)
- [LCS & LIS (最长子序列类)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11d-LCS%20&%20LIS%20(最长子序列类).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
