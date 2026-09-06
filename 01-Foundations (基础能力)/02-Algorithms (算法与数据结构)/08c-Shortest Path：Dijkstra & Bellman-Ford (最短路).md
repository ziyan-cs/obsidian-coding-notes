---
tags:
  - cs/algorithm
status: 🌱
---

# Shortest Path：Dijkstra & Bellman-Ford — 最短路

> [!abstract] 核心考点：Dijkstra 非负权最短路（堆优化）、Bellman-Ford 负权最短路与检测、Floyd 多源最短路

> [!warning] Dijkstra 一旦把节点从最小堆取出，就依赖“后续边不会让路径变短”的前提；**任意负权边都会破坏这个前提**。距离与边权可能很大时，使用 `long long`，并避免把 `INF + w` 当作普通整数相加。

## Dijkstra（单源最短路，非负权边）

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
// 时间复杂度：O((V + E) log V)
```

**不适用场景：** 有负权边（改用 Bellman-Ford）。

## Bellman-Ford（允许负权边，可检测负权环）

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
// 时间复杂度：O(VE)
```

> [!tip] 此示例用空 `vector` 表示“从 `src` 可达的负权环”。工程接口最好把“最短距离”和“是否存在负环”分开表达，避免与“空图/异常输入”的含义混淆。

## Floyd-Warshall（全源最短路）

```cpp
void floydWarshall(vector<vector<int>>& dist) {
    int n = dist.size();
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
}
// 时间复杂度：O(V³)，适合节点数较少的稠密图
```

## 最短路算法对比

|算法|时间复杂度|负权边|负权环检测|适用场景|
|---|---|---|---|---|
|Dijkstra|O((V+E)logV)|❌|❌|单源，非负权，稀疏图|
|Bellman-Ford|O(VE)|✅|✅|单源，有负权|
|Floyd-Warshall|O(V³)|✅|✅|全源，节点少|

---

## 30 秒回答

**如何选最短路算法？** 单源、边权非负时优先 Dijkstra + 最小堆；允许负权且需要检测从源点可达的负环时用 Bellman-Ford；节点数较少、要一次得到任意两点距离时用 Floyd-Warshall。任何实现都先明确“不可达”和“负环”在返回值里的表示，并防止距离相加溢出。

**自测：** 为什么 Dijkstra 不能处理负权边？Bellman-Ford 只需做 `V-1` 轮松弛的原因是什么？

## 关联笔记

- [DFS & BFS on Graph (图的搜索)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08a-DFS%20&%20BFS%20on%20Graph%20(图的搜索).md)
- [Union-Find (并查集)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08b-Union-Find%20(并查集).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
