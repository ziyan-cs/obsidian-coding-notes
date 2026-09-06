---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# One Dimensional Dynamic Programming (一维动态规划)

> [!note] 本节重点：核心考点：一维 DP 状态定义、斐波那契类递推、打家劫舍系列、爬楼梯变体、滚动数组优化

# 一维 DP 核心思想

一维 DP 的状态 `dp[i]` 通常表示**以第 i 个位置结尾**（或前 i 个）的最优解/方案数。关键是找到**递推关系**——当前状态如何由前一个或前几个状态转移而来。

**一般步骤：**
1. 定义 `dp[i]` 的含义
2. 找到递推关系（状态转移方程）
3. 初始化边界条件
4. 确定遍历顺序

# 爬楼梯系列

## 基础爬楼梯

```cpp
// 爬楼梯：每次 1 或 2 阶，求到达顶部的方案数
int climbStairs(int n) {
    if (n <= 2) return n;
    int a = 1, b = 2;           // dp[1], dp[2]
    for (int i = 3; i <= n; i++) {
        int c = a + b;          // dp[i] = dp[i-1] + dp[i-2]
        a = b;
        b = c;
    }
    return b;
}
// 本质就是斐波那契数列，O(n) 时间，O(1) 空间
```

**变体：**
- 每次可走 1/2/3 阶：`dp[i] = dp[i-1] + dp[i-2] + dp[i-3]`
- 最小花费爬楼梯：`dp[i] = min(dp[i-1], dp[i-2]) + cost[i]`
- 每次可走 1..k 阶：`dp[i] = sum(dp[i-j] for j in 1..k)`

## 最小花费爬楼梯

```cpp
int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    if (n <= 1) return 0;
    int a = cost[0], b = cost[1];
    for (int i = 2; i < n; i++) {
        int c = min(a, b) + cost[i];
        a = b;
        b = c;
    }
    return min(a, b);
}
```

# 打家劫舍系列

## 打家劫舍 I（线性排列）

```cpp
// 相邻房屋不能同时偷，求最大金额
int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    int a = nums[0], b = max(nums[0], nums[1]);
    for (int i = 2; i < n; i++) {
        int c = max(b, a + nums[i]);    // 不偷 i vs 偷 i
        a = b;
        b = c;
    }
    return b;
}
// dp[i] = max(dp[i-1], dp[i-2] + nums[i])
```

## 打家劫舍 II（环形排列）

```cpp
// 首尾相连，不能同时偷第一家 and 最后一家
int robII(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    // 分别计算：去掉第一家 / 去掉最后一家，取最大值
    return max(robRange(nums, 0, n - 2), robRange(nums, 1, n - 1));
}

int robRange(vector<int>& nums, int l, int r) {
    int a = 0, b = 0;
    for (int i = l; i <= r; i++) {
        int c = max(b, a + nums[i]);
        a = b;
        b = c;
    }
    return b;
}
```

## 打家劫舍 III（树形）

```cpp
// 二叉树，不能偷直接相连的两个节点
pair<int,int> robTree(TreeNode* node) {
    if (!node) return {0, 0};
    auto l = robTree(node->left);
    auto r = robTree(node->right);
    int rob = node->val + l.first + r.first;      // 偷当前节点
    int skip = max(l.first, l.second) + max(r.first, r.second);  // 不偷
    return {skip, rob};  // first=不偷, second=偷
}
int robIII(TreeNode* root) {
    auto res = robTree(root);
    return max(res.first, res.second);
}
```

# 其他经典一维 DP

## 最大子数组和（Kadane 算法）

```cpp
int maxSubArray(vector<int>& nums) {
    int cur = 0, res = INT_MIN;
    for (int x : nums) {
        cur = max(x, cur + x);    // 要么从当前元素重新开始，要么延续之前的
        res = max(res, cur);
    }
    return res;
}
```

## 乘积最大子数组

```cpp
int maxProduct(vector<int>& nums) {
    int res = nums[0], curMax = nums[0], curMin = nums[0];
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] < 0) swap(curMax, curMin);  // 负号翻转最大最小
        curMax = max(nums[i], curMax * nums[i]);
        curMin = min(nums[i], curMin * nums[i]);
        res = max(res, curMax);
    }
    return res;
}
```

