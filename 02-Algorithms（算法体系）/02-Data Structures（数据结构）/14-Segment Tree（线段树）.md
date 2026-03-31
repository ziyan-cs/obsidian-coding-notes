#data-structure #segment-tree #range-query #range-update #tree

## ⚡ TL;DR（快速决策）

- 线段树本质是：**把区间递归拆分成若干小段，从而高效维护区间信息**
- 一看到这些需求，要优先想到线段树：
    - 区间查询
    - 区间修改
    - 动态维护区间最值、区间和
- 它比树状数组更强，但实现也更重
- 若题目同时涉及多次区间修改和查询，线段树非常高频

## 🧩 Core Idea（核心本质）

- 每个节点维护一个区间 `[l, r]` 的信息
- 根节点维护整个区间
- 子节点分别维护左右半区间
- 一句话理解：
    - **线段树就是把大区间分成小区间，用树来维护它们的统计信息。**
- 常见操作：
    - build
    - query
    - update
    - lazy propagation（懒标记）

## 🔧 Usage Patterns（可复用代码模板）

1. 单点修改 + 区间求和框架

```cpp
#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int l, r, sum;
};
vector<Node> tr;
vector<int> a;

void build(int u, int l, int r) {
    tr[u] = {l, r, 0};
    if (l == r) {
        tr[u].sum = a[l];
        return;
    }
    int mid = (l + r) >> 1;
    build(u << 1, l, mid);
    build(u << 1 | 1, mid + 1, r);
    tr[u].sum = tr[u << 1].sum + tr[u << 1 | 1].sum;
}
```

1. 区间查询思想

```cpp
// 若查询区间完全覆盖当前节点区间，直接返回
// 否则递归到左右孩子合并答案
```

1. 懒标记场景

```cpp
// 区间修改时，常配合 lazy propagation
```

## ⚠️ Pitfalls（高频错误）

- 递归边界写错
- `mid` 和左右区间划分写错
- pushup / pushdown 漏掉
- 懒标记最容易出 bug
- 下标体系不统一会非常难查错

## 🚀 Performance / Tips（性能优化）

- 单次区间查询 / 修改通常是 $O(\log n)$
- 代码量比树状数组大很多，但表达能力也更强
- 做题顺序建议：
    - 先掌握单点修改 + 区间查询
    - 再学区间修改 + 懒标记
- 复杂区间数据结构题里，线段树是核心武器之一

## 🧪 Common Scenarios（常见使用场景）

- 区间和
- 区间最大值 / 最小值
- 区间加法 / 覆盖修改
- 动态维护序列统计信息

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    cout << "Segment Tree 常用于区间查询与区间修改" << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **线段树就是：通过递归分治维护区间信息、支持高效区间查询与修改的结构。**