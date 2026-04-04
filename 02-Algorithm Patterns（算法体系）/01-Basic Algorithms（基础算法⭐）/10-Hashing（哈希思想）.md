#algorithm #hash #unordered_map #unordered_set

## 核心

- 本质是：用空间换时间，快速判断某元素是否出现过 / 某信息是否存在
- 高频关键词：
    - 去重
    - 计数
    - 映射
    - 前缀和 + 哈希

## 什么时候用

- 需要快速查存在性
- 需要统计频次
- 需要记录某值第一次出现的位置

## 高频模板

### 去重

```cpp
unordered_set<int> st;
for (int x : nums) st.insert(x);
```

### 计数

```cpp
unordered_map<int, int> cnt;
for (int x : nums) cnt[x]++;
```

### 两数之和

```cpp
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> mp;
    for (int i = 0; i < (int)nums.size(); ++i) {
        int need = target - nums[i];
        if (mp.count(need)) return {mp[need], i};
        mp[nums[i]] = i;
    }
    return {};
}
```

### 最长连续序列

```cpp
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> st(nums.begin(), nums.end());
    int ans = 0;
    for (int x : st) {
        if (st.count(x - 1)) continue;
        int y = x;
        while (st.count(y)) ++y;
        ans = max(ans, y - x);
    }
    return ans;
}
```

## 高频坑点

- 该用 `unordered_map` 却用了 `map` 导致慢
- 只查存在性却写成 `mp[key]`，意外插入
- 忘记哈希题常和前缀和、字符串计数结合