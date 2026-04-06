#algorithm #sliding-window #two-pointers #substring

## 核心

- **维护一个连续区间，并在移动过程中动态更新答案**
- 是“双指针”的连续区间版
- 高频关键词：
    - 最长 / 最短子串
    - 连续子数组
    - 至多 / 至少 / 恰好
    - 频次统计

## 适用前提

- 连续区间
- 左边界右移后，区间状态可以增量维护
- 不想每次重新统计整个子数组

## 标准模板

```cpp
int l = 0;
for (int r = 0; r < n; ++r) {
    // 1. 扩张窗口，加入 a[r]

    while (窗口不合法) {
        // 2. 收缩窗口，移出 a[l]
        ++l;
    }

    // 3. 用当前合法窗口更新答案
}
```

## 常见题型

### 1. 最长不重复子串

```cpp
int lengthOfLongestSubstring(string s) {
    vector<int> cnt(256, 0);
    int ans = 0, l = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        cnt[s[r]]++;
        while (cnt[s[r]] > 1) {
            cnt[s[l]]--;
            ++l;
        }
        ans = max(ans, r - l + 1);
    }
    return ans;
}
```

### 2. 长度最小的子数组和

```cpp
int minSubArrayLen(int target, vector<int>& nums) {
    int l = 0, sum = 0, ans = INT_MAX;
    for (int r = 0; r < (int)nums.size(); ++r) {
        sum += nums[r];
        while (sum >= target) {
            ans = min(ans, r - l + 1);
            sum -= nums[l++];
        }
    }
    return ans == INT_MAX ? 0 : ans;
}
```

### 3. 固定长度窗口

```cpp
int maxSumK(vector<int>& a, int k) {
    int sum = 0;
    for (int i = 0; i < k; ++i) sum += a[i];
    int ans = sum;
    for (int r = k; r < (int)a.size(); ++r) {
        sum += a[r] - a[r - k];
        ans = max(ans, sum);
    }
    return ans;
}
```

## 高频坑点

- 没搞清楚是连续区间还是子序列
- `while` 收缩条件写错，导致少缩或多缩
- 窗口统计量没有增量更新，退化成暴力
- “恰好 k 个” 常常转成“至多 k 个 - 至多 k-1 个”

## 只记这个

- 连续区间 -> 先想滑动窗口
- 右指针负责扩张，左指针负责收缩
- 窗口内统计必须能 O(1) 或均摊维护