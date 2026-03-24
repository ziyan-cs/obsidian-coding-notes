#algorithm #backtracking #dfs

## ⚡ TL;DR（快速决策）

- 题目要求“所有方案 / 所有排列 / 所有组合” → 先想回溯
- 需要在搜索过程中试一个、退回来、再试下一个 → 典型回溯
- 回溯本质上常写成递归，但关键不只是递归，而是“做选择 + 撤销选择”
- 结果不对时，优先检查：结束条件、状态恢复、去重逻辑
- 回溯题真正关键通常不在“会不会搜”，而在“当前路径表示什么、下一层还能选什么”

## 🧩 Core Idea（核心本质）

- **本质**：回溯是在搜索过程中不断尝试选择，并在不合适时撤销选择回到上一步
- **关键机制**：通过递归枚举决策树，在每一层做选择、递归深入、再恢复现场
- **核心价值**：特别适合处理全排列、组合、子集、路径搜索、棋盘放置等“枚举所有可行解”问题

## 🔧 Usage Patterns（可复用代码模板）

### 1. 通用回溯骨架

```cpp
void dfs(...) {
	if (结束条件) {
		// 收集答案
		return;
	}
	for (每个选择) {
		做选择;
		dfs(...);
		撤销选择;
	}
}
```

### 2. 全排列

```cpp
vector<vector<int>> ans;
vector<int> path;
bool used[100] = {false};

void dfs(vector<int>& nums) {
	if (path.size() == nums.size()) {
		ans.push_back(path);
		return;
	}
	for (int i = 0; i < nums.size(); ++i) {
		if (used[i]) continue;
		used[i] = true;
		path.push_back(nums[i]);
		dfs(nums);
		path.pop_back();
		used[i] = false;
	}
}
```

### 3. 组合

```cpp
vector<vector<int>> ans;
vector<int> path;

void dfs(int n, int k, int start) {
	if (path.size() == k) {
		ans.push_back(path);
		return;
	}
	for (int i = start; i <= n; ++i) {
		path.push_back(i);
		dfs(n, k, i + 1);
		path.pop_back();
	}
}
```

### 4. 子集

```cpp
vector<vector<int>> ans;
vector<int> path;

void dfs(vector<int>& nums, int start) {
	ans.push_back(path);
	for (int i = start; i < nums.size(); ++i) {
		path.push_back(nums[i]);
		dfs(nums, i + 1);
		path.pop_back();
	}
}
```

### 5. 字符串路径构造先有印象

```cpp
string path;
void dfs(...) {
	if (结束条件) {
		ans.push_back(path);
		return;
	}
	for (每个选择) {
		path.push_back(ch);
		dfs(...);
		path.pop_back();
	}
}
```

### 6. 棋盘搜索先有印象

```cpp
void dfs(int row) {
	if (row == n) {
		// 收集答案
		return;
	}
	for (int col = 0; col < n; ++col) {
		if (当前位置不能放) continue;
		做选择;
		dfs(row + 1);
		撤销选择;
	}
}
```

### 7. 去重回溯先有印象

```cpp
sort(nums.begin(), nums.end());
for (int i = start; i < nums.size(); ++i) {
	if (i > start && nums[i] == nums[i - 1]) continue;
	path.push_back(nums[i]);
	dfs(nums, i + 1);
	path.pop_back();
}
```

### 8. 剪枝先有印象

```cpp
for (int i = start; i <= n; ++i) {
	if (剩余元素数量不够) break;
	path.push_back(i);
	dfs(n, k, i + 1);
	path.pop_back();
}
```

## ⚠️ Pitfalls（高频错误）

- 做了选择却忘记撤销，状态会被污染
- 结束条件写错，会导致漏解、重复解或死递归
- `path` 和 `ans` 的角色没分清，最容易写乱
- 排列、组合、子集三类题的搜索模型不同，别混模板
- `used[]` 和 `start` 的语义不同，不要混着用
- 去重逻辑最容易写错，尤其是“同层去重”和“不同层可重复”
- 回溯不是暴力乱搜，关键在于“状态定义 + 合法剪枝”
- 剪枝条件写错会直接错答案，不只是影响性能

## 🚀 Performance / Tips（性能优化）

- 写回溯前优先先想清：

```cpp
当前路径表示什么
这一层有哪些选择
什么时候收集答案
返回时要恢复什么
```

- 回溯真正高频的价值是：

```cpp
枚举所有方案
搜索可行路径
构造满足条件的解
```

- 能剪枝就尽早剪枝，回溯的核心成本通常在无效搜索
- 排列题常优先想 `used[]`，组合 / 子集题常优先想 `start`
- 如果回溯过程总写乱，先画决策树再落代码

## 🧪 Common Scenarios（常见使用场景）

- **全排列**：每个位置选一个还没用过的元素
- **组合问题**：从一组元素中选 k 个
- **子集问题**：所有选择 / 不选择的组合
- **路径搜索**：迷宫、单词搜索、网格路径
- **棋盘放置**：N 皇后、数独
- **字符串构造**：括号生成、IP 地址还原

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> ans;
vector<int> path;
bool used[10] = {false};

void dfs(vector<int>& nums) {
	if (path.size() == nums.size()) {
		ans.push_back(path);
		return;
	}
	for (int i = 0; i < nums.size(); ++i) {
		if (used[i]) continue;
		used[i] = true;
		path.push_back(nums[i]);
		dfs(nums);
		path.pop_back();
		used[i] = false;
	}
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {1, 2, 3};
	dfs(nums);
	cout << ans.size() << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 回溯的核心不是“递归枚举”，而是先把问题定义成 **路径 + 选择 + 结束条件 + 状态恢复**，再通过做选择和撤销选择把整个搜索过程稳定地跑完。