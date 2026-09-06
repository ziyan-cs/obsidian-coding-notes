---
tags:
  - cs/algorithm
status: 🌱
---

# BFS with Queue — 队列BFS

> [!important] **核心考点**：BFS 按层扩展、队列实现、无权图最短路径、visited 数组防重复


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

---

## 关联笔记

- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Hash Table (哈希表)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)
