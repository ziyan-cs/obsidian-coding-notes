#data-structure #prefix-sum #range-sum #array #precompute

## ⚡ TL;DR（快速决策）

- 前缀和本质是：**先预处理累计和，再用减法快速回答区间和问题**
- 一看到这些需求，要优先想到前缀和：
    - 多次询问区间和
    - 区间统计
    - 子数组和、二维矩阵求和
- 核心公式：
    - 一维：`sum(l, r) = pre[r] - pre[l - 1]`
    - 二维：配合容斥
- 如果你会反复问“这一段的总和是多少”，优先先想前缀和

## 🧩 Core Idea（核心本质）

- `pre[i]` 表示前 `i` 个元素的和
- 那么任意连续区间和都可以由两个前缀相减得到
- 一句话理解：
    - **把“很多次求区间和”变成“一次预处理 + 常数次查询”。**
- 为什么强：
    - 朴素每次查询要扫一段
    - 前缀和查询只要 O(1)

## 🔧 Usage Patterns（可复用代码模板）

1. 一维前缀和

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {1, 2, 3, 4, 5};
    int n = a.size();
    vector<int> pre(n + 1, 0);

    for (int i = 1; i <= n; ++i) {
        pre[i] = pre[i - 1] + a[i - 1];
    }

    int l = 2, r = 4;  // 1-based
    cout << pre[r] - pre[l - 1] << '\\n';
}
```

1. 统计子数组和

```cpp
// 某些题会把前缀和与哈希表结合
```

1. 二维前缀和

```cpp
pre[i][j] = pre[i-1][j] + pre[i][j-1] - pre[i-1][j-1] + a[i][j];
```

## ⚠️ Pitfalls（高频错误）

- 下标定义混乱，尤其是 0-based 和 1-based 混用
- `pre[i]` 到底表示前 i 个还是到下标 i 为止，要先统一
- 二维前缀和容易漏掉容斥里的减号
- 数据范围大时前缀和可能爆 `int`，要考虑 `long long`

## 🚀 Performance / Tips（性能优化）

- 预处理：$O(n)$
- 单次一维区间查询：$O(1)$
- 二维前缀和对矩阵区间求和尤其高频
- 高频经验：
    - 一维多查询先想前缀和
    - 二维矩阵多查询先想二维前缀和
    - 若题目同时有修改和查询，前缀和未必足够，可能要树状数组 / 线段树

## 🧪 Common Scenarios（常见使用场景）

- 区间和查询
- 子数组和问题
- 二维矩阵求和
- 统计满足条件的连续区间
- 配合哈希优化子数组题

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {2, 4, 6, 8};
    vector<int> pre(a.size() + 1, 0);
    for (int i = 1; i <= (int)a.size(); ++i) pre[i] = pre[i - 1] + a[i - 1];
    cout << pre[3] - pre[1] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **前缀和就是：通过累计和预处理，把区间和查询优化成常数时间。**