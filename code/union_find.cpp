// Template: Union Find — Path compression + union by rank
#include <iostream>
#include <vector>
#include <numeric>
#include <cassert>
using namespace std;

struct UnionFind {
    vector<int> parent, rank;
    UnionFind(int n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }

    // Find with path compression
    int find(int x) {
        return parent[x] == x ? x : (parent[x] = find(parent[x]));
    }

    // Union by rank
    void unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return;
        if (rank[x] < rank[y]) parent[x] = y;
        else if (rank[x] > rank[y]) parent[y] = x;
        else { parent[y] = x; rank[x]++; }
    }
};

int main() {
    UnionFind uf(5);

    // Initially all are separate
    for (int i = 0; i < 5; i++)
        assert(uf.find(i) == i);

    // Unite 0-1-2
    uf.unite(0, 1);
    uf.unite(1, 2);
    assert(uf.find(0) == uf.find(1));
    assert(uf.find(1) == uf.find(2));
    assert(uf.find(0) != uf.find(3));

    // Unite 3-4
    uf.unite(3, 4);
    assert(uf.find(3) == uf.find(4));
    assert(uf.find(0) != uf.find(3));

    // Connect the two groups
    uf.unite(1, 3);
    assert(uf.find(0) == uf.find(4));

    cout << "All tests passed!" << endl;
    return 0;
}
