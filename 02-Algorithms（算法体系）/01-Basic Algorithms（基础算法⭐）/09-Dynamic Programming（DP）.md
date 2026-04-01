#algorithm #dp #dynamic-programming #state-transition #optimization

## ⚡ TL;DR（快速决策）

- 动态规划本质是：**把重复子问题的答案保存下来，通过状态转移逐步求解更大问题**
- 一看到这些特征，要优先想到 DP：
    - 最优子结构
    - 重复子问题
    - 求最大值 / 最小值 / 方案数 / 可行性
    - 暴力搜索太慢但子问题会反复出现
- DP 最关键的三件事：
    - 状态定义
    - 转移方程
    - 初始化

## 🧩 Core Idea（核心本质）

- DP 不是“背公式”，而是系统拆解问题
- 常见思路是：
    - 先定义 `dp[i]` / `dp[i][j]` 表示什么
    - 再想它由哪些旧状态转移而来
- 一句话理解：
    - **大问题的最优答案，建立在小问题答案已经算好的基础上。**
- 两种常见写法：
    - 记忆化搜索
    - 递推表格法

## 🔧 Usage Patterns（可复用代码模板）

1. 爬楼梯

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<int> dp(n + 1, 0);
    dp[0] = 1;
    dp[1] = 1;
    for (int i = 2; i <= n; ++i) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    cout << dp[n] << '\\n';
}
```

1. 0-1 背包骨架

```cpp
for (int i = 0; i < n; ++i) {
    for (int j = W; j >= w[i]; --j) {
        dp[j] = max(dp[j], dp[j - w[i]] + val[i]);
    }
}
```

1. 最长上升子序列朴素 DP

```cpp
for (int i = 0; i < n; ++i) {
    dp[i] = 1;
    for (int j = 0; j < i; ++j) {
        if (a[j] < a[i]) dp[i] = max(dp[i], dp[j] + 1);
    }
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：状态定义不清
- 最常见错误 2：转移写对了但初始化错了
- 最常见错误 3：枚举顺序不对，导致使用未计算状态
- 最常见错误 4：把贪心题硬写成 DP 或反过来
- 最常见错误 5：过早做空间优化把逻辑写乱

## 🚀 Performance / Tips（性能优化）

- DP 复杂度取决于状态数 × 转移代价
- 高频经验：
    - 先用自然语言写出 `dp` 含义
    - 再写转移
    - 最后补初始化和答案位置
- 若状态只依赖前几个位置，可考虑滚动优化
- 学习顺序建议：
    - 线性 DP
    - 背包 DP
    - 区间 / 树形 / 状压 / 数位 DP

## 🧪 Common Scenarios（常见使用场景）

- 爬楼梯
- 最长子序列问题
- 背包问题
- 路径最优值
- 方案数统计

## 🆚 DP vs Recursion / Greedy

- 递归：是求解方式
- DP：是利用重复子问题避免重复计算
- 贪心：只看局部最优
- DP：显式比较多种历史状态

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<int> dp(n + 1, 0);
    dp[0] = 0;
    for (int i = 1; i <= n; ++i) dp[i] = dp[i - 1] + i;
    cout << dp[n] << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **动态规划就是：通过保存子问题答案并进行状态转移，高效求解最优值、方案数或可行性问题。**