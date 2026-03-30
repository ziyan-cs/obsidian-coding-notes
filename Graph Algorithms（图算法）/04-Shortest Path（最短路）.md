#algorithm #graph #shortest-path #dijkstra #bfs

## ⚡ TL;DR（快速决策）

- 最短路问题本质是：**从起点到终点，总代价最小**
- 一看到这些词，要先想到最短路：
    - 最少距离
    - 最少代价
    - 最少时间
    - 最少步数
- 先判断图的类型再选算法：
    - **无权图**：BFS
    - **边权非负**：Dijkstra
    - **可有负边**：Bellman-Ford / SPFA（视题目）
    - **多源任意两点**：Floyd
- 做题第一步不是写算法，而是：**先认图和边权性质**

## 🧩 Core Idea（核心本质）

- 最短路不是只有一种算法，而是一类问题
- 核心就是：
    - 点代表状态
    - 边代表转移
    - 边权代表代价
- 不同最短路算法，本质是在不同限制下更高效地算距离
- 一句话理解：
    - **先看边权，再选算法。**

## 🔧 Usage Patterns（可复用代码模板）

1. 无权图最短路：BFS

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<vector<int>> g(n + 1);
    g[1] = {2, 3};
    g[2] = {4};
    g[3] = {5};

    vector<int> dist(n + 1, -1);
    queue<int> q;
    q.push(1);
    dist[1] = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : g[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }

    cout << dist[4] << '\\n';
}
```

1. Dijkstra 适用场景

```cpp
// 边权非负时常用
```

1. Floyd 适用场景

```cpp
// 点数较小、要求任意两点最短路时常用
```

## ⚠️ Pitfalls（高频错误）

- 无权图别硬上 Dijkstra
- 有负边别乱用 Dijkstra
- 最短路题第一步要先判断：单源还是多源、是否有负边、点数边数范围
- 距离初始化常用“大数”或 `-1`（看问题类型）

## 🚀 Performance / Tips（性能优化）

- 高频选型记忆：
    - BFS：$O(n + m)$
    - Dijkstra（堆优化）：$O(m log n)$
    - Floyd：$O(n^3)$
- 图大时优先邻接表
- 点数小但要多次询问时，Floyd 很常见

## 🧪 Common Scenarios（常见使用场景）

- 地图路径
- 网络延迟
- 最少花费 / 最少时间 / 最少步数
- 状态图搜索

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
    vector<vector<int>> g(4);
    g[0] = {1, 2};
    g[1] = {3};
    g[2] = {3};

    vector<int> dist(4, -1);
    queue<int> q;
    q.push(0);
    dist[0] = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : g[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    cout << dist[3] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **最短路问题的核心不是背算法，而是先根据边权和范围判断该用哪一种算法。**