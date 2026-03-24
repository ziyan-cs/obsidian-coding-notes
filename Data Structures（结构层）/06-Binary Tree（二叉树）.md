#cpp #data-structure #binary-tree

## ⚡ TL;DR（快速决策）

- 数据天然有左右分叉、层级关系 → 二叉树
- 题目出现“左右子树、根节点、叶子节点” → 先切到树的视角
- 需要遍历、统计、判断结构 → 二叉树递归通常是第一选择
- 需要按大小快速定位 → 先确认是不是 BST，不要把普通二叉树和二叉搜索树混掉
- 树题大多数难点不在定义，而在“当前节点做什么 + 左右子树怎么递归”

## 🧩 Core Idea（核心本质）

- **本质**：二叉树是每个节点最多只有两个孩子的树结构
- **关键机制**：每个节点把问题继续拆成左子树和右子树，因此天然适合递归处理
- **核心优势**：结构清晰，遍历、分治、递归建模都很自然，是大量树题的基础载体

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础节点定义

```cpp
struct TreeNode {
	int val;
	TreeNode* left;
	TreeNode* right;

	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
```

### 2. 前序遍历

```cpp
void preorder(TreeNode* root) {
	if (root == nullptr) return;
	cout << root->val << '\\n';
	preorder(root->left);
	preorder(root->right);
}
```

### 3. 统计节点数

```cpp
int count_nodes(TreeNode* root) {
	if (root == nullptr) return 0;
	return 1 + count_nodes(root->left) + count_nodes(root->right);
}
```

### 4. 求树高

```cpp
int max_depth(TreeNode* root) {
	if (root == nullptr) return 0;
	return max(max_depth(root->left), max_depth(root->right)) + 1;
}
```

### 5. 判断两棵树是否相同

```cpp
bool is_same(TreeNode* a, TreeNode* b) {
	if (a == nullptr && b == nullptr) return true;
	if (a == nullptr || b == nullptr) return false;
	if (a->val != b->val) return false;
	return is_same(a->left, b->left) && is_same(a->right, b->right);
}
```

### 6. 层序遍历

```cpp
void level_order(TreeNode* root) {
	if (root == nullptr) return;
	queue<TreeNode*> q;
	q.push(root);
	while (!q.empty()) {
		TreeNode* cur = q.front();
		q.pop();
		if (cur->left) q.push(cur->left);
		if (cur->right) q.push(cur->right);
	}
}
```

### 7. 递归函数的常见骨架

```cpp
int solve(TreeNode* root) {
	if (root == nullptr) return 0;
	int left = solve(root->left);
	int right = solve(root->right);
	return 由 left、right 和 root 组合出的答案;
}
```

### 8. DFS 非递归先有印象

```cpp
stack<TreeNode*> st;
if (root) st.push(root);
while (!st.empty()) {
	TreeNode* cur = st.top();
	st.pop();
	if (cur->right) st.push(cur->right);
	if (cur->left) st.push(cur->left);
}
```

## ⚠️ Pitfalls（高频错误）

- 普通二叉树和二叉搜索树不是一回事，不要看到树就默认“左小右大”
- 空节点判断最容易漏，几乎所有递归都要先处理 `nullptr`
- 树的高度、节点深度、节点数是三种不同概念，不要混
- 中序遍历有序这个性质只对 BST 成立，不对普通二叉树成立
- 递归函数含义不清时，代码会越写越乱
- 左右子树递归顺序写反，结果可能完全变掉
- 层序遍历是 BFS，用队列，不是栈
- 树题很多 bug 本质是边界没想清楚，不是算法不会

## 🚀 Performance / Tips（性能优化）

- 树题优先先想：

```cpp
当前节点做什么
左子树返回什么
右子树返回什么
```

- 写递归前，先把函数含义说清楚，比直接写代码更重要
- 统计型、判断型、路径型问题，通常都能统一成“左右子树 + 当前节点”的组合模型
- 层序问题优先想到队列，递归问题优先想到 DFS
- 刷题时，树的真正主线通常是：遍历方式 + 递归定义 + 边界处理

## 🧪 Common Scenarios（常见使用场景）

- **遍历整棵树**：前序 / 中序 / 后序 / 层序
- **统计信息**：节点数、高度、叶子数
- **判断结构**：是否相同、是否对称、是否平衡
- **路径问题**：根到叶路径、路径和
- **树形递归**：把问题拆到左右子树处理
- **BST 相关题前置结构**：二叉树基础是前提

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

struct TreeNode {
	int val;
	TreeNode* left;
	TreeNode* right;

	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

int max_depth(TreeNode* root) {
	if (root == nullptr) return 0;
	return max(max_depth(root->left), max_depth(root->right)) + 1;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	TreeNode* root = new TreeNode(1);
	root->left = new TreeNode(2);
	root->right = new TreeNode(3);

	cout << max_depth(root) << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 二叉树的核心不是“有两个孩子”，而是理解它把问题天然拆成 **当前节点 + 左右子树**，所以大多数树题本质都是在定义递归含义和遍历方式。