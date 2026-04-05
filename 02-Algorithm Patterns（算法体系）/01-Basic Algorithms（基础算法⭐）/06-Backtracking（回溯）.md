#algorithm #backtracking #dfs #search

## 核心

- 回溯 = **带状态撤回的递归**
- 常见于：
    - 全排列
    - 组合
    - N 皇后
    - 子集
    - 路径搜索

## 经典题型：
### 1. 全排列（入门）

```cpp
vector<vector<int>> ans;
vector<int> path;
vector<bool> used;

void dfs(vector<int>& nums) {
	// 路径长度等于数组长度，终止
    if (path.size() == nums.size()) {
        ans.push_back(path);
        return;
    }
    // 遍历所有未选择的元素
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (!used[i]) {
	        // 做选择：把元素加入路径，标记已用
	        used[i] = ture;
	        path.push_back(nums[i]);
	        // 递归
	        dfs(nums);
	        // 撤销选择（回溯）：恢复状态
	        path.pop_back();
	        used[i] = false;
	    }
    }
}
```
**（LeetCode 46）**

### 2. 组合总和（回溯经典）

```cpp
void backtrack(vector<int>& candidates, int target, int start) {
    // 和等于目标，加入结果，终止
	if (target == 0) {
        res.push_back(path);
        return;
    }
    // 剪枝：和超过目标，直接返回
    if (target < 0) return;
    // 从start开始，避免重复组合
    for (int i = start; i < candidates.size(); ++i) {
	    // 做选择
        path.push_back(candidates[i]);
        // 递归（i不变，因为可以重复选）
        backtrack(candidates, target - candidates[i], i);
        // 撤销选择
        path.pop_back();
    }
}
```
**（LeetCode 39）**

### 3. N 皇后问题

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

### 4. 子集

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

## 高频坑点

- 忘记撤销选择
- 终止条件写错
- 去重逻辑混乱
- path / used 作用域不清楚