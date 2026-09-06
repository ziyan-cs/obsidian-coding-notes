---
tags:
  - cs/algorithm
status: 🌱
---

# Interval DP — 区间DP

> [!important] **核心考点**：区间 DP 的状态定义 `dp[i][j]` 表示区间 [i,j] 的最优解、长度递增枚举、石子合并/矩阵链乘、四边形不等式优化

## 区间 DP 核心思想

区间 DP 解决的是**在一段区间上做决策**的问题，大区间依赖于小区间的最优解。

**基本模板：**

```cpp
for (int len = 2; len <= n; len++) {                // 枚举区间长度
    for (int i = 1; i + len - 1 <= n; i++) {        // 枚举起点
        int j = i + len - 1;                        // 终点
        dp[i][j] = INF;
        for (int k = i; k < j; k++) {               // 枚举分割点
            dp[i][j] = min(dp[i][j], dp[i][k] + dp[k+1][j] + cost(i,j,k));
        }
    }
}
```

**关键特征：**
- 状态 `dp[i][j]` 表示区间 `[i, j]` 上的最优值
- 长度递增枚举——先处理短区间，大区间依赖小区间
- 枚举分割点 `k` 将区间一分为二
- 复杂度通常 O(n³)

## 石子合并

一排石子，相邻两堆可合并，代价为两堆重量和，求最小总代价。

```cpp
int mergeStones(vector<int>& stones) {
    int n = stones.size();
    if (n == 0) return 0;
    vector<int> prefix(n + 1, 0);
    for (int i = 1; i <= n; i++)
        prefix[i] = prefix[i-1] + stones[i-1];
    
    vector<vector<int>> dp(n+1, vector<int>(n+1, 0));
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i + len - 1 <= n; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; k++) {
                int sum = prefix[j] - prefix[i-1];  // 本次合并的额外代价
                dp[i][j] = min(dp[i][j], dp[i][k] + dp[k+1][j] + sum);
            }
        }
    }
    return dp[1][n];
}
```

**代价理解：** `prefix[j] - prefix[i-1]` 是最后一次合并时把 [i,k] 和 [k+1,j] 两堆合起来的重量总和——无论分割点在哪，合并区间 [i,j] 的总重量都一样，所以作为固定成本。

## 矩阵链乘

给定矩阵维度序列，求最小乘法次数。

```cpp
int matrixChain(vector<int>& dims) {
    if (dims.size() < 2) return 0;
    int n = dims.size() - 1;  // 矩阵个数
    vector<vector<int>> dp(n+1, vector<int>(n+1, 0));
    
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i + len - 1 <= n; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; k++) {
                dp[i][j] = min(dp[i][j],
                    dp[i][k] + dp[k+1][j] + dims[i-1]*dims[k]*dims[j]);
            }
        }
    }
    return dp[1][n];
}
```

## 回文分割

最少分割次数使每个子串都是回文。

```cpp
int minCut(string s) {
    int n = s.size();
    if (n == 0) return 0;
    vector<vector<bool>> isPal(n, vector<bool>(n, false));
    vector<int> dp(n, INT_MAX);
    
    for (int j = 0; j < n; j++) {
        dp[j] = j;  // 最坏情况：每个字符一刀
        for (int i = 0; i <= j; i++) {
            if (s[i] == s[j] && (j - i <= 2 || isPal[i+1][j-1])) {
                isPal[i][j] = true;
                if (i == 0) dp[j] = 0;
                else dp[j] = min(dp[j], dp[i-1] + 1);
            }
        }
    }
    return dp[n-1];
}
```

## 四边形不等式优化

Knuth 优化要求问题满足特定的四边形不等式/决策单调性等条件，才能把分割点搜索范围缩为 `opt[i][j-1]..opt[i+1][j]`；**不能只因看到区间 DP 就套用**。应先证明条件，并妥善初始化边界与 `opt`。

```cpp
for (int len = 2; len <= n; len++) {
    for (int i = 1; i + len - 1 <= n; i++) {
        int j = i + len - 1;
        dp[i][j] = INT_MAX;
        for (int k = s[i][j-1]; k <= s[i+1][j]; k++) {  // 缩小范围
            int val = dp[i][k] + dp[k+1][j] + cost(i,j);
            if (val < dp[i][j]) {
                dp[i][j] = val;
                s[i][j] = k;  // 记录最优决策点
            }
        }
    }
}
```

## 经典题型速查

| 题型 | 状态定义 | 转移 | 复杂度 |
|------|---------|------|--------|
| 石子合并 | `dp[i][j]` 最小合并代价 | `+ sum(i,j)` | O(n³) |
| 矩阵链乘 | `dp[i][j]` 最小乘法次数 | `+ d[i-1]*d[k]*d[j]` | O(n³) |
| 回文分割 | `dp[i]` 前 i 个字符最小分割 | `= min(dp[i-1] + 1)` | O(n²) |
| 戳气球 | `dp[i][j]` (i,j) 内最大收益 | `+ nums[i]*nums[k]*nums[j]` | O(n³) |
| 括号匹配 | `dp[i][j]` 最长合法子串 | `+ 2` (if 匹配) | O(n³) |

> [!tip]- **工程要点**：区间 DP 常为 O(n³)，能否通过取决于语言、常数、内存和题目时限；先估算 `n³` 的量级，再考虑优化。核心是理解“大区间由小区间推导”的依赖方向。

## 30 秒回答

**区间 DP 的遍历顺序为什么按长度递增？** `dp[i][j]` 往往要读取更短的 `dp[i][k]` 和 `dp[k+1][j]`，因此必须先算短区间。写题时先统一下标语义（闭区间还是开区间）、确定空区间/单点区间的初值，再枚举长度和分割点。

**自测：** 石子合并的区间和为什么与分割点无关？什么时候不能贸然使用 Knuth 优化？

---

## 关联笔记

- [1D DP： Climbing Stairs, House Robber (线性DP)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11a-1D%20DP：%20Climbing%20Stairs,%20House%20Robber%20(线性DP).md)
- [Knapsack： 0-1, Complete, Multiple (背包三种)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11b-Knapsack：%200-1,%20Complete,%20Multiple%20(背包三种).md)
- [LCS & LIS (最长子序列类)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11d-LCS%20&%20LIS%20(最长子序列类).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