## 解码方法

```cpp
int numDecodings(string s) {
    int n = s.size();
    if (n == 0) return 0;
    int a = 1, b = s[0] != '0' ? 1 : 0;
    for (int i = 2; i <= n; i++) {
        int c = 0;
        if (s[i-1] != '0') c += b;                      // 单独解码
        int two = stoi(s.substr(i-2, 2));
        if (two >= 10 && two <= 26) c += a;             // 两位一起解码
        a = b; b = c;
    }
    return b;
}
```

# 一维 DP 经典题型速查

| 题型 | dp[i] 含义 | 递推 | 复杂度 |
|------|-----------|------|--------|
| 爬楼梯 | 到 i 阶的方案数 | `dp[i] = dp[i-1] + dp[i-2]` | O(n) |
| 打家劫舍 | 前 i 间最大金额 | `dp[i] = max(dp[i-1], dp[i-2]+nums[i])` | O(n) |
| 最大子数组和 | 以 i 结尾的最大和 | `dp[i] = max(nums[i], dp[i-1]+nums[i])` | O(n) |
| 解码方法 | 前 i 个字符的解码数 | `dp[i] = dp[i-1] + dp[i-2]`（有条件） | O(n) |
| 单词拆分 | 前 i 个字符是否可拆分 | `dp[i] = any(dp[j] && s[j:i] in dict)` | O(n²) |

> [!tip]- **工程要点**：一维 DP 的核心是状态定义。大多数线性 DP 只需记录前几个状态，可以用滚动变量替代数组，将空间从 O(n) 优化到 O(1)。遇到新题先想：`dp[i]` 表示什么？怎么从 `dp[i-1]` 或 `dp[i-2]` 转移过来？

# 30 秒回答

**一维 DP 怎么写稳？** 先用一句话定义状态，再列出“选/不选”或“从哪些前态来”的转移，最后单独处理空输入、长度为 1 和第一个可转移位置。滚动变量只是在已经证明状态依赖范围很小时的空间优化，不应反过来替代状态定义。

**自测：** 为什么 `rob` 的递推要比较“偷当前”与“不偷当前”？`numDecodings` 里字符 `'0'` 为什么不能单独贡献方案数？

---

# 关联笔记

