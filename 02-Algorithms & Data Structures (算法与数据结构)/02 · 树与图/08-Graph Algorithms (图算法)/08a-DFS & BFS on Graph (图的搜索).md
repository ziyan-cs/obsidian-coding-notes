# Graph Search (DFS & BFS)
#algorithm #dfs #graph #tree #backtracking

## ⚡ TL;DR（快速决策）

- DFS（Depth First Search，深度优先搜索）本质是：**一条路先走到底，走不通再回退**
- 相关特征：
    - 附有一棵树 / 一张图
    - 遍历所有连通块 / 所有节点
    - 找一条可行路径
    - 枚举所有方案、路径、排列组合
    - 涉及“搜索 + 回退”的过程
- 常见应用：
    - 树遍历
    - 图遍历
    - 连通块统计
    - 岛屿问题
    - 回溯搜索
    - 拓扑结构中的递归遍历思想
- DFS 不一定比 BFS 更高级，只是**搜索顺序不同**
- 如果题目是“把所有可能一路试下去，不行再撤回”，大概率是 DFS / 回溯

## 🧩 Core Idea（核心本质）

- DFS 的思路：
    - 从当前点出发
    - 先进入一个分支
    - 一直往深处走
    - 走不下去再退回来
    - 再试下一个分支
- 一句话理解：
    - **先钻到底，再回头。
	
- DFS 最常见的实现方式是 `递归`
- `递归` 适合 DFS 原因：
    - “进入下一层” = 递归调用
    - “回到上一层” = 函数返回
    
- 解决的问题本质：
    - **遍历所有能到的状态**
    - **检查某个目标是否存在**
    - **统计答案**
    - **枚举全部可行方案**
- 在不同题型里，DFS 的含义会略有不同：
    - 树 / 图题中：偏向遍历
    - 搜索题中：偏向试探
    - 回溯题中：偏向“做选择 → 递归 → 撤销选择”

## 🔧 Usage Patterns（可复用代码模板）

1. 树的 DFS

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;

void dfs(int u, int parent) {
    cout << u << '\n';
    for (int v : g[u]) {
        if (v == parent) continue;
        dfs(v, u);
    }
}

int main() {
    int n = 5;
    g.assign(n + 1, {});

    g[1] = {2, 3};
    g[2] = {1, 4, 5};
    g[3] = {1};
    g[4] = {2};
    g[5] = {2};

    dfs(1, 0);
    return 0;
}
```

- 树没有环处理也不能乱写，通常要传 `parent`
- 从根递归遍历整棵树

1. 图的 DFS（visited 版）

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

void dfs(int u) {
    vis[u] = true;
    cout << u << '\n';

    for (int v : g[u]) {
        if (vis[v]) continue;
        dfs(v);
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

    dfs(1);
    return 0;
}
```

- 图里可能有环，所以通常要 `visited`
- `vis[u] = true` 一般要在进入节点时立刻标记

1. 统计连通块

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

void dfs(int u) {
    vis[u] = true;
    for (int v : g[u]) {
        if (!vis[v]) dfs(v);
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
            dfs(i);
        }
    }

    cout << components << '\n';
    return 0;
}
```

- 每次从一个未访问点重新 DFS，就找到一个新连通块
- 这是图题超级高频模型

1. 网格 DFS（岛屿类）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int n = 3, m = 4;
vector<string> grid = {
    "1100",
    "1101",
    "0011"
};
vector<vector<bool>> vis;
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};

void dfs(int x, int y) {
    vis[x][y] = true;
    for (int k = 0; k < 4; ++k) {
        int nx = x + dx[k];
        int ny = y + dy[k];
        if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
        if (vis[nx][ny] || grid[nx][ny] == '0') continue;
        dfs(nx, ny);
    }
}

int main() {
    vis.assign(n, vector<bool>(m, false));
    int islands = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (!vis[i][j] && grid[i][j] == '1') {
                ++islands;
                dfs(i, j);
            }
        }
    }

    cout << islands << '\n';
    return 0;
}
```

- 岛屿、感染、连通区域问题几乎都是 DFS / BFS 模板题
- 关键点：边界判断、访问标记、方向数组

