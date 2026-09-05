---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：并查集 find/union、路径压缩与按秩合并、连通性检测
> [!important] **核心考点**：并查集的 find/union 操作、路径压缩与按秩合并优化、连通性检测与环检测

并查集用于高效处理**集合合并与连通性查询**，支持：

- `find(x)`：找到 x 的根节点（所在集合的代表）
- `union(x, y)`：合并 x 和 y 所在的集合

```cpp
class UnionFind {
    vector<int> parent, rank_;
public:
    int count;
    UnionFind(int n) : parent(n), rank_(n, 0), count(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }
    bool unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return false;
        if (rank_[px] < rank_[py]) swap(px, py);  // 按秩合并
        parent[py] = px;
        if (rank_[px] == rank_[py]) rank_[px]++;
        count--;
        return true;
    }
    bool connected(int x, int y) { return find(x) == find(y); }
};
```

**时间复杂度：** 路径压缩 + 按秩合并后，每次操作近似 O(α(n))（α 为反阿克曼函数，实际上可视为常数）。

### 典型应用

```cpp
// 冗余连接
vector<int> findRedundantConnection(vector<vector<int>>& edges) {
    UnionFind uf(edges.size() + 1);
    for (auto& e : edges)
        if (!uf.unite(e[0], e[1])) return e;
    return {};
}

# 岛屿数量（并查集解法）
# 遍历矩阵，每遇到 '1' 就与相邻的 '1' union，最终 count 即为
```

---

## 关联笔记

- [DFS & BFS on Graph (图的搜索)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08a-DFS%20&%20BFS%20on%20Graph%20(图的搜索).md)
- [Shortest Path：Dijkstra & Bellman-Ford (最短路)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/08-Graph%20Algorithms%20(图算法)%20⭐/08c-Shortest%20Path：Dijkstra%20&%20Bellman-Ford%20(最短路).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
