#cpp #data-structure #binary-search-tree

## ⚡ TL;DR（快速决策）

- 需要在树结构上按大小快速查找 → 二叉搜索树（BST）
- 题目出现“左小右大”“中序有序” → 先切到 BST 视角
- 查询、插入、删除目标值 → 优先想利用 BST 性质，不要按普通二叉树全搜
- 先确认题目是不是 BST，不要把普通二叉树和 BST 混掉
- BST 题真正关键通常不在树结构本身，而在“如何利用有序性剪枝”

## 🧩 Core Idea（核心本质）

- **本质**：二叉搜索树是一种带有大小约束的二叉树
- **关键机制**：对任意节点，都满足左子树节点值小于当前节点值，右子树节点值大于当前节点值
- **核心优势**：可以把查找范围从整棵树缩到一条路径，天然适合搜索、插入、删除和范围判断

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

### 2. BST 查找目标值

```cpp
TreeNode* search_bst(TreeNode* root, int target) {
	while (root != nullptr) {
		if (root->val == target) return root;
		if (target < root->val) root = root->left;
		else root = root->right;
	}
	return nullptr;
}
```

### 3. BST 插入节点

```cpp
TreeNode* insert_bst(TreeNode* root, int val) {
	if (root == nullptr) return new TreeNode(val);
	if (val < root->val) root->left = insert_bst(root->left, val);
	else if (val > root->val) root->right = insert_bst(root->right, val);
	return root;
}
```

### 4. 验证是否为 BST

```cpp
bool is_valid_bst(TreeNode* root, long long low, long long high) {
	if (root == nullptr) return true;
	if (root->val <= low || root->val >= high) return false;
	return is_valid_bst(root->left, low, root->val) &&
		is_valid_bst(root->right, root->val, high);
}
```

### 5. 中序遍历得到有序序列

```cpp
void inorder(TreeNode* root, vector<int>& nums) {
	if (root == nullptr) return;
	inorder(root->left, nums);
	nums.push_back(root->val);
	inorder(root->right, nums);
}
```

### 6. 找最小值节点

```cpp
TreeNode* get_min(TreeNode* root) {
	while (root != nullptr && root->left != nullptr) {
		root = root->left;
	}
	return root;
}
```

### 7. 删除节点先有印象

```cpp
TreeNode* delete_bst(TreeNode* root, int key) {
	if (root == nullptr) return nullptr;
	if (key < root->val) root->left = delete_bst(root->left, key);
	else if (key > root->val) root->right = delete_bst(root->right, key);
	else {
		if (root->left == nullptr) return root->right;
		if (root->right == nullptr) return root->left;
		TreeNode* mn = get_min(root->right);
		root->val = mn->val;
		root->right = delete_bst(root->right, mn->val);
	}
	return root;
}
```

### 8. 第 k 小先有印象

```cpp
// 中序遍历天然从小到大
```

## ⚠️ Pitfalls（高频错误）

- 普通二叉树不是 BST，不要看到树就默认“左小右大”
- BST 性质要求的是“整个左子树都小、整个右子树都大”，不是只和直接孩子比较
- 验证 BST 时只检查 `left->val < root->val < right->val` 是不够的
- 中序遍历有序只对 BST 成立，不对普通二叉树成立
- 删除节点是 BST 最容易写乱的操作，尤其是有两个孩子时
- 查找 / 插入 / 删除时，递归返回的新子树根不要忘记接回去
- 是否允许重复值必须先看题意，不同题目定义可能不同
- 用 `int` 做边界验证可能不够稳，验证 BST 常用更大范围类型

## 🚀 Performance / Tips（性能优化）

- BST 题优先先想：

```cpp
能不能利用左小右大直接剪枝
```

- 查询、插入、删除很多时候都只走一条路径，不要按普通树全量遍历
- 验证 BST 时，优先用“值域约束”写法，比只看局部关系更稳
- 第 k 小、前驱后继、范围查询这类题，常常和中序遍历强相关
- 刷题时要先区分：

```cpp
这是普通二叉树题
还是 BST 题
```

## 🧪 Common Scenarios（常见使用场景）

- **查找某个值**：利用大小关系向左或向右走
- **插入新节点**：沿搜索路径插入叶子位置
- **删除节点**：分类讨论 0、1、2 个孩子
- **验证 BST**：递归维护合法值域
- **第 k 小 / 有序输出**：中序遍历
- **前驱后继 / 范围查询**：利用 BST 有序性剪枝

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

TreeNode* search_bst(TreeNode* root, int target) {
	while (root != nullptr) {
		if (root->val == target) return root;
		if (target < root->val) root = root->left;
		else root = root->right;
	}
	return nullptr;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	TreeNode* root = new TreeNode(4);
	root->left = new TreeNode(2);
	root->right = new TreeNode(6);

	cout << (search_bst(root, 2) != nullptr) << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 二叉搜索树的核心不是“它也是二叉树”，而是它额外提供了 **左小右大** 的有序性，所以很多题的关键都在于能否利用这条性质把搜索从整棵树缩成一条路径。