1. 找路径是否存在

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

bool dfs(int u, int target) {
    if (u == target) return true;
    vis[u] = true;

    for (int v : g[u]) {
        if (!vis[v] && dfs(v, target)) {
            return true;
        }
    }
    return false;
}

int main() {
    int n = 4;
    g.assign(n + 1, {});
    vis.assign(n + 1, false);

    g[1] = {2};
    g[2] = {1, 3};
    g[3] = {2, 4};
    g[4] = {3};

    cout << dfs(1, 4) << '\n';
    return 0;
}
```

- 如果只是判断“能不能到”，可以在搜索到答案时立刻返回
- 这是 DFS 的常见剪枝思路

1. 回溯：枚举所有子集

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<int> nums = {1, 2, 3};
vector<int> path;

void dfs(int idx) {
    if (idx == (int)nums.size()) {
        cout << "{ ";
        for (int x : path) cout << x << ' ';
        cout << "}\n";
        return;
    }

    dfs(idx + 1);  // 不选

    path.push_back(nums[idx]);
    dfs(idx + 1);  // 选
    path.pop_back();
}

int main() {
    dfs(0);
    return 0;
}
```

- 这是“选 / 不选”型 DFS 的标准模板
- `push_back` 后递归，回来要 `pop_back`

1. 回溯：全排列

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<int> nums = {1, 2, 3};
vector<int> path;
vector<bool> used(3, false);

