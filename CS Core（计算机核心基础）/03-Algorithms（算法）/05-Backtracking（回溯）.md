#algorithm #backtracking #dfs #search #state-space

## ⚡ TL;DR（快速决策）

- 回溯本质是：**在搜索过程中不断尝试、撤销选择，并遍历所有可能解**
- 一看到这些特征，要优先想到回溯：
    - 全排列、组合、子集
    - N 皇后、数独、括号生成
    - 需要“试一个方案，不行就撤回”
- 回溯通常可以理解成：
    - DFS + 状态修改 + 撤销恢复
- 它的核心不是暴力，而是：**有组织地枚举搜索树**

## 🧩 Core Idea（核心本质）

- 回溯会把问题看成一棵“决策树”
- 每层代表一个选择
- 走下去叫“做选择”
- 回来时叫“撤销选择”
- 一句话理解：
    - **试、走、撤、再试。**
- 一个标准回溯问题通常要想清楚：
    - 当前路径是什么
    - 还剩哪些选择
    - 什么时候结束

## 🔧 Usage Patterns（可复用代码模板）

1. 全排列模板

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<int> path;
vector<bool> used;

void dfs(vector<int>& nums) {
    if ((int)path.size() == (int)nums.size()) {
        for (int x : path) cout << x << ' ';
        cout << '\\n';
        return;
    }
    for (int i = 0; i < (int)nums.size(); ++i) {
        if (used[i]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        dfs(nums);
        path.pop_back();
        used[i] = false;
    }
}
```

1. 组合模板

```cpp
void dfs(int start, int n, int k) {
    if ((int)path.size() == k) {
        // 记录答案
        return;
    }
    for (int i = start; i <= n; ++i) {
        path.push_back(i);
        dfs(i + 1, n, k);
        path.pop_back();
    }
}
```

1. 子集模板

```cpp
void dfs(int idx, vector<int>& nums) {
    // 当前 path 就是一个子集
    for (int x : path) cout << x << ' ';
    cout << '\\n';
    for (int i = idx; i < (int)nums.size(); ++i) {
        path.push_back(nums[i]);
        dfs(i + 1, nums);
        path.pop_back();
    }
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：做了选择却忘记撤销
- 最常见错误 2：终止条件写错
- 最常见错误 3：去重逻辑混乱，导致重复答案
- 最常见错误 4：路径、used、start 等状态定义不清
- 最常见错误 5：没剪枝导致效率太差

## 🚀 Performance / Tips（性能优化）

- 回溯复杂度通常较高，关键在剪枝
- 高频经验：
    - 明确“路径、选择列表、结束条件”
    - 先写出最朴素搜索，再加剪枝
    - 搜索树很大时，剪枝比代码优化更重要
- 常见剪枝方式：
    - 提前判不合法
    - 排序后剪重复
    - 剩余数量不足时直接返回

## 🧪 Common Scenarios（常见使用场景）

- 全排列
- 组合 / 子集
- N 皇后
- 数独
- 字符串划分与搜索

## 🆚 Backtracking vs Recursion

- 回溯一定是递归高频应用之一
- 但递归不一定是回溯
- 回溯强调：
    - **尝试选择 + 撤销状态 + 搜索整棵决策树**

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;
vector<int> path;
void dfs(int start, int n) {
    for (int x : path) cout << x << ' ';
    cout << '\\n';
    for (int i = start; i <= n; ++i) {
        path.push_back(i);
        dfs(i + 1, n);
        path.pop_back();
    }
}
int main() {
    dfs(1, 3);
}
```

## 📌 One-liner Summary（一句话总结）

- **回溯就是：在搜索树上不断尝试选择、递归深入、再撤销恢复的枚举方法。**