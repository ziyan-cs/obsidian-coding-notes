#algorithm #graph #topological-sort #dag #indegree

## ⚡ TL;DR（快速决策）

- 拓扑排序本质是：**给有向无环图（DAG）安排一个合法先后顺序**
- 一看到这些关键词，要优先想到拓扑排序：
    - 依赖关系
    - 先修课程
    - 任务调度
    - 必须先做 A 再做 B
    - 判断图中是否有环
- 只适用于 **DAG**
- 最经典做法：**Kahn 算法（入度 + 队列）**
- 如果最后排不完所有点，通常说明图里有环

## 🧩 Core Idea（核心本质）

- 如果一条边是 `u -> v`，表示 `u` 必须在 `v` 前面
- 拓扑排序要找的是一种满足所有边方向约束的线性顺序
- 核心思想：
    - 先找当前所有**入度为 0** 的点
    - 它们没有前置依赖，可以先放进答案
    - 删除这些点的影响后，继续找新的入度为 0 的点
- 一句话理解：
    - **谁没有前置任务，谁就先做。**

## 🔧 Usage Patterns（可复用代码模板）

1. Kahn 算法模板

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<int> topoSort(int n, const vector<vector<int>>& g) {
    vector<int> indeg(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        for (int v : g[u]) ++indeg[v];
    }

    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (indeg[i] == 0) q.push(i);
    }

    vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);

        for (int v : g[u]) {
            if (--indeg[v] == 0) q.push(v);
        }
    }

    return order;
}
```

1. 判断是否有环

```cpp
bool hasCycle(int n, const vector<vector<int>>& g) {
    vector<int> order = topoSort(n, g);
    return (int)order.size() != n;
}
```

1. 课程表类问题

```cpp
if ((int)topoSort(n, g).size() == n) {
    cout << "可以完成所有课程\\n";
} else {
    cout << "存在环，无法完成\\n";
}
```

## ⚠️ Pitfalls（高频错误）

- 拓扑排序只适用于**有向图**
- 不是所有有向图都能拓扑排序，必须无环
- 入度数组别忘记初始化和更新
- 判环常见错误：忘记比较 `order.size()` 和 `n`
- 多个入度为 0 的点时，答案可能不唯一

## 🚀 Performance / Tips（性能优化）

- 复杂度通常是 $O(n + m)$
- 邻接表存图最常见
- 需要字典序最小拓扑序时，可把普通队列改成小根堆

## 🧪 Common Scenarios（常见使用场景）

- 先修课程
- 任务依赖调度
- 软件编译顺序
- 判断图中是否存在环

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
    int n = 4;
    vector<vector<int>> g(n + 1);
    g[1] = {2, 3};
    g[2] = {4};
    g[3] = {4};

    vector<int> indeg(n + 1, 0);
    for (int u = 1; u <= n; ++u)
        for (int v : g[u]) ++indeg[v];

    queue<int> q;
    for (int i = 1; i <= n; ++i)
        if (indeg[i] == 0) q.push(i);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        cout << u << ' ';
        for (int v : g[u]) if (--indeg[v] == 0) q.push(v);
    }
}
```

## 📌 One-liner Summary（一句话总结）

- **拓扑排序就是：在 DAG 中按依赖关系排出一个合法先后顺序。**