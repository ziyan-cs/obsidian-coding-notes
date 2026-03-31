#algorithm #graph #floyd #all-pairs-shortest-path #dp

## ⚡ TL;DR（快速决策）

- Floyd 本质是：**求任意两点之间的最短路**
- 一看到这些特征，要优先想到 Floyd：
    - 多次询问任意两点最短路
    - 点数不大
    - 需要全源最短路
- 经典复杂度：$O(n^3)$
- 点数大时通常不适合

## 🧩 Core Idea（核心本质）

- Floyd 是一种基于 DP 的最短路算法
- 核心思想：
    - 枚举一个中转点 `k`
    - 看 `i -> j` 是否可以通过 `i -> k -> j` 变得更短
- 状态转移：
    - `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`
- 一句话理解：
    - **尝试让每个点都当一次中转站。**

## 🔧 Usage Patterns（可复用代码模板）

1. Floyd 标准模板

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 4;
    const int INF = 1e9;
    vector<vector<int>> dist(n + 1, vector<int>(n + 1, INF));

    for (int i = 1; i <= n; ++i) dist[i][i] = 0;
    dist[1][2] = 3;
    dist[2][3] = 4;
    dist[1][3] = 10;
    dist[3][4] = 2;

    for (int k = 1; k <= n; ++k) {
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }

    cout << dist[1][4] << '\\n';
}
```

1. 判负环的经典思路

```cpp
// 若某个 dist[i][i] < 0，则可能存在负环
```

## ⚠️ Pitfalls（高频错误）

- `INF` 要足够大，但也别大到加法溢出
- 转移前常要判断 `dist[i][k]`、`dist[k][j]` 是否可达
- 三层循环顺序经典写法是：`k -> i -> j`
- 点数稍大时就会超时，别乱用

## 🚀 Performance / Tips（性能优化）

- 复杂度 $O(n^3)$，空间常为 $O(n^2)$
- 更适合：
    - 点数小
    - 查询多
    - 需要任意两点最短路
- 如果只是单源最短路，通常别用 Floyd

## 🧪 Common Scenarios（常见使用场景）

- 任意两点最短路
- 图传递闭包思路
- 小图多次查询
- 负环判定（变形）

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 3;
    const int INF = 1e9;
    vector<vector<int>> dist(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i) dist[i][i] = 0;
    dist[0][1] = 2;
    dist[1][2] = 3;
    dist[0][2] = 10;

    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);

    cout << dist[0][2] << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **Floyd 就是：枚举每个点作为中转站，从而求出任意两点间的最短路。**