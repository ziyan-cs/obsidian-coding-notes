#algorithm #sliding-window #two-pointers #substring #array

## ⚡ TL;DR（快速决策）

- 滑动窗口本质是：**维护一个连续区间，并在移动过程中动态更新区间信息**
- 一看到这些特征，要优先想到滑动窗口：
    - 最长 / 最短子串
    - 连续子数组满足某种条件
    - 窗口左右端点会动态扩张和收缩
- 滑动窗口是双指针的重要模型
- 核心不是“窗口”两个字，而是：**区间连续且可以边移动边维护性质**

## 🧩 Core Idea（核心本质）

- 通常用两个指针 `l`、`r` 表示当前窗口 `[l, r]`
- `r` 右移：扩张窗口
- `l` 右移：收缩窗口
- 一句话理解：
    - **一边移动边维护一个合法的连续区间。**
- 做题关键：
    - 窗口内维护什么信息
    - 什么条件下扩张
    - 什么条件下收缩

## 🔧 Usage Patterns（可复用代码模板）

1. 最长无重复子串

```cpp
#include <iostream>
#include <unordered_map>
using namespace std;

int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> cnt;
    int ans = 0;
    for (int l = 0, r = 0; r < (int)s.size(); ++r) {
        ++cnt[s[r]];
        while (cnt[s[r]] > 1) {
            --cnt[s[l]];
            ++l;
        }
        ans = max(ans, r - l + 1);
    }
    return ans;
}
```

1. 和至少为 target 的最短子数组

```cpp
int l = 0, sum = 0, ans = 1e9;
for (int r = 0; r < n; ++r) {
    sum += a[r];
    while (sum >= target) {
        ans = min(ans, r - l + 1);
        sum -= a[l++];
    }
}
```

1. 固定长度窗口

```cpp
for (int i = 0; i < n; ++i) {
    // 加入 a[i]
    if (i >= k) {
        // 移除 a[i-k]
    }
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：窗口维护信息更新不完整
- 最常见错误 2：收缩条件和扩张条件写反
- 最常见错误 3：并非所有子数组题都能滑动窗口，通常需要某种单调性
- 最常见错误 4：左端点移动后忘记同步更新计数 / 和
- 最常见错误 5：固定窗口和可变窗口模板混用

## 🚀 Performance / Tips（性能优化）

- 许多滑动窗口题复杂度是 $O(n)$
- 每个元素通常最多进窗口一次、出窗口一次
- 高频经验：
    - 连续区间问题先判断能否滑窗
    - 定长窗口和变长窗口要区分
    - 窗口内信息常用：计数、和、频率表、哈希表

## 🧪 Common Scenarios（常见使用场景）

- 最长无重复子串
- 最短覆盖子串
- 固定长度窗口最值 / 平均值
- 连续子数组和约束
- 字符串频率匹配

## 🆚 Sliding Window vs Two Pointers / Monotonic Queue

- 滑动窗口是双指针中的连续区间模型
- 单调队列常是滑动窗口的进一步优化工具，用于维护窗口最值

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4, 5};
    int k = 3, sum = 0;
    for (int i = 0; i < (int)a.size(); ++i) {
        sum += a[i];
        if (i >= k) sum -= a[i - k];
        if (i >= k - 1) cout << sum << ' ';
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **滑动窗口就是：通过移动连续区间的左右边界，在移动过程中维护区间性质并高效求解。**