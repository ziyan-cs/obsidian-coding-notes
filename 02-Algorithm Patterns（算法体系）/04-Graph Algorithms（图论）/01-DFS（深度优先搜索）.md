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
    **
- DFS 最常见的实现方式是递归
- 为什么递归很适合 DFS：
    - “进入下一层” = 递归调用
    - “回到上一层” = 函数返回
- DFS 解决的问题，本质上往往是：
    - **遍历所有能到的状态**
    - **检查某个目标是否存在**
    - **统计答案**
    - **枚举全部可行方案**
- 在不同题型里，DFS 的含义会略有不同：
    - 在树 / 图题中：偏向遍历
    - 在搜索题中：偏向试探
    - 在回溯题中：偏向“做选择 → 递归 → 撤销选择”

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

要点：

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

要点：

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

要点：

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

要点：

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

要点：

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

要点：

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

要点：

- 全排列题核心是：
    - 哪些数已经用过
    - 当前路径是什么
- `used[i] = false` 和 `pop_back()` 都不能忘

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：忘记标记访问
    - 图题里不标记 `visited`，很容易死循环
- 最常见错误 2：标记时机错误
    - 通常进入节点就标记，而不是处理完才标记
- 最常见错误 3：回溯没撤销现场
    - `push_back()` 之后递归，回来要 `pop_back()`
    - 改了 `used[i]` 也要恢复
- 最常见错误 4：树和图模板混用
    - 树常传 `parent`
    - 图常用 `visited`
- 最常见错误 5：递归边界写错
    - 不同题目的终止条件不一样，要先想清楚
- 最常见错误 6：网格题边界没判干净
    - 越界、障碍、访问状态常漏一个
- 最常见错误 7：递归太深爆栈
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

## 📌 One-liner Summary（一句话总结）

- **DFS 就是：从当前状态一路往深处搜索，走不通再回退，再继续试其他分支。**