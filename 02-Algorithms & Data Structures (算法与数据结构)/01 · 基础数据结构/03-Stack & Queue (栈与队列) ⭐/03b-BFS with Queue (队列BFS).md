> **核心考点**：BFS 按层扩展、队列实现、无权图最短路径、visited 数组防重复


BFS 用队列实现，**按层扩展**，保证找到的路径是最短路径（无权图）。

### 模板

```cpp
int bfs(vector<vector<int>>& graph, int start, int target) {
    queue<int> q;
    unordered_set<int> visited;
    q.push(start);
    visited.insert(start);
    int steps = 0;
    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            int node = q.front(); q.pop();
            if (node == target) return steps;
            for (int nb : graph[node])
                if (!visited.count(nb)) { visited.insert(nb); q.push(nb); }
        }
        steps++;
    }
    return -1;
}
```

### 二叉树层序遍历

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    queue<TreeNode*> q;
    q.push(root);
    vector<vector<int>> res;
    while (!q.empty()) {
        int sz = q.size();
        vector<int> level;
        for (int i = 0; i < sz; i++) {
            auto node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
        res.push_back(level);
    }
    return res;
}
```

### 多源 BFS

从多个起点同时出发（如矩阵中所有 0 同时扩散），初始化时将所有起点入队：

```cpp
vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    int m = mat.size(), n = mat[0].size();
    vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
    queue<pair<int,int>> q;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (mat[i][j] == 0) { dist[i][j] = 0; q.push({i, j}); }
    int dirs[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (auto& d : dirs) {
            int nx = x+d[0], ny = y+d[1];
            if (nx>=0 && nx<m && ny>=0 && ny<n && dist[nx][ny] > dist[x][y]+1) {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }
    return dist;
}
```