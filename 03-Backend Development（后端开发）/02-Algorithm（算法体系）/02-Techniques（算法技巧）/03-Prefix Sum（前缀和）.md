#data-structure #prefix-sum #range-sum #array #precompute #algorithm #enumeration #simulation #bruteforce

## 核心

-  **预处理数组，快速计算任意区间的元素和**
- sum_arr [ i + 1 ] = sum_arr [ i ] + arr [ i ]
- 什么时候用
	- 多次查询数组任意区间的和
	- 统计、最长 / 最短子数组和
	- 维矩阵的区域和查询
	- 需要快速计算区间和的场景

## 常见题型

### 1. 一维前缀和（基础版）

```cpp
vector<long long> buildPrefixSum(const vector<int>& arr) {
    int n = arr.size();
    vector<long long> s(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        s[i + 1] = s[i] + arr[i];
    }
    return s;
}

long long querySum(const vector<long long>& s, int l, int r) {
    return s[r + 1] - s[l];
}
```

### 2. 二维前缀和（矩阵区域和）

```cpp
vector<vector<long long>> build2DPrefixSum(const vector<vector<int>>& a) {
    int n = a.size() - 1;
    int m = a[0].size() - 1;
    vector<vector<long long>> s(n + 1, vector<long long>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            s[i][j] = a[i][j] + s[i-1][j] + s[i][j-1] - s[i-1][j-1];
        }
    }
    return s;
}

long long query2DSum(const vector<vector<long long>>& s, int x1, int y1, int x2, int y2) {
    return s[x2][y2] - s[x1-1][y2] - s[x2][y1-1] + s[x1-1][y1-1];
}
```

### 3. 哈希表优化（和为 K 的子数组）

```cpp
int subarraySum(vector<int>& nums, int k) {
    unordered_map<long long, int> cnt;
    cnt[0] = 1;
    long long s = 0;
    int ans = 0;
    for (int x : nums) {
        s += x;
        if (cnt.count(s - k)) ans += cnt[s - k];
        cnt[s]++;
    }
    return ans;
}
```
