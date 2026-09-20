---
study_stage: backlog
tags: [algorithm/dynamic-programming, algorithm/state-design, learning/foundation]
---

> [!abstract] 学习目标
> 从状态、转移、初值和遍历顺序推导一维动态规划，理解滚动压缩的前提，并能区分计数、可行性与最优化问题。

# 动态规划解决什么问题

动态规划（dynamic programming, DP）适用于具有**重叠子问题**和**最优子结构**的问题。它不是“套递推式”，而是把原问题映射成有限状态图，按依赖顺序只计算一次每个状态。

写代码前依次回答：

1. `dp[i]` 精确定义什么，是否包含位置 `i`？
2. 最后一个决策有哪些互斥选择？
3. 当前状态依赖哪些更小状态？
4. 空输入与最小规模的初值是什么？
5. 遍历方向能否保证依赖已经计算？
6. 求的是最大/最小值、方案数，还是是否可行？

同一道题改变状态定义，转移和初值也会改变。不要只复制公式。

# 线性递推：爬楼梯

每次走 1 或 2 阶，令 `dp[i]` 为到达第 `i` 阶的方案数。最后一步来自 `i-1` 或 `i-2`：

```text
dp[i] = dp[i - 1] + dp[i - 2]
dp[0] = 1, dp[1] = 1
```

`dp[0] = 1` 表示“什么都不做”是一种到达起点的方式，它让转移统一。若题目把“0 阶有几种走法”定义不同，接口结果也需单独约定。

```cpp
long long climb_stairs(int n) {
    if (n < 0) return 0;
    long long prev2 = 1; // dp[0]
    long long prev1 = 1; // dp[1]
    for (int i = 2; i <= n; ++i) {
        long long current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }
    return n == 0 ? prev2 : prev1;
}
```

方案数增长很快，真实题目需检查整数溢出或按模计算。

# 选或不选：打家劫舍

令 `dp[i]` 表示前 `i` 间房能取得的最大金额。对第 `i-1` 间房：

```text
不选：dp[i - 1]
选择：dp[i - 2] + value[i - 1]
dp[i] = max(两者)
```

```cpp
long long rob_linear(const std::vector<int>& value) {
    long long two_back = 0;
    long long one_back = 0;
    for (int x : value) {
        long long current = std::max(one_back, two_back + x);
        two_back = one_back;
        one_back = current;
    }
    return one_back;
}
```

环形排列中首尾不能同时选择，可拆成“不含最后一间”和“不含第一间”两个线性问题；长度为 0 或 1 要单独处理。树形版本则不再是一维 DP，应以后序返回“选当前/不选当前”两个状态。

# 以当前位置结尾：Kadane

最大子数组要求连续。定义 `best_ending_here` 为**必须以当前元素结尾**的最大和：

```cpp
long long max_subarray(const std::vector<int>& a) {
    if (a.empty()) throw std::invalid_argument("empty input");
    long long ending = a.front();
    long long answer = ending;
    for (std::size_t i = 1; i < a.size(); ++i) {
        ending = std::max<long long>(a[i], ending + a[i]);
        answer = std::max(answer, ending);
    }
    return answer;
}
```

不能把初值设为 0，除非题目允许空子数组；否则全负数组会得到错误答案。若要恢复区间，记录“重新开始”的位置和更新全局答案时的左右端点。

# 多状态一维 DP

乘积最大子数组需要同时保存以当前位置结尾的最大值与最小值，因为负数会交换二者。股票、状态机等问题也常在每个位置维护多个互斥状态，例如“持有/不持有”。

核心原则：若未来决策需要区分两类历史，它们就不能被过早合并为一个数。

# 计数问题与解码

数字字符串解码中，`dp[i]` 表示前 `i` 个字符的方案数：

- 单字符在 `''1''..''9''` 时从 `dp[i-1]` 转移；
- 两字符组成 `10..26` 时从 `dp[i-2]` 转移；
- `''0''` 不能单独解码，只能作为 `10` 或 `20` 的一部分。

计数使用加法，最优化使用 `min/max`，可行性使用逻辑或；三种代数语义不能混用。

# 空间压缩的条件

只有当前状态依赖有限个、且覆盖后不再需要的旧状态时，才能滚动压缩。压缩前先写清二维/一维原始依赖，再决定变量更新顺序。空间从 `O(n)` 降到 `O(1)` 可能失去路径恢复信息，也未必比清晰数组版本更适合教学和调试。

# 检查理解

1. `dp[i]` 表示“前 i 个元素”与“以 i 结尾”会怎样改变下标？
2. Kadane 为什么必须单独保存全局答案？
3. 乘积最大子数组为何需要最大和最小两个状态？
4. 空间压缩时，如何判断某个旧状态还能否覆盖？

> [!summary] 本篇结论
> 一维 DP 的核心是让状态包含未来决策所需的最少历史。先定义语义和依赖，再确定初值与顺序；滚动变量只是已证明依赖后的空间优化。

下一步：[11-Knapsack Dynamic Programming (背包动态规划)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/11-Knapsack%20Dynamic%20Programming%20(背包动态规划).md)
