#algorithm #greedy #interval #sorting

## 核心

- 贪心 = **每一步都做当前看起来最优的选择**
- 前提不是“感觉对”，而是要有正确性依据
- 常见关键词：
    - 区间调度
    - 最少 / 最多
    - 局部最优推出全局最优

## 常见套路

### 1. 排序后贪心

```cpp
sort(a.begin(), a.end());
for (auto& x : a) {
    if (满足条件) 选它;
}
```

### 2. 区间问题

- 按右端点排序最常见

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
        return a[1] < b[1];
    });
    int ans = 0, end = INT_MIN;
    for (auto& it : intervals) {
        if (it[0] >= end) {
            ++ans;
            end = it[1];
        }
    }
    return ans;
}
```

### 3. 零钱 / 分配类

- 先拿大的、先满足最紧的约束，常见但不一定都能贪心

## 经典例子

### 分发饼干

```cpp
int findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(), g.end());
    sort(s.begin(), s.end());
    int i = 0, j = 0;
    while (i < (int)g.size() && j < (int)s.size()) {
        if (s[j] >= g[i]) ++i;
        ++j;
    }
    return i;
}
```

## 高频坑点

- 没证明正确性就强行贪心
- 排序关键字选错
- 题目其实应该用 DP，却误判成贪心