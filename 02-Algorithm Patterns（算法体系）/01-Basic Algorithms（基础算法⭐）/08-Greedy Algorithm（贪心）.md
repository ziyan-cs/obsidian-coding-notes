#algorithm #greedy #optimization #local-optimum #strategy

## ⚡ TL;DR（快速决策）

- 贪心本质是：**每一步都做当前看起来最优的选择，希望最终得到全局最优**
- 一看到这些特征，要优先想到贪心：
    - 当前最优决策明显
    - 题目要求最大化 / 最小化
    - 可以通过排序后顺序决策
    - 选择一旦做出，通常不回头
- 贪心最难的不是写代码，而是：**证明当前局部最优为什么是对的**

## 🧩 Core Idea（核心本质）

- 贪心不是“凭感觉选最大的”
- 它要求问题满足某种贪心性质
- 一句话理解：
    - **局部最优选择不会破坏最终全局最优。**
- 常见贪心步骤：
    1. 找决策依据
    2. 排序或选择顺序
    3. 逐步做局部最优决策
    4. 验证是否合法并更新答案

## 🔧 Usage Patterns（可复用代码模板）

1. 区间选点 / 活动选择

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<pair<int,int>> seg = {{1,3}, {2,5}, {4,6}, {6,7}};
    sort(seg.begin(), seg.end(), [](auto& a, auto& b) {
        return a.second < b.second;
    });

    int ans = 0, last = -1e9;
    for (auto [l, r] : seg) {
        if (l >= last) {
            ++ans;
            last = r;
        }
    }
    cout << ans << '\\n';
}
```

1. 最少硬币（特定币制下）

```cpp
for (int x : coins) {
    ans += amount / x;
    amount %= x;
}
```

1. 排序后贪心

```cpp
sort(a.begin(), a.end());
for (int x : a) {
    // 按某个规则贪心选择
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：没有证明就盲目贪心
- 最常见错误 2：把 DP / 回溯题误判成贪心
- 最常见错误 3：排序关键字选错
- 最常见错误 4：局部选择看似合理，但会破坏后续可行性

## 🚀 Performance / Tips（性能优化）

- 很多贪心题复杂度由排序主导，为 $O(n \log n)$
- 高频经验：
    - “排序 + 扫描” 是贪心最常见形态
    - 看到区间、资源分配、最少 / 最多问题时优先想贪心
    - 若贪心不成立，通常需要 DP 或搜索
- 贪心判断关键：
    - 当前最优是否有交换论证 / 不后悔性质

## 🧪 Common Scenarios（常见使用场景）

- 活动选择
- 区间覆盖
- 跳跃游戏部分题型
- 哈夫曼编码思想
- 最少资源、最多收益类问题

## 🆚 Greedy vs DP

- 贪心：每步只看当前最优
- DP：综合历史状态做全局最优
- 一句话区别：
    - **贪心靠性质，DP 靠状态。**

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
int main() {
    vector<int> a = {4, 2, 7, 1};
    sort(a.begin(), a.end());
    for (int x : a) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **贪心就是：每一步都做当前最优选择，并依赖问题性质保证最终最优。**