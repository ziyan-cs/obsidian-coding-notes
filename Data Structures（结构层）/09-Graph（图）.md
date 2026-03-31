#data-structure #graph #vertex-edge #adjacency-list #cpp

## ⚡ TL;DR（快速决策）

- 图本质是：**由点和边组成的关系结构**
- 一看到这些场景，要优先想到图：
    - 连接关系
    - 可达性
    - 最短路
    - 环、连通块、拓扑、生成树
- 图不一定是树；树只是特殊图
- 图论题第一步通常不是写算法，而是：**先判断图的类型**

## 🧩 Core Idea（核心本质）

- 点表示对象，边表示对象间的关系
- 图可以分很多类：
    - 有向 / 无向
    - 带权 / 无权
    - 稀疏 / 稠密
- 一句话理解：
    - **图就是“谁和谁有关系”的抽象模型。**
- 题目里凡是“城市、课程、网络、路径、依赖”，都很可能抽成图

## 🔧 Usage Patterns（可复用代码模板）

1. 邻接表存图

```cpp
int n = 5;
vector<vector<int>> g(n + 1);
g[1].push_back(2);
g[2].push_back(1);
```

1. DFS

```cpp
void dfs(int u) {
    vis[u] = true;
    for (int v : g[u]) {
        if (!vis[v]) dfs(v);
    }
}
```

1. BFS

```cpp
queue<int> q;
q.push(start);
vis[start] = true;
while (!q.empty()) {
    int u = q.front(); q.pop();
}
```

1. 带权图

```cpp
vector<vector<pair<int, int>>> g(n + 1);
g[u].push_back({v, w});
```

## ⚠️ Pitfalls（高频错误）

- 无向图忘记双向加边
- 有环图忘记 `visited`
- 图和树模板混用
- 带权图存边顺序写混

## 🚀 Performance / Tips（性能优化）

- 稀疏图常用邻接表
- 稠密图、小图可考虑邻接矩阵
- 复杂度通常和点数 `n`、边数 `m` 一起看
- 高频经验：
    - 连通性：DFS / BFS / 并查集
    - 最短路：BFS / Dijkstra / Floyd
    - 有向依赖：拓扑排序

## 🧪 Common Scenarios（常见使用场景）

- 城市道路
- 社交网络
- 课程依赖
- 地图路径
- 网络连接 / 连通块

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;
int main() {
    int n = 3;
    vector<vector<int>> g(n + 1);
    g[1].push_back(2);
    g[2].push_back(3);
    for (int v : g[1]) cout << v << ' ';
}
```

## 📌 One-liner Summary（一句话总结）

- **图就是：由点和边构成、用于描述关系与连接性的结构模型。**