#data-structure #fenwick-tree #bit #prefix-sum #logn

## ⚡ TL;DR（快速决策）

- 树状数组本质是：**用** $O(\log n)$ **维护前缀和的单点修改与查询**
- 一看到这些需求，要优先想到树状数组：
    - 单点修改 + 前缀和查询
    - 区间和查询（通过两个前缀和相减）
    - 动态维护频率、逆序对、排名统计
- 它比线段树更轻、更短，但功能相对更专一

## 🧩 Core Idea（核心本质）

- 树状数组用一个数组维护若干区间和
- 核心函数是 `lowbit(x) = x & -x`
- 一句话理解：
    - **树状数组就是“用二进制拆区间”的前缀和维护结构。**
- 为什么快：
    - 修改和查询都会沿着二进制跳转

## 🔧 Usage Patterns（可复用代码模板）

1. 基础模板

```cpp
#include <iostream>
#include <vector>
using namespace std;

struct BIT {
    int n;
    vector<int> tr;
    BIT(int n) : n(n), tr(n + 1, 0) {}

    int lowbit(int x) { return x & -x; }

    void add(int x, int v) {
        for (; x <= n; x += lowbit(x)) tr[x] += v;
    }

    int sum(int x) {
        int res = 0;
        for (; x > 0; x -= lowbit(x)) res += tr[x];
        return res;
    }
};
```

1. 区间和

```cpp
rangeSum(l, r) = sum(r) - sum(l - 1);
```

1. 统计逆序对思路

```cpp
// 常配合离散化 + 树状数组
```

## ⚠️ Pitfalls（高频错误）

- 树状数组通常使用 1-based 下标
- `lowbit` 写错会全盘崩
- 区间和查询别忘了用两个前缀和相减
- 它擅长的是某些固定操作，不是万能线段树替代品

## 🚀 Performance / Tips（性能优化）

- 单点修改：$O(log n)$
- 前缀和查询：$O(log n)$
- 常数小、代码短，是它最大优点
- 若需要更复杂区间操作，往往要看线段树

## 🧪 Common Scenarios（常见使用场景）

- 动态前缀和
- 逆序对统计
- 频率统计
- 排名 / 第 k 小相关辅助

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;
int lowbit(int x) { return x & -x; }
int main() {
    int n = 5;
    vector<int> tr(n + 1, 0);
    auto add = [&](int x, int v) {
        for (; x <= n; x += lowbit(x)) tr[x] += v;
    };
    auto sum = [&](int x) {
        int res = 0;
        for (; x > 0; x -= lowbit(x)) res += tr[x];
        return res;
    };
    add(1, 2); add(3, 5);
    cout << sum(3) << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **树状数组就是：一个利用二进制结构，在对数时间内维护前缀和的结构。**