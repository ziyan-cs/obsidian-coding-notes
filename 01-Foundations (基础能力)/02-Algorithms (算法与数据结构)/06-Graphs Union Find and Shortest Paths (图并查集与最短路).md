---
status: stable
confidence: high
content_verified: 2026-09-19
tags: [algorithm/graph, algorithm/shortest-path, learning/foundation]
---

> [!abstract] 学习目标
> 从图表示与可达性出发，掌握 DFS/BFS、拓扑排序、并查集和最短路，并能根据边权、规模与输出需求选择算法。

# 图表示决定遍历成本

图由顶点 `V` 与边 `E` 构成，可有向/无向、有权/无权。邻接表空间通常为 `O(V + E)`，枚举一个顶点的出边高效；邻接矩阵空间 `O(V²)`，判断一条边是否存在为 `O(1)`，适合稠密图或矩阵算法。

无向图的每条边通常在邻接表存两次。边的方向、重复边、自环和编号范围必须在读入时明确。

# DFS、BFS 与拓扑排序

DFS 沿路径深入，适合连通分量、环检测、回溯和进入/退出时间；BFS 按层扩展，在无权图中求最少边数。二者使用邻接表时都是 `O(V + E)`。

有向图判环要区分“未访问、当前递归栈、已完成”三种状态；无向图 DFS 则需忽略返回父节点的那条边。只用一个 `visited` 布尔值无法覆盖两种语义。

## Kahn 拓扑排序

```cpp
std::vector<int> topological_sort(const std::vector<std::vector<int>>& graph) {
    std::vector<int> indegree(graph.size());
    for (const auto& edges : graph)
        for (int v : edges) ++indegree[v];

    std::queue<int> q;
    for (int u = 0; u < static_cast<int>(graph.size()); ++u)
        if (indegree[u] == 0) q.push(u);

    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : graph[u])
            if (--indegree[v] == 0) q.push(v);
    }
    if (order.size() != graph.size()) return {}; // 存在有向环
    return order;
}
```

拓扑序只对 DAG 存在，且通常不唯一。若需要字典序最小结果，可把普通队列换成最小堆，复杂度随之改变。

# 并查集：维护动态连通分量

并查集支持 `find` 查询代表元、`unite` 合并集合。路径压缩配合按大小/秩合并后，均摊复杂度为 `O(α(n))`，在实际规模下近似常数。

```cpp
class DisjointSet {
    std::vector<int> parent, size;
public:
    explicit DisjointSet(int n) : parent(n), size(n, 1) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        while (x != parent[x]) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (size[a] < size[b]) std::swap(a, b);
        parent[b] = a;
        size[a] += size[b];
        return true;
    }
};
```

它适合 Kruskal、无向图动态加边判环和连通分量合并；普通并查集不擅长删除边，也不能给出实际路径。

# 最短路径：先看边权

| 条件 | 算法 | 典型复杂度 |
|---|---|---:|
| 无权/等权 | BFS | `O(V + E)` |
| 权重仅 0/1 | 0-1 BFS | `O(V + E)` |
| 非负权 | Dijkstra + 堆 | `O((V+E) log V)` |
| 有负边、需检测负环 | Bellman–Ford | `O(VE)` |
| DAG | 拓扑序松弛 | `O(V + E)` |
| 全源、图较稠密/规模较小 | Floyd–Warshall | `O(V³)`、空间 `O(V²)` |

## Dijkstra 的不变量

从未确定顶点中取当前距离最小者；非负边保证它以后不会被更短路径改写。堆实现通常不做 decrease-key，而是把新距离再次入堆，弹出时跳过 `distance != dist[u]` 的过期条目。

```cpp
using Edge = std::pair<int, long long>; // to, weight
std::vector<long long> dijkstra(const std::vector<std::vector<Edge>>& g, int s) {
    constexpr long long INF = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> dist(g.size(), INF);
    using State = std::pair<long long, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    dist[s] = 0; pq.push({0, s});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        for (auto [v, w] : g[u]) {
            if (w < 0) throw std::invalid_argument("negative edge");
            if (dist[v] > d + w) {
                dist[v] = d + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
```

使用足够宽的距离类型，并让 `INF + weight` 不溢出。若需恢复路径，松弛时记录 `parent[v] = u`。

# 检查理解

1. 有向图与无向图判环为何不能共用同一个 visited 逻辑？
2. Kahn 输出节点数不足为何能证明存在环？
3. Dijkstra 为什么要求非负边？
4. 并查集为什么能判连通，却不能恢复两点间路径？

> [!summary] 本篇结论
> 图算法的第一步不是套模板，而是确定方向、边权、稀疏度和输出语义。遍历解决可达性，拓扑序解决依赖，并查集合并连通分量，最短路则由边权条件决定。

下一步：[07-Binary Search (二分查找)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/07-Binary%20Search%20(二分查找).md)
