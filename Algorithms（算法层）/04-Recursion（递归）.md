#algorithm #recursion #dfs

## ⚡ TL;DR（快速决策）

- 问题能拆成更小的同类子问题 → 先想递归
- 数据结构天然自相似，如树、分治、回溯 → 递归高频
- 写递归前先确定：函数含义、递归出口、子问题如何缩小
- 递归题真正关键通常不在“会不会自己调用自己”，而在“每层到底表示什么”
- 一旦子问题没有收敛到出口，递归就会失控

## 🧩 Core Idea（核心本质）

- **本质**：递归是函数通过调用自己来解决更小规模的同类问题
- **关键机制**：先定义当前函数表示什么，再把当前问题拆成更小的子问题，直到命中出口
- **核心价值**：能把树、分治、搜索、数学定义类问题写得更自然、更接近问题结构本身

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础递归骨架

```cpp
int solve(int n) {
	if (出口条件) return 基础答案;
	return solve(更小问题);
}
```

### 2. 阶乘

```cpp
int fact(int n) {
	if (n == 0) return 1;
	return n * fact(n - 1);
}
```

### 3. 递归遍历数组先有印象

```cpp
void print_array(int a[], int n, int i) {
	if (i == n) return;
	cout << a[i] << '\n';
	print_array(a, n, i + 1);
}
```

### 4. 二叉树递归

```cpp
int max_depth(TreeNode* root) {
	if (root == nullptr) return 0;
	return max(max_depth(root->left), max_depth(root->right)) + 1;
}
```

### 5. 分治骨架

```cpp
int solve(int l, int r) {
	if (l == r) return base_value;
	int mid = (l + r) / 2;
	int left = solve(l, mid);
	int right = solve(mid + 1, r);
	return merge(left, right);
}
```

### 6. 回溯型递归骨架

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

### 7. 记忆化递归先有印象

```cpp
int dfs(int x) {
	if (memo[x] != -1) return memo[x];
	if (出口条件) return 基础答案;
	return memo[x] = 由更小问题得到的结果;
}
```

### 8. 递归函数定义优先写清

```cpp
// dfs(u) 表示什么？
// solve(l, r) 表示什么？
// f(n) 表示什么？
```

## ⚠️ Pitfalls（高频错误）

- 递归出口漏掉，直接无限调用
- 子问题没有变小，递归不会收敛
- 函数含义没定义清楚，导致递归体越写越乱
- 返回值递归里漏写 `return`
- 把“当前层该做什么”和“子问题返回什么”混在一起
- 递归顺序不同，结果可能完全不同
- 递归不一定高效，重复子问题多时要考虑记忆化或 DP
- 调试递归时只盯某一层，容易看不清整体调用关系

## 🚀 Performance / Tips（性能优化）

- 写递归前优先先想清：

```cpp
函数表示什么
出口是什么
子问题如何缩小
```

- 树题、分治题、回溯题，通常优先先想递归表达
- 递归真正高频的价值是：

```cpp
表达树结构
表达分治过程
表达搜索过程
```

- 重复子问题明显时，优先考虑记忆化搜索
- 如果递归过程总是写乱，先把调用树画出来再落代码

## 🧪 Common Scenarios（常见使用场景）

- **树问题**：遍历、统计、判断结构
- **分治问题**：归并排序、快速排序、区间拆分
- **回溯问题**：排列、组合、子集、路径搜索
- **数学定义问题**：阶乘、斐波那契、汉诺塔
- **记忆化搜索**：自顶向下 DP
- **DFS**：图和树的深度优先搜索

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int fact(int n) {
	if (n == 0) return 1;
	return n * fact(n - 1);
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	cout << fact(5) << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 递归的核心不是“函数调用自己”，而是先把问题定义成 **更小规模的同类子问题**，再用清晰的函数含义和递归出口把整个求解过程稳定地串起来。