- [Knapsack： 0-1, Complete, Multiple (背包三种)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11b-Knapsack：%200-1,%20Complete,%20Multiple%20(背包三种).md)
- [Interval DP (区间DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11c-Interval%20DP%20(区间DP).md)
- [LCS & LIS (最长子序列类)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11d-LCS%20&%20LIS%20(最长子序列类).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

# Knapsack Dynamic Programming (背包动态规划)

> [!note] 本节重点：核心考点：0-1 背包逆序遍历容量、完全背包正序遍历、多重背包二进制优化、恰好装满初始化

# 背包问题核心框架

背包问题的本质：给定一组物品（每个有重量和价值），在容量限制下最大化总价值。

**核心三要素：**
1. 物品数量（n）
2. 背包容量（cap）
3. 每种物品可选几次（0-1次 / 无限次 / 有限次）

---

# 1 背包（每种物品最多选一次）

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

# 完全背包（每种物品无限次）

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

**一维写法中的关键区别：** 0-1 背包容量逆序，使当前物品只能使用一次；完全背包容量正序，使当前物品可以复用。若题目改成“方案数”，还要同时明确物品/容量的外层循环，因为它决定统计组合还是排列。

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

# 多重背包（每种物品有限次）

## 解法一：暴力拆成 0-1（容易超时）

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

## 解法二：二进制优化（推荐）

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

# 背包问题对比

| 类型 | 容量遍历 | 物品遍历 | 时间复杂度 | 空间优化后 |
|------|---------|---------|-----------|-----------|
| 0-1 背包 | 逆序 | 外循环物品 | O(n·cap) | O(cap) |
| 完全背包 | 正序 | 外循环物品 | O(n·cap) | O(cap) |
| 多重背包（暴力） | 逆序 | 三层循环 | O(cap·Σcnt) | O(cap) |
| 多重背包（二进制） | 逆序 | 拆后 0-1 | O(cap·Σlog(cnt)) | O(cap) |

# 背包问题常见变体

| 变体 | 解法要点 |
|------|---------|
| 求方案数 | `dp[j] += dp[j - w[i]]` |
| 求能否装满 | `dp[j] = dp[j] \|\| dp[j - w[i]]` |
| 恰好装满 | dp 初始化为 -INF，`dp[0] = 0` |
| 二维费用 | `dp[j][k]` 两个维度都遍历容量 |
| 分组背包 | 每组选一个，组内遍历时容量在外、物品在内 |
| 依赖背包 | 转化为树形 DP + 分组背包 |

> [!tip]- **工程要点**：背包问题的核心就两点——**物品在外还是容量在外**决定是组合还是排列，**容量正序还是逆序**决定是完全还是 0-1。面试时先写出暴力二维版本再优化为一维，不易出错。

# 关联笔记 · 延伸要点 2
- [1D DP： Climbing Stairs, House Robber (线性DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11a-1D%20DP：%20Climbing%20Stairs,%20House%20Robber%20(线性DP).md)
- [Interval DP (区间DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11c-Interval%20DP%20(区间DP).md)
- [LCS & LIS (最长子序列类)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11d-LCS%20&%20LIS%20(最长子序列类).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

# Interval Dynamic Programming (区间动态规划)

> [!note] 本节重点：核心考点：区间 DP 的状态定义 `dp[i][j]` 表示区间 [i,j] 的最优解、长度递增枚举、石子合并/矩阵链乘、四边形不等式优化

# 区间 DP 核心思想

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

# 石子合并

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

# 矩阵链乘

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

# 回文分割

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

# 四边形不等式优化

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

# 经典题型速查

| 题型 | 状态定义 | 转移 | 复杂度 |
|------|---------|------|--------|
| 石子合并 | `dp[i][j]` 最小合并代价 | `+ sum(i,j)` | O(n³) |
| 矩阵链乘 | `dp[i][j]` 最小乘法次数 | `+ d[i-1]*d[k]*d[j]` | O(n³) |
| 回文分割 | `dp[i]` 前 i 个字符最小分割 | `= min(dp[i-1] + 1)` | O(n²) |
| 戳气球 | `dp[i][j]` (i,j) 内最大收益 | `+ nums[i]*nums[k]*nums[j]` | O(n³) |
| 括号匹配 | `dp[i][j]` 最长合法子串 | `+ 2` (if 匹配) | O(n³) |

> [!tip]- **工程要点**：区间 DP 常为 O(n³)，能否通过取决于语言、常数、内存和题目时限；先估算 `n³` 的量级，再考虑优化。核心是理解“大区间由小区间推导”的依赖方向。

# 关联笔记 · 延伸要点 3
- [1D DP： Climbing Stairs, House Robber (线性DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11a-1D%20DP：%20Climbing%20Stairs,%20House%20Robber%20(线性DP).md)
- [Knapsack： 0-1, Complete, Multiple (背包三种)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11b-Knapsack：%200-1,%20Complete,%20Multiple%20(背包三种).md)
- [LCS & LIS (最长子序列类)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11d-LCS%20&%20LIS%20(最长子序列类).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

# Longest Common Subsequence and LIS (最长子序列)

> [!note] 本节重点：核心考点：LCS 二维 DP 模板、LIS O(n log n) 贪心 + 二分、编辑距离、最长回文子序列

# 最长递增子序列（LIS）

## O(n²) DP 解法

```cpp
int lengthOfLIS(vector<int>& nums) {
    if (nums.empty()) return 0;
    int n = nums.size(), res = 1;
    vector<int> dp(n, 1);           // dp[i] = 以 i 结尾的最长递增子序列长度
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i])
                dp[i] = max(dp[i], dp[j] + 1);
        }
        res = max(res, dp[i]);
    }
    return res;
}
```

## O(n log n) 贪心 + 二分（重点）

维护 `tails[k]` = 长度为 k+1 的递增子序列的最小末尾元素：

```cpp
int lengthOfLIS(vector<int>& nums) {
    vector<int> tails;
    for (int x : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;                              // 更新最小末尾
    }
    return tails.size();
}
```

**理解：** `tails` 不一定是正确的 LIS 序列，但**长度**一定正确。算法核心是让 tails 中的数尽可能小，以便后续能接更多数。

> [!tip] 此处用 `lower_bound`，得到的是严格递增 LIS；若题目要求“非递减”子序列，通常改用 `upper_bound`。若要恢复具体序列，还需要额外记录前驱和每个长度对应的末尾下标。

---

# 最长公共子序列（LCS）

```cpp
int longestCommonSubsequence(string a, string b) {
    int m = a.size(), n = b.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    }
    return dp[m][n];
}
```

**短字符串优化：** 用 `min(m, n)` 决定方向，空间可优化为 O(min(m, n))。

**最长公共子串（连续）的特例：**

```cpp
int longestCommonSubstring(string a, string b) {
    int m = a.size(), n = b.size(), res = 0;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i-1] == b[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
                res = max(res, dp[i][j]);
            }  // 不相等时 dp[i][j] = 0（与 LCS 的区别）
        }
    }
    return res;
}
```

---

# 编辑距离

```cpp
int minDistance(string a, string b) {
    int m = a.size(), n = b.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 0; i <= m; i++) dp[i][0] = i;     // 删除
    for (int j = 0; j <= n; j++) dp[0][j] = j;     // 插入

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1];
            else
                dp[i][j] = 1 + min({dp[i-1][j],    // 删除
                                    dp[i][j-1],    // 插入
                                    dp[i-1][j-1]}); // 替换
        }
    }
    return dp[m][n];
}
```

---

# 最长回文子序列

```cpp
int longestPalindromeSubseq(string s) {
    int n = s.size();
    if (n == 0) return 0;
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int i = n - 1; i >= 0; i--) {
        dp[i][i] = 1;
        for (int j = i + 1; j < n; j++) {
            if (s[i] == s[j])
                dp[i][j] = dp[i+1][j-1] + 2;
            else
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
        }
    }
    return dp[0][n-1];
}
// 也可用 s 和 reverse(s) 求 LCS
```

---

# 题型速查

| 题型 | 解法 | 复杂度 |
|------|------|--------|
| LIS（最长递增子序列） | 贪心 + 二分（tails 数组） | O(n log n) |
| LCS（最长公共子序列） | 二维 DP，匹配+1，不匹配取 max | O(mn) |
| 编辑距离 | 二维 DP，增删改取 min+1 | O(mn) |
| 最长回文子序列 | 区间 DP 或 s + rev(s) 求 LCS | O(n²) |
| 最长公共子串 | dp[i][j] = dp[i-1][j-1]+1，不等则归零 | O(mn) |
| 最长重复子数组 | 同上，用一维滚动数组优化空间 | O(mn) |
| 最大子数组和（Kadane） | cur = max(x, cur+x) | O(n) |

> [!tip]- **工程要点**：子序列问题（不连续）通常用 DP；子数组/子串问题（连续）通常用滑动窗口或 Kadane。LIS 的 O(n log n) 解法是面试高频，核心是 tails 数组的**二分替换**思路，类比"耐心排序"。

# 关联笔记 · 延伸要点 4
- [1D DP： Climbing Stairs, House Robber (线性DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11a-1D%20DP：%20Climbing%20Stairs,%20House%20Robber%20(线性DP).md)
- [Knapsack： 0-1, Complete, Multiple (背包三种)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11b-Knapsack：%200-1,%20Complete,%20Multiple%20(背包三种).md)
- [Interval DP (区间DP)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/11-Dynamic%20Programming%20(动态规划)%20⭐/11c-Interval%20DP%20(区间DP).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)



# 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

# 常见误区

- 把 **09-Dynamic Programming (动态规划)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **09-Dynamic Programming (动态规划)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