void dfs() {
    if ((int)path.size() == (int)nums.size()) {
        for (int x : path) cout << x << ' ';
        cout << '\n';
        return;
    }

    for (int i = 0; i < (int)nums.size(); ++i) {
        if (used[i]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        dfs();
        path.pop_back();
        used[i] = false;
    }
}

int main() {
    dfs();
    return 0;
}
```

- 全排列题核心是：
    - 哪些数已经用过
    - 当前路径是什么
- `used[i] = false` 和 `pop_back()` 都不能忘

## ⚠️ Pitfalls（高频错误）

- 1：忘记标记访问
    - 图题里不标记 `visited`，很容易死循环
- 2：标记时机错误
    - 通常进入节点就标记，而不是处理完才标记
- 3：回溯没撤销现场
    - `push_back()` 之后递归，回来要 `pop_back()`
    - 改了 `used[i]` 也要恢复
- 4：树和图模板混用
    - 树常传 `parent`
    - 图常用 `visited`
- 5：递归边界写错
    - 不同题目的终止条件不一样，要先想清楚
- 6：网格题边界没判干净
    - 越界、障碍、访问状态常漏一个
- 7：递归太深爆栈
    - 数据很大时递归 DFS 可能栈溢出
    - 有些题更适合手写栈或改 BFS

## 🚀 Performance / Tips（性能优化）

- DFS 的复杂度常见理解：
    - 树 / 图遍历：通常是 $O(n + m)$
    - 网格遍历：通常是 $O(n \times m)$
    - 回溯搜索：通常和状态总数有关，可能是指数级
- 经验判断：
    - 只是遍历所有点：DFS 很稳
    - 要枚举所有方案：要小心指数爆炸
- 实战建议：
    - 图题先想清楚邻接表怎么存
    - 网格题先准备方向数组
    - 回溯题先定义“路径”“已用状态”“终止条件”
- DFS 想清楚这 3 件事，代码就稳很多：
    1. 当前状态是什么？
    2. 从当前状态能去哪里？
    3. 什么时候停？
- 如果只是遍历，不需要存整条路径，就别乱存，减少不必要开销
- 如果要找最短路，**优先先想 BFS**，DFS 往往不是最自然选择

## 🧪 Common Scenarios（常见使用场景）

- **树遍历**：先序遍历、统计子树信息、求深度
- **图遍历**：连通性、可达性、连通块数量
- **网格问题**：岛屿数量、感染扩散、区域搜索
- **搜索枚举**：子集、排列、组合、分割方案
- **路径问题**：是否存在路径、枚举所有路径
- **回溯问题**：N 皇后、数独、括号生成、组合总和

## 🆚 DFS vs 其他常见方法

- **vs BFS（广度优先搜索）**
    - DFS：先往深处走
    - BFS：一层一层扩展
    - BFS 更适合最短路（无权图）
- **vs Backtracking（回溯）**
    - 回溯本质上通常就是带撤销操作的 DFS
    - 可以理解成 DFS 的搜索型用法
- **vs DP（动态规划）**
    - DFS 偏搜索过程
    - DP 偏重复子问题优化
- **vs Union Find（并查集）**
    - 连通块问题既可以 DFS / BFS，也可以并查集
    - DFS 更直观，并查集更偏结构化维护

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> g;
vector<bool> vis;

void dfs(int u) {
    vis[u] = true;
    cout << u << '\\n';
    for (int v : g[u]) {
        if (!vis[v]) dfs(v);
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

    dfs(1);
    return 0;
}
```
#algorithm #bfs #graph #tree #queue

## ⚡ TL;DR（快速决策）

- BFS（Breadth First Search，广度优先搜索）本质是：**从起点一层一层向外扩展**
- 相关特征：
    - 求**最短步数 / 最少操作次数 / 最短路径（无权图）**
    - “从一个状态扩展到下一批状态”
    - 涉及按层推进、波纹扩散、感染传播
    - 网格最短路、迷宫最短路、状态最少变换次数
    - 树的层序遍历
- 常见应用：
    - 图的最短路（无权图）
    - 网格最短路
    - 连通块遍历
    - 树的层序遍历
    - 多源扩散问题
    - 状态图最少步数问题
- BFS 和 DFS 不是谁更高级，而是**适用场景不同**
- 如果题目问的是“最少几步到达”，优先想到 BFS

## 🧩 Core Idea（核心本质）

- BFS 的思路：
    - 从起点出发
    - 先访问所有距离为 1 的点
    - 再访问所有距离为 2 的点
    - 再访问所有距离为 3 的点
- 一句话理解：
    - **一圈一圈往外扩。
    
- BFS 最经典的数据结构是：`queue`
- `queue` 适合 BFS 的原因：
    - 先进入队列的点先被处理
    - 正好保证“按层、按距离从近到远”地扩展
	
- 解决的问题本质：
    - **分层遍历所有可达状态**
    - **求无权图最短路 / 最少步数**
    - **按层统计答案**
    - **模拟波纹式扩散**
- 在不同题型里，BFS 的含义会略有不同：
    - 树题中：偏向层序遍历
    - 图题中：偏向最短路 / 可达性
    - 网格题中：偏向最少步数 / 扩散过程
    - 状态搜索题中：偏向“从起点状态扩展到目标状态”

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

        cout << u << '\n';
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

        cout << u << '\n';
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
    cout << dist[4] << '\n';
    return 0;
}
```

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

    cout << components << '\n';
    return 0;
}
```

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
    cout << dist[2][3] << '\n';
    return 0;
}
```

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

    cout << dist[1][1] << '\n';
    return 0;
}
```

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
    cout << bfs(2, 9) << '\n';
    return 0;
}
```

- 不一定非得是图上的点，很多题是“状态”构成图
- BFS 非常适合求最少操作次数

## ⚠️ Pitfalls（高频错误）

- 1：访问标记太晚
    - 通常在**入队时**就标记
    - 如果出队时才标记，可能重复入队很多次
- 2：把 BFS 写成了普通遍历，却忘了维护距离
    - 求最短步数时，通常要维护 `dist`
- 3：把有权图也直接拿 BFS 跑最短路
    - 普通 BFS 适用于**无权图**或边权全相同的图
    - 有权图要优先想 Dijkstra / 0-1 BFS 等
- 4：网格题边界没判干净
    - 越界、障碍、访问状态常漏一个
- 5：层数统计写乱
    - 有时题目要“第几层”“扩散几轮”，要么用 `dist`，要么按队列大小分层
- 6：树和图模板混用
    - 图要防重
    - 树如果有父边，也别忘了避免来回走
- 7：状态空间太大没做限制
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
    cout << dist[4] << '\n';
    return 0;
}
```