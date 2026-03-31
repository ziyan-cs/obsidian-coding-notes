#algorithm #dp #tree-dp #dfs #subtree

## ⚡ TL;DR（快速决策）

- 树形 DP 本质是：**在树上定义状态，并通过子树信息向上合并答案**
- 一看到这些特征，要优先想到树形 DP：
    - 问题发生在树上
    - 某节点答案依赖子节点答案
    - 求子树最优、整棵树最优、选点方案
    - 涉及“选当前点 / 不选当前点”
- 最经典套路：**DFS + DP**
- 如果问题是“每个节点的答案由孩子决定”，高概率是树形 DP

## 🧩 Core Idea（核心本质）

- 树没有环（或根树视角下可用 parent 去掉回头路）
- 所以可以从叶子往上做状态转移
- 常见状态：
    - `dp[u]`：节点 `u` 为根的子树答案
    - `dp[u][0/1]`：表示选不选 `u`
- 一句话理解：
    - **先算孩子，再合并到父亲。**

## 🔧 Usage Patterns（可复用代码模板）

1. 树形 DP 基础框架

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<int> dp;

void dfs(int u, int parent) {
    dp[u] = 1;
    for (int v : g[u]) {
        if (v == parent) continue;
        dfs(v, u);
        dp[u] += dp[v];
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});
    dp.assign(n + 1, 0);

    g[1] = {2, 3};
    g[2] = {1, 4, 5};
    g[3] = {1};
    g[4] = {2};
    g[5] = {2};

    dfs(1, 0);
    cout << dp[1] << '\\n';
}
```

1. 树上选点（选 / 不选）

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<vector<int>> dp;
vector<int> w;

void dfs(int u, int parent) {
    dp[u][1] = w[u];
    for (int v : g[u]) {
        if (v == parent) continue;
        dfs(v, u);
        dp[u][0] += max(dp[v][0], dp[v][1]);
        dp[u][1] += dp[v][0];
    }
}
```

1. 子树大小

```cpp
subtreeSize[u] = 1 + sum(subtreeSize[v]);
```

## ⚠️ Pitfalls（高频错误）

- 忘记传 `parent`，在无向树上会来回递归
- 状态定义不清，导致父子转移混乱
- 合并子树时漏掉某个分支
- 根节点怎么选、答案从哪里取，要看题意

## 🚀 Performance / Tips（性能优化）

- 树形 DP 通常复杂度是 $O(n)$ 或 $O(n \times 状态数)$
- DFS 是最常见入口
- 做题顺序建议：
    - 先画出一个节点和孩子的关系
    - 再写 `u` 如何由孩子转移
- 很多树题本质都是“DFS 遍历 + 状态合并”

## 🧪 Common Scenarios（常见使用场景）

- 树上独立集
- 子树大小 / 子树和
- 树上选点最优值
- 树的重心 / 距离类 DP

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<int> sz;

void dfs(int u, int p) {
    sz[u] = 1;
    for (int v : g[u]) {
        if (v == p) continue;
        dfs(v, u);
        sz[u] += sz[v];
    }
}

int main() {
    int n = 3;
    g.assign(n + 1, {});
    sz.assign(n + 1, 0);
    g[1] = {2, 3};
    g[2] = {1};
    g[3] = {1};
    dfs(1, 0);
    cout << sz[1] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **树形 DP 就是：通过 DFS 先算子树，再把子节点状态合并到父节点。**