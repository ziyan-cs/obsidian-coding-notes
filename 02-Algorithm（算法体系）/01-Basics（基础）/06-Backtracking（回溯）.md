#algorithm #backtracking #dfs #search

# 0. 核心

- 回溯 = **带状态撤回的递归**
- 常见于：
    - 全排列
    - 组合
    - N 皇后
    - 子集
    - 路径搜索

# 1. 经典案例

### 1.1 全排列（入门）

```cpp
vector<vector<int>> ans;
vector<int> path;
vector<bool> used;

void dfs(vector<int>& nums) {
    if (path.size() == nums.size()) {
        ans.push_back(path);
        return;
    }
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (!used[i]) {
	        used[i] = ture;
	        path.push_back(nums[i]);
	        dfs(nums);
	        path.pop_back();
	        used[i] = false;
	    }
    }
}
```
**（LeetCode 46）**

### 1.2 组合总和（回溯经典）

```cpp
void backtrack(vector<int>& candidates, int target, int start) {
	if (target == 0) {
        res.push_back(path);
        return;
    }
    if (target < 0) return;
    for (int i = start; i < candidates.size(); ++i) {
        path.push_back(candidates[i]);
        backtrack(candidates, target - candidates[i], i);
        path.pop_back();
    }
}
```
**（LeetCode 39）**

### 1.3 N 皇后问题

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
**（LeetCode 51）**

### 1.4 子集

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
**（LeetCode78 ）**