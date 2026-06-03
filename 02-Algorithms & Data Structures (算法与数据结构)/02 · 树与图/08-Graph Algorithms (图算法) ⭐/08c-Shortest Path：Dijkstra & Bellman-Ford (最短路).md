---
tags:
  - algorithm
  - tree-graph
---

> **核心考点**：Dijkstra（堆优化）、Bellman-Ford（负权检测）、Floyd 多源最短路
> **核心考点**：Dijkstra 非负权最短路（堆优化）、Bellman-Ford 负权最短路与检测、Floyd 多源最短路

### Dijkstra（单源最短路，非负权边）

```cpp
vector<int> dijkstra(vector<vector<pair<int,int>>>& graph, int start) {
    int n = graph.size();
    vector<int> dist(n, INT_MAX);
    dist[start] = 0;
    // {距离, 节点}，最小堆
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;  // 过期条目
        for (auto [v, w] : graph[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
# 时间复杂度：O((V + E) log V)
```

**不适用场景：** 有负权边（改用 Bellman-Ford）。

### Bellman-Ford（允许负权边，可检测负权环）

```cpp
vector<int> bellmanFord(int n, vector<tuple<int,int,int>>& edges, int src) {
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;
    for (int i = 0; i < n - 1; i++)            // 松弛 n-1 轮
        for (auto& [u, v, w] : edges)
            if (dist[u] != INT_MAX && dist[u]+w < dist[v])
                dist[v] = dist[u] + w;
    // 检测负权环
    for (auto& [u, v, w] : edges)
        if (dist[u] != INT_MAX && dist[u]+w < dist[v])
            return {};  // 有负权环
    return dist;
}
# 时间复杂度：O(VE)
```

### Floyd-Warshall（全源最短路）

```cpp
void floydWarshall(vector<vector<int>>& dist) {
    int n = dist.size();
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
}
# 时间复杂度：O(V³)，适合节点数较少的稠密图
```

### 最短路算法对比

|算法|时间复杂度|负权边|负权环检测|适用场景|
|---|---|---|---|---|
|Dijkstra|O((V+E)logV)|❌|❌|单源，非负权，稀疏图|
|Bellman-Ford|O(VE)|✅|✅|单源，有负权|
|Floyd-Warshall|O(V³)|✅|✅|全源，节点少|
