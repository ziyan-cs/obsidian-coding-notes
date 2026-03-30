#algorithm #graph #dijkstra #shortest-path #heap

## ⚡ TL;DR（快速决策）

- Dijkstra 本质是：**在边权非负的图中求单源最短路**
- 一看到这些特征，要优先想到 Dijkstra：
    - 单源最短路
    - 所有边权都 >= 0
    - 需要从起点求到所有点或某终点最小距离
- 最常见写法：**邻接表 + 小根堆**
- 如果图里有负边，别用 Dijkstra

## 🧩 Core Idea（核心本质）

- Dijkstra 的核心贪心：
    - 每次取当前“距离起点最近、且还没确定”的点
    - 用它去松弛别的点
- 因为边权非负，所以先弹出的最短距离就能被确认
- 一句话理解：
    - **每次先确定当前最近的点。**

## 🔧 Usage Patterns（可复用代码模板）

1. 堆优化 Dijkstra 模板

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
using PII = pair<int, int>;

int main() {
    int n = 5;
    vector<vector<PII>> g(n + 1);
    g[1].push_back({2, 2});
    g[1].push_back({3, 5});
    g[2].push_back({3, 1});
    g[2].push_back({4, 4});
    g[3].push_back({4, 1});

    const int INF = 1e9;
    vector<int> dist(n + 1, INF);
    priority_queue<PII, vector<PII>, greater<PII>> pq;

    dist[1] = 0;
    pq.push({0, 1});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d != dist[u]) continue;

        for (auto [v, w] : g[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    cout << dist[4] << '\\n';
}
```

1. 只求到终点时的理解

```cpp
// 若终点最短路已被弹出，往往可以认为已确定
```

## ⚠️ Pitfalls（高频错误）

- 图里有负边时不能用
- 堆里可能有旧状态，所以要写：`if (d != dist[u]) continue;`
- 邻接表里常把 `pair` 顺序写反，注意是 `{to, weight}` 还是 `{weight, to}`
- 距离初始化要足够大

## 🚀 Performance / Tips（性能优化）

- 堆优化复杂度常记作 $O(m \log n)$
- 图稀疏时很常用
- 邻接表 + 小根堆是标准写法
- 如果边权全是 1，其实 BFS 更自然

## 🧪 Common Scenarios（常见使用场景）

- 地图最短距离
- 网络延迟时间
- 最少花费路径
- 各类单源最短路题

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;
using PII = pair<int, int>;

int main() {
    vector<vector<PII>> g(3);
    g[0].push_back({1, 2});
    g[1].push_back({2, 3});
    g[0].push_back({2, 10});

    vector<int> dist(3, 1e9);
    priority_queue<PII, vector<PII>, greater<PII>> pq;
    dist[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        for (auto [v, w] : g[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    cout << dist[2] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **Dijkstra 就是：在非负权图中，每次先确定当前最近的点来推进单源最短路。**