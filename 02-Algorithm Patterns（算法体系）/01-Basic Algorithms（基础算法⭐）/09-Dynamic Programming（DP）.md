#algorithm #dp #dynamic-programming #state-transition

## 核心

- DP = 记住重复子问题答案，避免重复计算
- 先想 4 件事：
    - 状态是什么
    - 状态转移怎么写
    - 初始化是什么
    - 遍历顺序怎么定

## 通用模板

```cpp
for (状态1) {
    for (状态2) {
        dp[...] = max/min(dp[...], dp[...]);
    }
}
```

## 高频例子

### 爬楼梯

```cpp
int climbStairs(int n) {
    vector<int> dp(n + 1);
    dp[0] = 1;
    dp[1] = 1;
    for (int i = 2; i <= n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    return dp[n];
}
```

### 0/1 背包

```cpp
int knapsack(vector<int>& w, vector<int>& v, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < (int)w.size(); ++i) {
        for (int j = cap; j >= w[i]; --j) {
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
        }
    }
    return dp[cap];
}
```

### 最长递增子序列（O(n^2)）

```cpp
int lengthOfLIS(vector<int>& nums) {
    int n = nums.size(), ans = 1;
    vector<int> dp(n, 1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) dp[i] = max(dp[i], dp[j] + 1);
        }
        ans = max(ans, dp[i]);
    }
    return ans;
}
```

## 高频坑点

- 状态定义含糊
- 初始化漏掉
- 维度压缩后更新顺序错
- 把 DFS + 记忆化和迭代 DP 关系没想清楚