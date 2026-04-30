#data-structure #monotonic-queue #deque #sliding-window #optimization

## ⚡ TL;DR（快速决策）

- 单调队列本质是：**在队列中维护一个单调序列，从而在线性时间内维护区间最值**
- 一看到这些特征，要优先想到单调队列：
    - 滑动窗口最大值 / 最小值
    - 区间最值需要边走边维护
    - 朴素做法是每次重扫窗口，复杂度太高
- 它不是新容器，而是 **deque 的使用技巧**
- 单调队列最经典应用：`Sliding Window Maximum`

## 🧩 Core Idea（核心本质）

- 我们希望维护当前窗口里的“最有用候选人”
- 队头通常保存当前窗口最值的位置
- 新元素进入时：
    - 把队尾那些“不可能再成为答案”的元素弹掉
- 窗口左端移动时：
    - 把已经离开窗口的队头弹掉
- 一句话理解：
    - **让队列里只留下真正可能成为窗口答案的元素。**
- 为什么常存下标而不是值：
    - 需要判断元素是否已经滑出窗口
    - 下标还能反查值

## 🔧 Usage Patterns（可复用代码模板）

1. 滑动窗口最大值模板

```cpp
#include <deque>
#include <iostream>
#include <vector>
using namespace std;

vector<int> maxSlidingWindow(const vector<int>& nums, int k) {
    deque<int> dq;  // 存下标，且对应值单调递减
    vector<int> ans;

    for (int i = 0; i < (int)nums.size(); ++i) {
        while (!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        if (i >= k - 1) ans.push_back(nums[dq.front()]);
    }
    return ans;
}
```

1. 维护窗口最小值

```cpp
while (!dq.empty() && nums[dq.back()] >= nums[i]) dq.pop_back();
```

1. 固定窗口基本框架

```cpp
for (int i = 0; i < n; ++i) {
    // 1. 删除过期元素
    // 2. 维护单调性
    // 3. 加入当前元素
    // 4. 当窗口形成后读取答案
}
```

## ⚠️ Pitfalls（高频错误）

- 队列里应该存下标还是值没想清楚
- 窗口过期判断写错，常见是 `dq.front() <= i - k`
- 最大值和最小值时，比较号方向写反
- 相等元素到底保留谁，和 `< / <=` 的选择有关
- 忘记只有当 `i >= k - 1` 时窗口才真正形成

## 🚀 Performance / Tips（性能优化）

- 整体复杂度通常是 $O(n)$
- 每个元素最多进队一次、出队一次
- 从 $O(nk)$ 优化到 $O(n)$` 是它最核心的价值
- 高频经验：
    - 求最大值：队列通常维护递减
    - 求最小值：队列通常维护递增

## 🧪 Common Scenarios（常见使用场景）

- 滑动窗口最大值 / 最小值
- DP 优化中的窗口最值维护
- 某些区间转移优化
- 实时维护最近 k 个元素的极值

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <deque>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    deque<int> dq;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();
        dq.push_back(i);
        if (i >= k - 1) cout << a[dq.front()] << ' ';
    }
}
```

## 📌 One-liner Summary（一句话总结）

- **单调队列就是：通过维护队列单调性，在线性时间内求滑动窗口最值。**