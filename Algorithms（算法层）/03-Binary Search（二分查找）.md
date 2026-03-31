#algorithm #binary-search #monotonicity #search #cpp

## ⚡ TL;DR（快速决策）

- 二分查找本质是：**利用单调性，每次把答案范围缩小一半**
- 一看到这些特征，要优先想到二分：
    - 有序数组查找
    - 找第一个满足条件的位置
    - 找最小可行值 / 最大可行值
    - 答案空间具有单调性
- 二分不是只能查数组，也可以二分答案
- 题目里出现“最小的满足条件”“最大的合法值”，要高度敏感

## 🧩 Core Idea（核心本质）

- 二分的核心不是“中点”本身，而是：
    - 左边一段一定不行 / 一定行
    - 右边一段一定行 / 一定不行
- 一句话理解：
    - **只要问题具有单调分界线，就可以尝试二分。**
- 二分常见对象：
    - 下标
    - 值域
    - 时间、长度、答案空间

## 🔧 Usage Patterns（可复用代码模板）

1. 有序数组查找目标

```cpp
int binarySearch(const vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (a[mid] == target) return mid;
        if (a[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}
```

1. 找第一个 >= target 的位置

```cpp
int l = 0, r = n - 1, ans = n;
while (l <= r) {
    int mid = l + (r - l) / 2;
    if (a[mid] >= target) {
        ans = mid;
        r = mid - 1;
    } else {
        l = mid + 1;
    }
}
```

1. 二分答案

```cpp
while (l <= r) {
    int mid = l + (r - l) / 2;
    if (check(mid)) {
        ans = mid;
        r = mid - 1;
    } else {
        l = mid + 1;
    }
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：循环不变量没想清楚
- 最常见错误 2：`l <= r` 和 `l < r` 写法混乱
- 最常见错误 3：更新边界时漏掉 `+1 / -1`
- 最常见错误 4：中点写成 `(l + r) / 2` 导致潜在溢出
- 最常见错误 5：题目根本没有单调性却强行二分

## 🚀 Performance / Tips（性能优化）

- 经典复杂度：$O(log n)$
- 二分答案通常是 $O(\log V \cdot check)$
- 高频经验：
    - 先写 `check(mid)` 的含义
    - 再决定二分的是“最小可行”还是“最大可行”
    - 写完后一定手推边界样例

## 🧪 Common Scenarios（常见使用场景）

- 有序数组找目标
- 找边界位置
- 最小可行值 / 最大可行值
- 长度、容量、时间的答案搜索
- 旋转数组、峰值等变形题

## 🆚 Binary Search vs Two Pointers

- 二分靠“单调分界线”
- 双指针靠“移动过程中可维护条件”
- 两者都利用结构性质，但思路不同

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 3, 5, 7, 9};
    int target = 7;
    int l = 0, r = (int)a.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (a[mid] == target) {
            cout << mid << '\\n';
            return 0;
        }
        if (a[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    cout << -1 << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **二分查找就是：利用单调性，每次排除一半范围来逼近目标答案。**