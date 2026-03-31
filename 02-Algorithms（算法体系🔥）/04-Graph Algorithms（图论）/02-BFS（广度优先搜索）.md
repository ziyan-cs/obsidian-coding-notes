#algorithm #bfs #graph #tree #queue

## ⚡ TL;DR（快速决策）

- BFS（Breadth First Search，广度优先搜索）本质是：**从起点一层一层向外扩展**
- 一看到这些特征，要优先想到 BFS：
    - 题目让你求**最短步数 / 最少操作次数 / 最短路径（无权图）**
    - 题目是“从一个状态扩展到下一批状态”
    - 涉及按层推进、波纹扩散、感染传播
    - 网格最短路、迷宫最短路、状态最少变换次数
    - 树的层序遍历
- BFS 常见应用：
    - 图的最短路（无权图）
    - 网格最短路
    - 连通块遍历
    - 树的层序遍历
    - 多源扩散问题
    - 状态图最少步数问题
- BFS 和 DFS 不是谁更高级，而是**适用场景不同**
- 如果题目问的是“最少几步到达”，优先先想 BFS

## 🧩 Core Idea（核心本质）

- BFS 的核心思路：
    - 从起点出发
    - 先访问所有距离为 1 的点
    - 再访问所有距离为 2 的点
    - 再访问所有距离为 3 的点
- 一句话理解：
    - **一圈一圈往外扩。**
- BFS 最经典的数据结构是：`queue`
- 为什么队列适合 BFS：
    - 先进入队列的点先被处理
    - 正好保证“按层、按距离从近到远”地扩展
- BFS 解决的问题，本质上往往是：
    - **分层遍历所有可达状态**
    - **求无权图最短路 / 最少步数**
    - **按层统计答案**
    - **模拟波纹式扩散**
- 在不同题型里，BFS 的含义会略有不同：
    - 在树题中：偏向层序遍历
    - 在图题中：偏向最短路 / 可达性
    - 在网格题中：偏向最少步数 / 扩散过程
    - 在状态搜索题中：偏向“从起点状态扩展到目标状态”

## 🔧 Usage Patterns（可复用代码模板）

1. 树的层序遍历

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<vector<int>> g;

void bfs(int root) {
    queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        cout << u << '\\n';
        for (int v : g[u]) {
            q.push(v);
        }
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});

    g[1] = {2, 3};
    g[2] = {4, 5};

    bfs(1);
    return 0;
}
```

要点：

- 树的 BFS 常用于层序遍历
- 如果不是严格有向树结构，仍要注意去重 / 父节点问题

1. 图的 BFS（visited 版）

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

void bfs(int start) {
    queue<int> q;
    q.push(start);
    vis[start] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        cout << u << '\\n';
        for (int v : g[u]) {
            if (vis[v]) continue;
            vis[v] = true;
            q.push(v);
        }
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});
    vis.assign(n + 1, false);

    g[1] = {2, 3};
    g[2] = {1, 4};
    g[3] = {1, 5};
    g[4] = {2};
    g[5] = {3};

    bfs(1);
    return 0;
}
```

要点：

- 图里可能有环，所以通常要 `visited`
- `vis[v] = true` 一般在**入队时**就标记

1. 无权图最短路

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<int> dist;

void bfs(int start) {
    queue<int> q;
    q.push(start);
    dist[start] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : g[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});
    dist.assign(n + 1, -1);

    g[1] = {2, 3};
    g[2] = {1, 4};
    g[3] = {1, 5};
    g[4] = {2};
    g[5] = {3};

    bfs(1);
    cout << dist[4] << '\\n';
    return 0;
}
```

要点：

- BFS 求无权图最短路是超级高频模板
- `dist[v] = dist[u] + 1` 就是“层数 + 1”

1. 统计连通块

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

void bfs(int start) {
    queue<int> q;
    q.push(start);
    vis[start] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : g[u]) {
            if (vis[v]) continue;
            vis[v] = true;
            q.push(v);
        }
    }
}

int main() {
    int n = 6;
    g.assign(n + 1, {});
    vis.assign(n + 1, false);

    g[1] = {2};
    g[2] = {1, 3};
    g[3] = {2};
    g[4] = {5};
    g[5] = {4};
    g[6] = {};

    int components = 0;
    for (int i = 1; i <= n; ++i) {
        if (!vis[i]) {
            ++components;
            bfs(i);
        }
    }

    cout << components << '\\n';
    return 0;
}
```

要点：

- 和 DFS 一样，BFS 也能统计连通块
- 本质仍然是“从一个起点把这一整块全扩完”

1. 网格 BFS（最短路模板）

```cpp
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
using namespace std;

int n = 3, m = 4;
vector<string> grid = {
    "S...",
    ".##.",
    "...T"
};
vector<vector<int>> dist;
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};

void bfs(int sx, int sy) {
    queue<pair<int, int>> q;
    q.push({sx, sy});
    dist[sx][sy] = 0;

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
            if (grid[nx][ny] == '#') continue;
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }
}

int main() {
    dist.assign(n, vector<int>(m, -1));
    bfs(0, 0);
    cout << dist[2][3] << '\\n';
    return 0;
}
```

