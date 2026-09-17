---
status: stable
confidence: high
verified: 2026-09-17
---

> [!summary]- 复述检查：学完后再展开
>
> **回答**：区间 DP 用 dp[l][r] 描述一个区间的最优值，按区间长度从短到长计算，并枚举断点或最后一次操作。关键是保证转移依赖的子区间已经求出，优化前先验证状态与决策是否满足所需结构。
>
> > [!note] 本节重点：区间 DP 的状态定义 `dp[i][j]` 表示区间 [i,j] 的最优解、长度递增枚举、石子合并/矩阵链乘、四边形不等式优化

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

> [!example]- 题型索引
> | 题型 | 状态定义 | 转移 | 复杂度 |
> |------|---------|------|--------|
> | 石子合并 | `dp[i][j]` 最小合并代价 | `+ sum(i,j)` | O(n³) |
> | 矩阵链乘 | `dp[i][j]` 最小乘法次数 | `+ d[i-1]*d[k]*d[j]` | O(n³) |
> | 回文分割 | `dp[i]` 前 i 个字符最小分割 | `= min(dp[i-1] + 1)` | O(n²) |
> | 戳气球 | `dp[i][j]` (i,j) 内最大收益 | `+ nums[i]*nums[k]*nums[j]` | O(n³) |
> | 括号匹配 | `dp[i][j]` 最长合法子串 | `+ 2` (if 匹配) | O(n³) |
>
> > [!tip]- **工程要点**：区间 DP 常为 O(n³)，能否通过取决于语言、常数、内存和题目时限；先估算 `n³` 的量级，再考虑优化。核心是理解“大区间由小区间推导”的依赖方向。
