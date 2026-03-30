#algorithm #dp #interval-dp #range #merge

## ⚡ TL;DR（快速决策）

- 区间 DP 本质是：**按区间长度从小到大计算区间答案**
- 一看到这些特征，要优先想到区间 DP：
    - 问题定义在一个区间 `[l, r]`
    - 大区间答案依赖更小区间
    - 合并石子、括号匹配、回文处理、区间合并
- 核心套路：
    - 枚举区间长度
    - 枚举左端点
    - 枚举分割点
- 如果题目是“把一个区间拆成两半再合并答案”，高概率是区间 DP

## 🧩 Core Idea（核心本质）

- 常见状态：`dp[l][r]`
- 表示区间 `[l, r]` 的最优值 / 最小代价 / 可行性
- 转移通常长这样：
    - `dp[l][r] = min/max(dp[l][k] + dp[k+1][r] + cost)`
- 因为大区间依赖小区间，所以必须按长度递推
- 一句话理解：
    - **先算小区间，再拼大区间。**

## 🔧 Usage Patterns（可复用代码模板）

1. 区间 DP 基础框架

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<vector<int>> dp(n, vector<int>(n, 0));

    for (int len = 2; len <= n; ++len) {
        for (int l = 0; l + len - 1 < n; ++l) {
            int r = l + len - 1;
            dp[l][r] = 1e9;
            for (int k = l; k < r; ++k) {
                dp[l][r] = min(dp[l][r], dp[l][k] + dp[k + 1][r]);
            }
        }
    }
}
```

1. 合并石子思想

```cpp
// 额外加上区间和 cost(l, r)
```

1. 回文区间判断

```cpp
// 有些题 dp[l][r] 表示区间是否回文
```

## ⚠️ Pitfalls（高频错误）

- 枚举顺序写错，导致用到未计算状态
- `k` 的范围常写错，应仔细看是 `[l, r-1]`
- 区间和若每次现算，可能多出一层复杂度，常配前缀和
- 初始值如 `dp[i][i]` 要看题意设对

## 🚀 Performance / Tips（性能优化）

- 经典区间 DP 常是 $O(n^3)$
- 若代价涉及区间和，常配前缀和降常数
- 做题时先写出：
    - 状态定义
    - 枚举顺序
    - 分割点含义

## 🧪 Common Scenarios（常见使用场景）

- 合并石子
- 矩阵链乘
- 回文串处理
- 戳气球类问题
- 括号区间最优结构

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 4;
    vector<vector<int>> dp(n, vector<int>(n, 0));

    for (int len = 2; len <= n; ++len) {
        for (int l = 0; l + len - 1 < n; ++l) {
            int r = l + len - 1;
            dp[l][r] = 1e9;
            for (int k = l; k < r; ++k) {
                dp[l][r] = min(dp[l][r], dp[l][k] + dp[k + 1][r] + 1);
            }
        }
    }

    cout << dp[0][n - 1] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **区间 DP 就是：定义区间状态，先算短区间，再通过分割点转移到长区间。**