要点：

- 网格最短路、迷宫最短路几乎是 BFS 标配
- 关键点：边界判断、障碍判断、访问标记、距离数组

1. 多源 BFS

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int n = 3, m = 3;
vector<string> grid = {
    "100",
    "000",
    "001"
};
vector<vector<int>> dist;
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};

int main() {
    queue<pair<int, int>> q;
    dist.assign(n, vector<int>(m, -1));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j] == '1') {
                q.push({i, j});
                dist[i][j] = 0;
            }
        }
    }

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k];
            int ny = y + dy[k];
            if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }

    cout << dist[1][1] << '\\n';
    return 0;
}
```

要点：

- 多个起点同时入队
- 常用于：最近距离、扩散传播、火焰蔓延、腐烂橘子这类题

1. 状态图 BFS（最少操作次数）

```cpp
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

int bfs(int start, int target) {
    queue<int> q;
    unordered_map<int, int> dist;

    q.push(start);
    dist[start] = 0;

    while (!q.empty()) {
        int x = q.front();
        q.pop();

        if (x == target) return dist[x];

        vector<int> nexts = {x + 1, x - 1, x * 2};
        for (int y : nexts) {
            if (y < 0 || y > 100) continue;
            if (dist.count(y)) continue;
            dist[y] = dist[x] + 1;
            q.push(y);
        }
    }

    return -1;
}

int main() {
    cout << bfs(2, 9) << '\\n';
    return 0;
}
```

要点：

- 不一定非得是图上的点，很多题是“状态”构成图
- BFS 非常适合求最少操作次数

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：访问标记太晚
    - 通常在**入队时**就标记
    - 如果出队时才标记，可能重复入队很多次
- 最常见错误 2：把 BFS 写成了普通遍历，却忘了维护距离
    - 求最短步数时，通常要维护 `dist`
- 最常见错误 3：把有权图也直接拿 BFS 跑最短路
    - 普通 BFS 适用于**无权图**或边权全相同的图
    - 有权图要优先想 Dijkstra / 0-1 BFS 等
- 最常见错误 4：网格题边界没判干净
    - 越界、障碍、访问状态常漏一个
- 最常见错误 5：层数统计写乱
    - 有时题目要“第几层”“扩散几轮”，要么用 `dist`，要么按队列大小分层
- 最常见错误 6：树和图模板混用
    - 图要防重
    - 树如果有父边，也别忘了避免来回走
- 最常见错误 7：状态空间太大没做限制
    - 状态图 BFS 要先想清楚状态范围，不然容易爆空间

## 🚀 Performance / Tips（性能优化）

- BFS 的复杂度常见理解：
    - 图遍历：通常是 $O(n + m)$
    - 网格遍历：通常是 $O(n \times m)$
    - 状态图：通常和状态总数、转移数有关
- 经验判断：
    - 只要每个点 / 状态最多进队一次，复杂度通常就稳
    - 如果状态会被重复入队，往往说明访问标记时机错了
- 实战建议：
    - 图题先想清楚邻接表怎么存
    - 网格题先准备方向数组
    - 最短步数题先准备 `dist`
    - 多源问题先想“能不能把所有起点一次性入队”
- BFS 想清楚这 3 件事，代码就稳很多：
    1. 初始状态有哪些？
    2. 当前状态能扩展到哪里？
    3. 什么时候算到答案？
- 如果题目要求最短路 / 最少步数，BFS 往往比 DFS 更自然、更稳

## 🧪 Common Scenarios（常见使用场景）

- **树的层序遍历**：按层输出、统计每层节点
- **图最短路**：无权图从起点到终点最短步数
- **网格最短路**：迷宫、最短移动步数
- **扩散问题**：病毒传播、火焰蔓延、腐烂橘子
- **连通性问题**：遍历图 / 网格、统计连通块
- **状态搜索问题**：最少操作次数、最少变换次数

## 🆚 BFS vs 其他常见方法

- **vs DFS（深度优先搜索）**
    - BFS：按层扩展
    - DFS：一路往深处走
    - 求最短路（无权图）通常优先 BFS
- **vs Dijkstra**
    - BFS：适合无权图
    - Dijkstra：适合非负权图
- **vs 0-1 BFS**
    - 普通 BFS：边权都相同
    - 0-1 BFS：边权只有 0 和 1
- **vs DP（动态规划）**
    - BFS 偏状态扩展过程
    - DP 偏重复子问题优化

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<int> dist;

void bfs(int start) {
    queue<int> q;
    q.push(start);
    dist[start] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : g[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});
    dist.assign(n + 1, -1);

    g[1] = {2, 3};
    g[2] = {1, 4};
    g[3] = {1, 5};
    g[4] = {2};
    g[5] = {3};

    bfs(1);
    cout << dist[4] << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **BFS 就是：从起点开始一层一层向外扩展，因此天然适合求无权图最短路和最少步数。**