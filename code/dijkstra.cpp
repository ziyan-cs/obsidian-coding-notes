// Template: Dijkstra — Shortest path from single source
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cassert>
using namespace std;

vector<int> dijkstra(const vector<vector<pair<int,int>>>& g, int start) {
    int n = (int)g.size();
    vector<int> dist(n, INT_MAX);
    dist[start] = 0;
    // Min-heap: (distance, vertex)
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;          // stale entry
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

int main() {
    // Graph with 4 vertices:
    //   0 --4--> 1
    //   |       /
    //   1     2
    //   |     |
    //   v    /
    //   2--5-->3
    //   edge: 0->1(4), 0->2(1), 2->1(2), 1->3(1), 2->3(5)
    vector<vector<pair<int,int>>> g(4);
    g[0].push_back({1, 4});
    g[0].push_back({2, 1});
    g[2].push_back({1, 2});
    g[1].push_back({3, 1});
    g[2].push_back({3, 5});

    auto dist = dijkstra(g, 0);
    assert(dist[0] == 0);
    assert(dist[1] == 3);   // 0->2->1 = 1 + 2
    assert(dist[2] == 1);   // 0->2 = 1
    assert(dist[3] == 4);   // 0->2->1->3 = 1 + 2 + 1

    cout << "All tests passed!" << endl;
    return 0;
}
