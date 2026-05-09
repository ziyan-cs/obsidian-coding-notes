
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