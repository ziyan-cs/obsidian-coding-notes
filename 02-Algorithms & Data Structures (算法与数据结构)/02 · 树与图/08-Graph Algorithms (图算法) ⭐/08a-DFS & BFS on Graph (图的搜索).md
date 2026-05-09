
### 图的表示

python

```python
# 邻接表（稀疏图，最常用）
graph = defaultdict(list)
for u, v in edges:
    graph[u].append(v)
    graph[v].append(u)    # 无向图

# 邻接矩阵（稠密图，边数接近 n²）
n = 5
matrix = [[0]*n for _ in range(n)]
matrix[u][v] = weight
```

### 图 DFS 模板

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
    int cnt = 0;
    for (int i=0; i<(int)grid.size(); i++)
        for (int j=0; j<(int)grid[0].size(); j++)
            if (grid[i][j]=='1') { dfs(grid,i,j); cnt++; }
    return cnt;
}
```

### 拓扑排序（有向无环图 DAG）

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