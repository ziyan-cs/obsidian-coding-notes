#algorithm #dp #state-compression #bitmask #subset-dp

## ⚡ TL;DR（快速决策）

- 状压 DP 本质是：**用二进制位表示集合状态，再在这些状态上做 DP**
- 一看到这些特征，要优先想到状压 DP：
    - 元素个数不大（通常 `n <= 20` 左右）
    - 每个元素只有选 / 不选、在 / 不在这类状态
    - 要枚举子集、排列、访问集合
    - 旅行商、集合覆盖、配对问题
- 关键工具：`mask`
- 如果题目状态天然是一个小集合，优先先想状压

## 🧩 Core Idea（核心本质）

- 用一个整数的二进制表示集合
- 第 `i` 位是 1，表示第 `i` 个元素已被选中 / 已访问
- 常见状态：
    - `dp[mask]`
    - `dp[mask][i]`
- 一句话理解：
    - **把集合变成二进制，再在二进制状态上转移。**
- 状压 DP 的难点通常不是代码，而是：
    - 状态怎么定义
    - 从哪个旧集合转移过来

## 🔧 Usage Patterns（可复用代码模板）

1. 子集枚举基础

```cpp
#include <iostream>
using namespace std;

int main() {
    int n = 3;
    for (int mask = 0; mask < (1 << n); ++mask) {
        cout << mask << '\\n';
    }
}
```

1. TSP 风格状态定义

```cpp
// dp[mask][u] 表示访问过集合 mask，当前停在 u 的最小代价
```

1. 枚举子状态

```cpp
for (int sub = mask; sub; sub = (sub - 1) & mask) {
    // 枚举 mask 的非空子集
}
```

1. 基础状压 DP 框架

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 3;
    const int INF = 1e9;
    vector<vector<int>> dp(1 << n, vector<int>(n, INF));
    dp[1][0] = 0;

    for (int mask = 0; mask < (1 << n); ++mask) {
        for (int u = 0; u < n; ++u) {
            if (dp[mask][u] == INF) continue;
            for (int v = 0; v < n; ++v) {
                if ((mask >> v) & 1) continue;
                int nextMask = mask | (1 << v);
                dp[nextMask][v] = min(dp[nextMask][v], dp[mask][u] + 1);
            }
        }
    }
}
```

## ⚠️ Pitfalls（高频错误）

- `n` 稍大时，`2^n` 会直接爆炸
- 位运算优先级和括号容易写错
- 状态定义不清，转移就会乱
- 起始状态、终止状态常常写错
- 子集枚举别忘了空集是否需要单独处理

## 🚀 Performance / Tips（性能优化）

- 常见复杂度：
    - `O(n * 2^n)`
    - `O(n^2 * 2^n)`
- 经验判断：
    - `n <= 15` 很常见
    - `n <= 20` 可能还能做
- 写状压 DP 前先确认数据范围
- 若状态很多但转移稀疏，可考虑优化枚举方式

## 🧪 Common Scenarios（常见使用场景）

- TSP / Hamilton 路径
- 集合覆盖
- 配对问题
- 小规模选人 / 选任务问题
- 棋盘状态压缩

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int n = 3;
    for (int mask = 0; mask < (1 << n); ++mask) {
        for (int i = 0; i < n; ++i) {
            if ((mask >> i) & 1) cout << i << ' ';
        }
        cout << '\\n';
    }
}
```

## 📌 One-liner Summary（一句话总结）

- **状压 DP 就是：把集合状态压成二进制位，再在这些集合状态上做动态规划。**