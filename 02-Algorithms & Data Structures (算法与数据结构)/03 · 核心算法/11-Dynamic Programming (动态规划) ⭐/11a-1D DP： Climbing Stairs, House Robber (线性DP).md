---
tags:
  - algorithm
  - core-algorithm
---

> **核心考点**：一维 DP 状态定义、斐波那契类递推、打家劫舍系列、爬楼梯变体、滚动数组优化

## 一维 DP 核心思想

一维 DP 的状态 `dp[i]` 通常表示**以第 i 个位置结尾**（或前 i 个）的最优解/方案数。关键是找到**递推关系**——当前状态如何由前一个或前几个状态转移而来。

**一般步骤：**
1. 定义 `dp[i]` 的含义
2. 找到递推关系（状态转移方程）
3. 初始化边界条件
4. 确定遍历顺序

## 爬楼梯系列

### 基础爬楼梯

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

### 最小花费爬楼梯

```cpp
int minCostClimbingStairs(vector<int>& cost) {
    int n = cost.size();
    int a = cost[0], b = cost[1];
    for (int i = 2; i < n; i++) {
        int c = min(a, b) + cost[i];
        a = b;
        b = c;
    }
    return min(a, b);
}
```

## 打家劫舍系列

### 打家劫舍 I（线性排列）

```cpp
// 相邻房屋不能同时偷，求最大金额
int rob(vector<int>& nums) {
    int n = nums.size();
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

### 打家劫舍 II（环形排列）

```cpp
// 首尾相连，不能同时偷第一家 and 最后一家
int robII(vector<int>& nums) {
    int n = nums.size();
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

### 打家劫舍 III（树形）

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

## 其他经典一维 DP

### 最大子数组和（Kadane 算法）

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

### 乘积最大子数组

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

### 解码方法

```cpp
int numDecodings(string s) {
    int n = s.size();
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

## 一维 DP 经典题型速查

| 题型 | dp[i] 含义 | 递推 | 复杂度 |
|------|-----------|------|--------|
| 爬楼梯 | 到 i 阶的方案数 | `dp[i] = dp[i-1] + dp[i-2]` | O(n) |
| 打家劫舍 | 前 i 间最大金额 | `dp[i] = max(dp[i-1], dp[i-2]+nums[i])` | O(n) |
| 最大子数组和 | 以 i 结尾的最大和 | `dp[i] = max(nums[i], dp[i-1]+nums[i])` | O(n) |
| 解码方法 | 前 i 个字符的解码数 | `dp[i] = dp[i-1] + dp[i-2]`（有条件） | O(n) |
| 单词拆分 | 前 i 个字符是否可拆分 | `dp[i] = any(dp[j] && s[j:i] in dict)` | O(n²) |

> **工程要点**：一维 DP 的核心是状态定义。大多数线性 DP 只需记录前几个状态，可以用滚动变量替代数组，将空间从 O(n) 优化到 O(1)。遇到新题先想：`dp[i]` 表示什么？怎么从 `dp[i-1]` 或 `dp[i-2]` 转移过来？
