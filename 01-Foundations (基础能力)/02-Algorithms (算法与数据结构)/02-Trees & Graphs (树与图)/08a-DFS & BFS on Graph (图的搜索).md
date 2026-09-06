---
tags:
  - cs/algorithm
status: 🌱
---

# DFS & BFS on Graph — 图的搜索

> [!important] **核心考点**：图的邻接表/邻接矩阵表示、DFS 递归与 BFS 队列模板、visited 标记防环

## 图的表示

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

## 图 DFS 模板

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

## 拓扑排序（有向无环图 DAG）

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

## 30 秒回答

**DFS 和 BFS 怎么选？** 只要遍历连通性，两者都是 `O(V+E)`；DFS 的递归结构适合回溯、连通块和后序处理，深图要防递归栈溢出；无权图求最少边数时用 BFS，并在节点首次入队时标记已访问，避免重复入队。

**自测：** Kahn 拓扑排序最终输出不足 `n` 个节点说明什么？为什么图搜索必须维护 `visited`？

## 关联笔记

- [Union-Find (并查集)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08b-Union-Find%20(并查集).md)
- [Shortest Path：Dijkstra & Bellman-Ford (最短路)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08c-Shortest%20Path：Dijkstra%20&%20Bellman-Ford%20(最短路).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
