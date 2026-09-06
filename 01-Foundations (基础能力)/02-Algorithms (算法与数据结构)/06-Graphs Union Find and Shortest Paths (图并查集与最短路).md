---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# Graph Traversal (图的遍历)

> [!note] 本节重点：核心考点：图的邻接表/邻接矩阵表示、DFS 递归与 BFS 队列模板、visited 标记防环

# 图的表示

```cpp
// 邻接表（稀疏图，最常用）
vector<vector<int>> adj(n);
for (auto& e : edges) {
    adj[e[0]].push_back(e[1]);
    adj[e[1]].push_back(e[0]);   // 无向图
}

// 邻接矩阵（稠密图）
vector<vector<int>> matrix(n, vector<int>(n, 0));
matrix[u][v] = weight;
```

# 图 DFS 模板

```cpp
// 图用邻接表
vector<vector<int>> graph(n);
for (auto& e : edges) { graph[e[0]].push_back(e[1]); graph[e[1]].push_back(e[0]); }

// 岛屿数量（矩阵 DFS）
void dfs(vector<vector<char>>& grid, int i, int j) {
    if (i<0||i>=(int)grid.size()||j<0||j>=(int)grid[0].size()||grid[i][j]!='1') return;
    grid[i][j] = '0';
    dfs(grid,i+1,j); dfs(grid,i-1,j); dfs(grid,i,j+1); dfs(grid,i,j-1);
}
int numIslands(vector<vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    int cnt = 0;
    for (int i=0; i<(int)grid.size(); i++)
        for (int j=0; j<(int)grid[0].size(); j++)
            if (grid[i][j]=='1') { dfs(grid,i,j); cnt++; }
    return cnt;
}
```

# 拓扑排序（有向无环图 DAG）

**Kahn 算法（BFS）：**

```cpp
vector<int> topoSort(int n, vector<vector<int>>& edges) {
    vector<vector<int>> g(n);
    vector<int> indegree(n, 0);
    for (auto& e : edges) { g[e[0]].push_back(e[1]); indegree[e[1]]++; }

    queue<int> q;
    for (int i = 0; i < n; i++) if (indegree[i] == 0) q.push(i);
    vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : g[u]) if (--indegree[v] == 0) q.push(v);
    }
    return (int)order.size() == n ? order : vector<int>{};  // 空 = 有环
}
```

**DFS 后序（检测环）：**

```cpp
vector<int> state;
bool hasCycleDFS(int u, vector<vector<int>>& g) {
    state[u] = 1;
    for (int v : g[u]) {
        if (state[v] == 1) return true;   // 发现环
        if (state[v] == 0 && hasCycleDFS(v, g)) return true;
    }
    state[u] = 2;
    return false;
}
```

---

# 30 秒回答

**DFS 和 BFS 怎么选？** 只要遍历连通性，两者都是 `O(V+E)`；DFS 的递归结构适合回溯、连通块和后序处理，深图要防递归栈溢出；无权图求最少边数时用 BFS，并在节点首次入队时标记已访问，避免重复入队。

**自测：** Kahn 拓扑排序最终输出不足 `n` 个节点说明什么？为什么图搜索必须维护 `visited`？

# 关联笔记

- Union-Find (并查集)
- Shortest Path：Dijkstra & Bellman-Ford (最短路)
- Array & Two Pointers (数组与双指针)
- Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)
- Fast & Slow Pointers (快慢指针)

---

# Union Find (并查集)

> [!note] 本节重点：核心考点：并查集的 find/union 操作、路径压缩与按秩合并优化、连通性检测与环检测

并查集用于高效处理**集合合并与连通性查询**，支持：

- `find(x)`：找到 x 的根节点（所在集合的代表）
- `union(x, y)`：合并 x 和 y 所在的集合

```cpp
class UnionFind {
    vector<int> parent, rank_;
public:
    int count;
    UnionFind(int n) : parent(n), rank_(n, 0), count(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (rank_[px] < rank_[py]) swap(px, py);  // 按秩合并
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        count--;
        return true;
    }
    bool connected(int x, int y) { return find(x) == find(y); }
};
```

**时间复杂度：** 路径压缩 + 按秩合并后，每次操作近似 O(α(n))（α 为反阿克曼函数，实际上可视为常数）。

## 典型应用

```cpp
// 冗余连接
vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    UnionFind uf(edges.size() + 1);
    for (auto& e : edges)
        if (!uf.unite(e[0], e[1])) return e;
    return {};
}

// 岛屿数量（并查集解法）：
// 遍历矩阵，每遇到 '1' 就与相邻的 '1' union，最终 count 即为
```

---

# 关联笔记 · 延伸要点 2
- DFS & BFS on Graph (图的搜索)
- Shortest Path：Dijkstra & Bellman-Ford (最短路)
- Array & Two Pointers (数组与双指针)
- Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)
- Fast & Slow Pointers (快慢指针)

---

# Shortest Path (最短路径)

> [!note] 本节重点：核心考点：Dijkstra 非负权最短路（堆优化）、Bellman-Ford 负权最短路与检测、Floyd 多源最短路

> [!warning] Dijkstra 一旦把节点从最小堆取出，就依赖“后续边不会让路径变短”的前提；**任意负权边都会破坏这个前提**。距离与边权可能很大时，使用 `long long`，并避免把 `INF + w` 当作普通整数相加。

# Dijkstra（单源最短路，非负权边）

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

# Bellman-Ford（允许负权边，可检测负权环）

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

# Floyd-Warshall（全源最短路）

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

# 最短路算法对比

|算法|时间复杂度|负权边|负权环检测|适用场景|
|---|---|---|---|---|
|Dijkstra|O((V+E)logV)|❌|❌|单源，非负权，稀疏图|
|Bellman-Ford|O(VE)|✅|✅|单源，有负权|
|Floyd-Warshall|O(V³)|✅|✅|全源，节点少|

---

# 关联笔记 · 延伸要点 3
- DFS & BFS on Graph (图的搜索)
- Union-Find (并查集)
- Array & Two Pointers (数组与双指针)
- Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)
- Fast & Slow Pointers (快慢指针)

# 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

# 常见误区

- 把 **06-Graphs Union Find and Shortest Paths (图并查集与最短路)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **06-Graphs Union Find and Shortest Paths (图并查集与最短路)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？

# 关联学习

- 导航：[00-Algorithms Map (算法导航)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/00-Algorithms%20Map%20(算法导航).md)
- 下一步：[07-Binary Search (二分查找)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/07-Binary%20Search%20(二分查找).md)
