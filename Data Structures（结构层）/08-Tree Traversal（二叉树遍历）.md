#cpp #tree #traversal

## ⚡ TL;DR（快速决策）

- 需要按“根-左-右”处理 → 前序遍历
- 需要按“左-根-右”处理 → 中序遍历
- 需要按“左-右-根”处理 → 后序遍历
- 需要按层从上到下处理 → 层序遍历（BFS）
- 树遍历题真正关键通常不在“会不会背顺序”，而在“当前节点应该在哪个时机处理”

## 🧩 Core Idea（核心本质）

- **本质**：树遍历是在按某种固定顺序访问整棵树的所有节点
- **关键机制**：DFS 通过递归或栈控制访问顺序，BFS 通过队列按层推进
- **核心区别**：前中后序的差异，本质上是“处理当前节点”的时机不同

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

### 3. 中序遍历

```cpp
void inorder(TreeNode* root) {
	if (root == nullptr) return;
	inorder(root->left);
	cout << root->val << '\\n';
	inorder(root->right);
}
```

### 4. 后序遍历

```cpp
void postorder(TreeNode* root) {
	if (root == nullptr) return;
	postorder(root->left);
	postorder(root->right);
	cout << root->val << '\\n';
}
```

### 5. 层序遍历

```cpp
void level_order(TreeNode* root) {
	if (root == nullptr) return;
	queue<TreeNode*> q;
	q.push(root);
	while (!q.empty()) {
		TreeNode* cur = q.front();
		q.pop();
		cout << cur->val << '\\n';
		if (cur->left) q.push(cur->left);
		if (cur->right) q.push(cur->right);
	}
}
```

### 6. 按层分组遍历

```cpp
vector<vector<int>> level_order(TreeNode* root) {
	vector<vector<int>> ans;
	if (root == nullptr) return ans;
	queue<TreeNode*> q;
	q.push(root);
	while (!q.empty()) {
		int sz = q.size();
		vector<int> level;
		while (sz--) {
			TreeNode* cur = q.front();
			q.pop();
			level.push_back(cur->val);
			if (cur->left) q.push(cur->left);
			if (cur->right) q.push(cur->right);
		}
		ans.push_back(level);
	}
	return ans;
}
```

### 7. 前序非递归

```cpp
void preorder_iter(TreeNode* root) {
	if (root == nullptr) return;
	stack<TreeNode*> st;
	st.push(root);
	while (!st.empty()) {
		TreeNode* cur = st.top();
		st.pop();
		cout << cur->val << '\\n';
		if (cur->right) st.push(cur->right);
		if (cur->left) st.push(cur->left);
	}
}
```

### 8. 中序非递归

```cpp
void inorder_iter(TreeNode* root) {
	stack<TreeNode*> st;
	TreeNode* cur = root;
	while (cur != nullptr || !st.empty()) {
		while (cur != nullptr) {
			st.push(cur);
			cur = cur->left;
		}
		cur = st.top();
		st.pop();
		cout << cur->val << '\\n';
		cur = cur->right;
	}
}
```

## ⚠️ Pitfalls（高频错误）

- 前序、中序、后序真正区别是“访问根节点的时机”，不是死背名字
- 空节点判断最容易漏，几乎所有递归遍历都要先处理 `nullptr`
- 把 DFS 和 BFS 混掉，会导致遍历顺序完全不对
- 层序遍历要用队列，不是栈
- 非递归中序遍历最容易把“入栈走左”和“弹栈转右”顺序写乱
- 不是所有树题都只是打印顺序，很多题是“借遍历顺序做统计或判断”
- BST 的中序遍历有序，但普通二叉树的中序遍历不一定有序
- 当前节点处理位置一旦放错，整题含义就变了

## 🚀 Performance / Tips（性能优化）

- 树遍历题优先先问自己：

```cpp
当前节点应该先处理
中间处理
还是最后处理
```

- 递归遍历最自然，非递归遍历更适合你想精确控制过程时使用
- 层序问题优先想到 `queue`
- 路径、统计、构造类题，往往只是“遍历 + 状态维护”
- 刷题里很多树题本质不是新算法，而是“选对遍历顺序”

## 🧪 Common Scenarios（常见使用场景）

- **前序遍历**：拷贝树、序列化、根优先处理
- **中序遍历**：BST 有序输出、顺序检查
- **后序遍历**：删除树、子树结果汇总、树形 DP 基础
- **层序遍历**：逐层输出、最短层数、层级统计
- **非递归遍历**：模拟调用栈、精细控制访问顺序
- **综合树题**：遍历过程里顺便统计、判断、构造答案

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

void inorder(TreeNode* root) {
	if (root == nullptr) return;
	inorder(root->left);
	cout << root->val << ' ';
	inorder(root->right);
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	TreeNode* root = new TreeNode(2);
	root->left = new TreeNode(1);
	root->right = new TreeNode(3);

	inorder(root);
	cout << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 二叉树遍历的核心不是“背前中后层四种顺序”，而是先判断 **当前节点应该在什么时机处理**，然后用 DFS 或 BFS 把整棵树按这个顺序稳定地走完。