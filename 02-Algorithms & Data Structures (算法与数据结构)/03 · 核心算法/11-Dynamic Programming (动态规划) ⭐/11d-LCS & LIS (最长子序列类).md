> **核心考点**：LCS 二维 DP 模板、LIS O(n log n) 贪心 + 二分、编辑距离、最长回文子序列

## 最长递增子序列（LIS）

### O(n²) DP 解法

```cpp
int lengthOfLIS(vector<int>& nums) {
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

### O(n log n) 贪心 + 二分（重点）

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

---

## 最长公共子序列（LCS）

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

## 编辑距离

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

## 最长回文子序列

```cpp
int longestPalindromeSubseq(string s) {
    int n = s.size();
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

## 题型速查

| 题型 | 解法 | 复杂度 |
|------|------|--------|
| LIS（最长递增子序列） | 贪心 + 二分（tails 数组） | O(n log n) |
| LCS（最长公共子序列） | 二维 DP，匹配+1，不匹配取 max | O(mn) |
| 编辑距离 | 二维 DP，增删改取 min+1 | O(mn) |
| 最长回文子序列 | 区间 DP 或 s + rev(s) 求 LCS | O(n²) |
| 最长公共子串 | dp[i][j] = dp[i-1][j-1]+1，不等则归零 | O(mn) |
| 最长重复子数组 | 同上，用一维滚动数组优化空间 | O(mn) |
| 最大子数组和（Kadane） | cur = max(x, cur+x) | O(n) |

> **工程要点**：子序列问题（不连续）通常用 DP；子数组/子串问题（连续）通常用滑动窗口或 Kadane。LIS 的 O(n log n) 解法是面试高频，核心是 tails 数组的**二分替换**思路，类比"耐心排序"。
