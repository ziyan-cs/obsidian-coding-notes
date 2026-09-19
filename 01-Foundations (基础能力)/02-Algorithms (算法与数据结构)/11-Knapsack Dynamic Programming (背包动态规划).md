---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [algorithm/dynamic-programming, algorithm/knapsack, learning/foundation]
---

> [!abstract] 学习目标
> 用“物品使用次数、容量语义与更新顺序”统一 0-1、完全和多重背包，并理解方案数中的组合/排列差异。

# 从二维状态开始

有 `n` 件物品，重量 `weight[i]`、价值 `value[i]`，容量 `C`。0-1 背包中每件至多使用一次。定义：

```text
dp[i][c] = 只考虑前 i 件物品，容量不超过 c 的最大价值
```

对第 `i-1` 件物品：

```text
不选：dp[i - 1][c]
选：  dp[i - 1][c - weight[i - 1]] + value[i - 1]
```

二维版本直接体现“上一层物品”，最适合证明。压缩为一维后，遍历方向承担了保留上一层的职责。

# 0-1 背包：容量倒序

```cpp
long long zero_one_knapsack(
    const std::vector<int>& weight,
    const std::vector<int>& value,
    int capacity) {
    if (weight.size() != value.size() || capacity < 0)
        throw std::invalid_argument("invalid input");
    std::vector<long long> dp(capacity + 1, 0);

    for (std::size_t i = 0; i < weight.size(); ++i) {
        if (weight[i] <= 0) throw std::invalid_argument("non-positive weight");
        for (int c = capacity; c >= weight[i]; --c) {
            dp[c] = std::max(dp[c], dp[c - weight[i]] + value[i]);
        }
    }
    return dp[capacity];
}
```

容量倒序确保 `dp[c - w]` 仍来自上一层，因此当前物品不会在同一轮重复使用。这里状态语义是“容量不超过 c”，所以全 0 初始化允许不装满。

# 完全背包：容量正序

每种物品可重复使用时，容量正序使 `dp[c - w]` 可能已经包含当前物品：

```cpp
for (std::size_t i = 0; i < weight.size(); ++i)
    for (int c = weight[i]; c <= capacity; ++c)
        dp[c] = std::max(dp[c], dp[c - weight[i]] + value[i]);
```

若存在重量 0 且价值为正的可无限物品，最优值无界；输入约束必须排除此情况。

# 多重背包

每件物品最多 `count[i]` 次：

- 直接枚举使用数量，复杂度可能较高；
- 按 `1, 2, 4, ...` 二进制分组拆成若干 0-1 物品，将数量降到 `O(log count)` 组；
- 单调队列优化可进一步处理大规模统一余数类，但实现复杂，需先证明状态形式。

二进制分组的最后一组应是剩余数量，不要求恰好为 2 的幂。

# 初始化决定“恰好”还是“至多”

求恰好装满时，不可达状态不能初始化为 0：

```cpp
constexpr long long NEG = std::numeric_limits<long long>::lowest() / 4;
std::vector<long long> dp(capacity + 1, NEG);
dp[0] = 0;
```

转移前要检查前态可达，避免对哨兵加价值产生溢出或伪答案。求最少物品数则用正无穷哨兵；求可行性用 `bool`。

# 组合数与排列数

硬币方案数中，循环顺序改变计数语义：

```text
物品在外、容量在内：每组硬币组合只被生成一次 → 组合数
容量在外、物品在内：不同选择顺序分别生成       → 排列数
```

这不是口诀，而是状态路径计数：外层循环决定按什么顺序把转移边加入状态图。方案数可能溢出，题目若要求取模，必须在每次加法时处理。

# 常见变体映射

| 问题 | 状态/运算 |
|---|---|
| 分割等和子集 | 0-1 可行性，目标为总和一半 |
| 目标和 | 转化为满足和约束的子集计数 |
| 零钱兑换最少硬币 | 完全背包 + `min` |
| 零钱兑换组合数 | 完全背包 + 计数，物品在外 |
| 二维费用 | 容量扩成两个维度，均按使用次数选择方向 |
| 分组背包 | 每组至多选一件，组内从同一上一层转移 |

# 检查理解

1. 0-1 背包压缩后为什么必须倒序容量？
2. “容量不超过”与“恰好装满”的初始化有何不同？
3. 组合数和排列数为何由循环顺序区分？
4. 二进制分组怎样表示 `1..count` 的任意使用数量？

> [!summary] 本篇结论
> 背包 DP 的统一坐标是：物品能用几次、容量代表什么、状态如何合并。遍历方向不是孤立口诀，而是在一维数组上保留或复用当前物品层的机制。

下一步：[12-Interval Dynamic Programming (区间动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/12-Interval%20Dynamic%20Programming%20(区间动态规划).md)
