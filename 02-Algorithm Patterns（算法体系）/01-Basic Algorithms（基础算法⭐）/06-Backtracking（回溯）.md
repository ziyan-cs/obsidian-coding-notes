#algorithm #backtracking #dfs #search

## 核心

- 回溯 = 递归搜索 + 做选择 + 撤销选择
- 常见于：
    - 全排列
    - 子集
    - 组合
    - N 皇后
    - 路径搜索

## 通用模板

```cpp
void dfs(...) {
    if (终止条件) {
        记录答案;
        return;
    }
    for (选择 in 选择列表) {
        if (!合法) continue;
        做选择;
        dfs(...);
        撤销选择;
    }
}
```

## 高频例子

### 全排列

```cpp
vector<vector<int>> ans;
vector<int> path;
vector<int> used;

void dfs(vector<int>& nums) {
    if (path.size() == nums.size()) {
        ans.push_back(path);
        return;
    }
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (used[i]) continue;
        used[i] = 1;
        path.push_back(nums[i]);
        dfs(nums);
        path.pop_back();
        used[i] = 0;
    }
}
```

### 子集

```cpp
void dfs(int idx, vector<int>& nums, vector<int>& path, vector<vector<int>>& ans) {
    ans.push_back(path);
    for (int i = idx; i < (int)nums.size(); ++i) {
        path.push_back(nums[i]);
        dfs(i + 1, nums, path, ans);
        path.pop_back();
    }
}
```

## 高频坑点

- 忘记撤销选择
- 终止条件写错
- 去重逻辑混乱
- path / used 作用域不清楚