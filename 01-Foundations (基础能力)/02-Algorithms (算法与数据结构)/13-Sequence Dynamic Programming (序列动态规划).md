---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [algorithm/dynamic-programming, algorithm/sequence, learning/foundation]
---

> [!abstract] 学习目标
> 区分子序列与连续子数组，掌握 LIS、LCS、编辑距离和回文子序列的状态设计，并理解长度、方案和路径恢复是不同输出。

# 先区分连续性

- **子数组/子串**必须连续，常使用滑动窗口、前缀和、Kadane 或“以当前位置结尾”的 DP。
- **子序列**只保持相对顺序，可以跳过元素，常用“前 i 个元素”或“以 i 结尾”的 DP。

仅凭题名里有“序列”无法选算法；还要确认是否连续、是否允许重排、是否求长度还是恢复具体序列。

# 最长递增子序列 LIS

## 二次 DP

令 `dp[i]` 为**以 `a[i]` 结尾**的 LIS 长度：

```text
dp[i] = 1 + max(dp[j])，其中 j < i 且 a[j] < a[i]
```

答案是所有 `dp[i]` 的最大值，不一定在最后位置。时间 `O(n²)`；记录使转移成立的前驱即可恢复一条 LIS。

## `O(n log n)` 的 tails

```cpp
int lis_length(const std::vector<int>& a) {
    std::vector<int> tails;
    for (int x : a) {
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return static_cast<int>(tails.size());
}
```

`tails[len-1]` 表示长度为 `len` 的递增子序列能取得的最小结尾。它通常**不是**一条真实 LIS；替换只是为未来保留更优扩展空间。严格递增用 `lower_bound`，非递减用 `upper_bound`。恢复序列需保存每个元素的前驱与它占据的层级。

# 最长公共子序列 LCS

令 `dp[i][j]` 为 `a` 前 `i` 个字符与 `b` 前 `j` 个字符的 LCS 长度：

```text
a[i-1] == b[j-1]：dp[i][j] = dp[i-1][j-1] + 1
否则：             dp[i][j] = max(dp[i-1][j], dp[i][j-1])
```

时间 `O(mn)`、完整表空间 `O(mn)`。只求长度可滚动到 `O(min(m,n))` 空间；要恢复一条 LCS，通常保留完整表或使用更复杂的 Hirschberg 分治。

LCS 不要求连续。最长公共子串在字符不等时必须归零，因为当前连续段被打断。

# 编辑距离

Levenshtein distance 允许插入、删除、替换。`dp[i][j]` 表示把源串前 `i` 个字符变为目标串前 `j` 个字符的最少操作数：

```text
dp[i][0] = i
dp[0][j] = j

字符相等：dp[i][j] = dp[i-1][j-1]
否则：1 + min(
    dp[i-1][j],    // 删除源字符
    dp[i][j-1],    // 插入目标字符
    dp[i-1][j-1]   // 替换
)
```

若操作成本不同、允许转置或按 Unicode 用户感知字符比较，状态与预处理必须调整。直接按 UTF-8 字节计算得到的是字节序列距离，不一定是人类字符距离。

# 最长回文子序列

定义 `dp[l][r]` 为闭区间的最长回文子序列长度：

```text
s[l] == s[r]：dp[l][r] = dp[l+1][r-1] + 2
否则：        dp[l][r] = max(dp[l+1][r], dp[l][r-1])
```

按区间长度递增或 `l` 倒序、`r` 正序计算。也可转化为 `s` 与 `reverse(s)` 的 LCS 求长度，但直接区间状态更贴合回文结构，路径恢复也更直观。

# 状态与输出的对应关系

| 目标 | 常用状态 | 额外信息 |
|---|---|---|
| LIS 长度 | 以 `i` 结尾或 tails | 无 |
| 恢复 LIS | 长度 + 前驱 | 前驱下标、末尾位置 |
| LCS 长度 | 前缀二维 DP | 可滚动 |
| 恢复 LCS | 完整表/决策 | 回溯方向 |
| 编辑脚本 | 编辑距离表 | 每格选择的操作 |

# 检查理解

1. tails 为什么不一定是一条真实 LIS？
2. 严格递增与非递减为什么分别使用 lower/upper bound？
3. LCS 与最长公共子串在“不匹配”时为何转移不同？
4. 空间压缩为什么会让路径恢复变难？

> [!summary] 本篇结论
> 序列 DP 的状态必须明确连续性、前缀范围和是否强制以当前位置结尾。长度可以压缩，恢复路径则需要保留决策；不要把输出要求当成无关细节。

下一步：[14-Greedy and Sliding Window (贪心与滑动窗口)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/14-Greedy%20and%20Sliding%20Window%20(贪心与滑动窗口).md)
