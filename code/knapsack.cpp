// Template: Knapsack — 0-1 Knapsack and Complete Knapsack
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;

// 0-1 背包：逆序遍历容量
int knapsack01(const vector<int>& w, const vector<int>& v, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < (int)w.size(); i++)
        for (int j = cap; j >= w[i]; j--)     // 逆序保证每个物品最多选一次
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    return dp[cap];
}

// 完全背包：正序遍历容量
int knapsackComplete(const vector<int>& w, const vector<int>& v, int cap) {
    vector<int> dp(cap + 1, 0);
    for (int i = 0; i < (int)w.size(); i++)
        for (int j = w[i]; j <= cap; j++)     // 正序允许重复选取
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    return dp[cap];
}

int main() {
    // Items: (weight, value)
    vector<int> w = {2, 3, 4, 5};
    vector<int> v = {3, 4, 5, 6};

    // 0-1 Knapsack, capacity = 5
    // Best: item 0 (w=2,v=3) + item 1 (w=3,v=4) = total value 7
    assert(knapsack01(w, v, 5) == 7);

    // Complete Knapsack, capacity = 5
    // Best: item 0 (w=2,v=3) once + item 1 (w=3,v=4) once = total value 7
    assert(knapsackComplete(w, v, 5) == 7);

    // Simpler case to verify the difference between 0-1 and complete
    vector<int> w2 = {1, 2};
    vector<int> v2 = {2, 3};

    // 0-1, cap=3: best is item 0 + item 1 = 2 + 3 = 5
    assert(knapsack01(w2, v2, 3) == 5);

    // Complete, cap=3: best is item 0 three times = 2 * 3 = 6
    assert(knapsackComplete(w2, v2, 3) == 6);

    cout << "All tests passed!" << endl;
    return 0;
}
