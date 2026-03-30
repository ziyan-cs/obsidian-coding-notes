#algorithm #dp #knapsack #bag-dp #optimization

## ⚡ TL;DR（快速决策）

- 背包 DP 本质是：**在容量限制下做选择，使价值最大 / 方案可行 / 方案数满足要求**
- 一看到这些词，要优先想到背包 DP：
    - 选或不选
    - 容量 / 体积 / 花费限制
    - 最大价值 / 最少花费 / 方案数
- 最经典分类：
    - 0-1 背包：每个物品最多选一次
    - 完全背包：每个物品可选无限次
    - 多重背包：每个物品可选有限次
- 做题第一步：**先判断每个物品能选几次**

## 🧩 Core Idea（核心本质）

- 背包题通常有两维核心信息：
    - 枚举到第几个物品
    - 当前用了多少容量
- 常见状态：
    - `dp[j]`：容量为 `j` 时的最优值
- 核心区别在于循环顺序：
    - 0-1 背包：`j` 倒序
    - 完全背包：`j` 正序
- 一句话理解：
    - **背包 DP 的关键是“物品选几次”和“容量怎么枚举”。**

## 🔧 Usage Patterns（可复用代码模板）

1. 0-1 背包

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 3, W = 5;
    vector<int> w = {1, 2, 3};
    vector<int> v = {6, 10, 12};
    vector<int> dp(W + 1, 0);

    for (int i = 0; i < n; ++i) {
        for (int j = W; j >= w[i]; --j) {
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
        }
    }

    cout << dp[W] << '\\n';
}
```

1. 完全背包

```cpp
for (int i = 0; i < n; ++i) {
    for (int j = w[i]; j <= W; ++j) {
        dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    }
}
```

1. 恰好装满 / 可行性

```cpp
vector<bool> dp(W + 1, false);
dp[0] = true;
for (int x : nums) {
    for (int j = W; j >= x; --j) {
        dp[j] = dp[j] || dp[j - x];
    }
}
```

## ⚠️ Pitfalls（高频错误）

- 0-1 背包和完全背包循环顺序写反
- 不清楚 `dp[j]` 表示“恰好”还是“不超过”
- 初始化不对，尤其是方案数 / 不可达状态
- 看到“选或不选”就硬套 0-1 背包，但题目可能不是容量模型

## 🚀 Performance / Tips（性能优化）

- 0-1 / 完全背包常见复杂度是 $O(nW)$
- 先二维理解，再滚成一维最稳
- 若容量很大，要考虑是否有别的做法
- 若要求方案数、最小值、可行性，`max` 只是其中一种转移形式

## 🧪 Common Scenarios（常见使用场景）

- 选物品最大价值
- 硬币兑换
- 分割等和子集
- 最小花费达成目标
- 方案数统计

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int W = 5;
    vector<int> w = {1, 2, 3};
    vector<int> v = {6, 10, 12};
    vector<int> dp(W + 1, 0);

    for (int i = 0; i < (int)w.size(); ++i)
        for (int j = W; j >= w[i]; --j)
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);

    cout << dp[W] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **背包 DP 就是：在容量限制下决定每个物品选几次，从而优化目标值。**