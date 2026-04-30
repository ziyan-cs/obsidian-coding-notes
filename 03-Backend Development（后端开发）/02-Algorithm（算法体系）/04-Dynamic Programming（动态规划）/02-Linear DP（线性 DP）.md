#algorithm #dp #linear-dp #sequence #optimization

## ⚡ TL;DR（快速决策）

- 线性 DP 本质是：**状态排成一条线，当前位置答案由前面若干位置转移而来**
- 一看到这些特征，要优先想到线性 DP：
    - 数组 / 字符串从左到右处理
    - 当前答案依赖前一个或前几个状态
    - 求最优值、方案数、可行性
    - 经典题像爬楼梯、打家劫舍、最长上升子序列
- 线性 DP 的核心不是背公式，而是：
    - 状态怎么定义
    - 从哪里转移过来
- 如果问题天然沿着下标推进，优先先想线性 DP

## 🧩 Core Idea（核心本质）

- “线性”表示状态通常按 `i = 1..n` 顺着推进
- 常见状态定义：
    - `dp[i]`：前 `i` 个元素的最优答案
    - `dp[i]`：以 `i` 结尾的最优答案
- 最核心三问：
    1. `dp[i]` 表示什么？
    2. `dp[i]` 从哪些旧状态转移？
    3. 初始值是什么？
- 一句话理解：
    - **当前位置的答案，建立在前面已经算好的答案上。**

## 🔧 Usage Patterns（可复用代码模板）

1. 爬楼梯模板

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

1. 打家劫舍模板

```cpp
#include <iostream>
#include <vector>
using namespace std;

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];

    vector<int> dp(n, 0);
    dp[0] = nums[0];
    dp[1] = max(nums[0], nums[1]);

    for (int i = 2; i < n; ++i) {
        dp[i] = max(dp[i - 1], dp[i - 2] + nums[i]);
    }
    return dp[n - 1];
}
```

1. 最长上升子序列（朴素 DP）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int lengthOfLIS(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1);
    int ans = 1;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) {
                dp[i] = max(dp[i], dp[j] + 1);
            }
        }
        ans = max(ans, dp[i]);
    }
    return ans;
}
```

## ⚠️ Pitfalls（高频错误）

- `dp[i]` 定义不清，导致转移乱写
- 初始值没设好，后面全错
- “前 i 个”和“以 i 结尾”这两类状态别混
- 下标从 0 还是从 1 开始要统一
- 有些题可以滚动优化，但别一开始就为了省空间把逻辑写乱

## 🚀 Performance / Tips（性能优化）

- 常见复杂度：
    - 简单线性 DP 常是 $O(n)$
    - 双层转移常是 $O(n^2)$
- 看到只依赖前几个状态时，可考虑滚动数组 / 变量优化
- 做题顺序建议：
    - 先写清状态定义
    - 再写转移
    - 再补初始化

## 🧪 Common Scenarios（常见使用场景）

- 爬楼梯
- 打家劫舍
- 最大子数组和
- 最长上升子序列
- 字符串线性 DP

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4};
    int n = a.size();
    vector<int> dp(n, 0);
    dp[0] = a[0];
    for (int i = 1; i < n; ++i) dp[i] = max(dp[i - 1], a[i]);
    cout << dp[n - 1] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **线性 DP 就是：沿着序列顺序推进，让当前位置答案由前面的状态转移得到。**