#algorithm #graph #mst #kruskal #prim

## ⚡ TL;DR（快速决策）

- 最小生成树（MST）本质是：**在连通无向图中，用最小总代价把所有点连起来**
- 一看到这些特征，要优先想到 MST：
    - 要把所有点联通
    - 总边权最小
    - 不要求某两点最短，只要求整体代价最小
- 两大经典算法：
    - **Kruskal**：按边从小到大选，常配并查集
    - **Prim**：从点出发逐步扩展，像“生成树版 Dijkstra”
- 如果题目重点是“选哪些边”，经常先想 Kruskal

## 🧩 Core Idea（核心本质）

- 生成树要求：
    - 包含所有点
    - 连通
    - 边数正好是 `n - 1`
    - 不能有环
- 最小生成树就是在所有生成树中，边权和最小的那一棵
- 一句话理解：
    - **用最便宜的方式把所有点连起来。**
- 它和最短路的区别：
    - 最短路关注“某点到某点”最短
    - MST 关注“整体联通总代价最小”

## 🔧 Usage Patterns（可复用代码模板）

1. Kruskal 模板

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int u, v, w;
    bool operator<(const Edge& other) const {
        return w < other.w;
    }
};

vector<int> fa;
int find(int x) {
    return fa[x] == x ? x : fa[x] = find(fa[x]);
}

int main() {
    int n = 4;
    vector<Edge> edges = {
        {1, 2, 1}, {2, 3, 2}, {3, 4, 3}, {1, 4, 10}, {1, 3, 4}
    };

    fa.resize(n + 1);
    for (int i = 1; i <= n; ++i) fa[i] = i;

    sort(edges.begin(), edges.end());

    int ans = 0, cnt = 0;
    for (auto e : edges) {
        int fu = find(e.u), fv = find(e.v);
        if (fu == fv) continue;
        fa[fu] = fv;
        ans += e.w;
        ++cnt;
        if (cnt == n - 1) break;
    }

    cout << ans << '\\n';
}
```

1. 判是否成功生成树

```cpp
// 最终选边数若不是 n - 1，通常说明图不连通
```

## ⚠️ Pitfalls（高频错误）

- MST 通常讨论的是**无向图**
- 最短路和最小生成树别混
- Kruskal 里并查集别写错
- 选边数不是 `n - 1` 时，往往图不连通
- 图不连通时可能不存在生成树

## 🚀 Performance / Tips（性能优化）

- Kruskal 常见复杂度：排序主导，约 $O(m \log m)$
- 稀疏图里 Kruskal 很常用
- Prim 更适合某些稠密图 / 邻接矩阵场景
- 做题时若边已经给出得很自然，常优先想 Kruskal

## 🧪 Common Scenarios（常见使用场景）

- 建路最小花费
- 联网最小成本
- 城市连接问题
- 图整体联通代价最小

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int u, v, w;
    bool operator<(const Edge& o) const { return w < o.w; }
};

vector<int> fa;
int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }

int main() {
    int n = 3;
    vector<Edge> edges = {{1,2,1},{2,3,2},{1,3,5}};
    fa = vector<int>(n + 1);
    for (int i = 1; i <= n; ++i) fa[i] = i;

    sort(edges.begin(), edges.end());
    int ans = 0;
    for (auto e : edges) {
        int fu = find(e.u), fv = find(e.v);
        if (fu != fv) {
            fa[fu] = fv;
            ans += e.w;
        }
    }
    cout << ans << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **最小生成树就是：在无向图中用总代价最小的方式把所有点连起来。**