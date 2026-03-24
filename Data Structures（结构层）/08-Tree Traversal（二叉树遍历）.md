#数据结构 #二叉树 #树 #二分 #查找
## 一页速览

- **二叉树遍历**：按一定顺序访问二叉树所有节点
- **深度优先遍历**：前序、中序、后序
- **广度优先遍历**：层序遍历
- **核心区别**：根节点在什么时候被访问
- **常见写法**：递归、栈模拟、队列层序
- **高频场景**：输出节点顺序、求树高、判断结构、序列化基础
- **最常错**：前中后序顺序记混、递归出口漏写、层序遍历错用栈、左右顺序写反

## 一、先总后分：这一节到底学什么

### 1. 二叉树遍历解决什么问题

- 想把树里的每个节点都访问一遍
- 想按不同顺序处理节点信息
- 这时就要用遍历

### 2. 二叉树遍历最核心看什么

- 前序、中序、后序的访问顺序
- 层序遍历为什么要用队列
- 递归写法为什么自然
- 不同遍历适合解决什么题

### 3. 当前阶段先抓主线

- 会分清 4 种常见遍历
- 会写递归版前中后序
- 会理解层序遍历的队列思路
- 会根据题目判断该用哪种遍历

---

## 二、二叉树遍历是什么

### 1. 基本理解

- 遍历就是按某种规则访问树中所有节点
- 每个节点通常访问一次

### 2. 两大类

- **深度优先遍历 DFS**：前序、中序、后序
- **广度优先遍历 BFS**：层序遍历

### 3. 当前阶段先记一句话

- **遍历 = 按固定顺序把整棵树走一遍**

---

## 三、前序、中序、后序先整体记住

### 1. 前序遍历

- 顺序：**根 -> 左 -> 右**

### 2. 中序遍历

- 顺序：**左 -> 根 -> 右**

### 3. 后序遍历

- 顺序：**左 -> 右 -> 根**

### 4. 当前阶段速记

- **前序**：根最先
- **中序**：根在中间
- **后序**：根最后

---

## 四、前序遍历

### 1. 基本思路

- 先访问根节点
- 再递归遍历左子树
- 最后递归遍历右子树

### 2. 递归模板

```cpp
void preorder(TreeNode* root) {
	if (root == nullptr) return;
	cout << root->val << '\\n';
	preorder(root->left);
	preorder(root->right);
}
```

### 3. 当前阶段怎么理解

- 根节点最早被处理
- 常适合做“复制树结构”“先处理当前节点”的题

---

## 五、中序遍历

### 1. 基本思路

- 先递归遍历左子树
- 再访问根节点
- 最后递归遍历右子树

### 2. 递归模板

```cpp
void inorder(TreeNode* root) {
	if (root == nullptr) return;
	inorder(root->left);
	cout << root->val << '\\n';
	inorder(root->right);
}
```

### 3. 当前阶段怎么理解

- 根节点在中间被访问
- 在二叉搜索树里，中序遍历结果有序

### 4. 当前阶段先记

- **看到 BST，优先想到中序有序**

---

## 六、后序遍历

### 1. 基本思路

- 先递归遍历左子树
- 再递归遍历右子树
- 最后访问根节点

### 2. 递归模板

```cpp
void postorder(TreeNode* root) {
	if (root == nullptr) return;
	postorder(root->left);
	postorder(root->right);
	cout << root->val << '\\n';
}
```

### 3. 当前阶段怎么理解

- 根节点最后处理
- 常适合做“先处理孩子，再处理父节点”的题

---

## 七、用一个例子统一看 3 种 DFS

假设二叉树：

```cpp
  1
 / \\
2   3
```

### 1. 前序遍历结果

```cpp
1 2 3
```

### 2. 中序遍历结果

```cpp
2 1 3
```

### 3. 后序遍历结果

```cpp
2 3 1
```

### 4. 当前阶段怎么记忆

- 看根节点 `1` 出现在最前、中间、最后的位置

---

## 八、为什么递归写法这么自然

### 1. 因为树本身就是递归结构

- 一个节点下面还是左子树和右子树
- 子树本身又是树

### 2. 递归模板为什么几乎一样

- 先写终止条件：`root == nullptr`
- 再决定“根节点在什么位置访问”

### 3. 当前阶段先记

- **前中后序真正的区别，本质就是根节点访问时机不同**

---

## 九、层序遍历

### 1. 基本思路

- 一层一层访问节点
- 从上到下，从左到右

### 2. 为什么要用队列

- 先进入队列的节点先处理
- 这正符合一层一层推进的顺序

### 3. 基础模板

```cpp
void levelOrder(TreeNode* root) {
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

### 4. 当前阶段怎么理解

- 每次从队头拿一个节点
- 再把它的左右孩子放到队尾

---

## 十、4 种遍历怎么区分

|遍历|顺序|核心特征|高频印象|
|---|---|---|---|
|前序|根 左 右|根最先访问|先处理当前节点|
|中序|左 根 右|根在中间|BST 中结果有序|
|后序|左 右 根|根最后访问|先处理孩子|
|层序|按层访问|用队列|BFS / 一层一层|

---

## 十一、当前阶段最常见写法

### 1. 前序模板

```cpp
void preorder(TreeNode* root) {
	if (root == nullptr) return;
	cout << root->val << '\\n';
	preorder(root->left);
	preorder(root->right);
}
```

### 2. 中序模板

```cpp
void inorder(TreeNode* root) {
	if (root == nullptr) return;
	inorder(root->left);
	cout << root->val << '\\n';
	inorder(root->right);
}
```

### 3. 后序模板

```cpp
void postorder(TreeNode* root) {
	if (root == nullptr) return;
	postorder(root->left);
	postorder(root->right);
	cout << root->val << '\\n';
}
```

### 4. 层序模板

```cpp
void levelOrder(TreeNode* root) {
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

---

## 十二、现代 C++ 的基础习惯

### 1. 先把前中后序顺序背熟

- 很多题不是代码难
- 而是顺序记混了

### 2. 递归先写出口

- `if (root == nullptr) return;`
- 这是最常见的第一步

### 3. 看到这些关键词先想到对应遍历

- 根最先处理：前序
- BST 有序：中序
- 先算左右子树：后序
- 一层一层：层序

### 4. 以后会继续补

- 非递归遍历
- Morris 遍历
- 层序分层写法
- 但当前阶段先把递归版学稳

---

## 十三、高频易错点

1. 前序、中序、后序最容易记混
2. 根节点访问位置决定遍历类型
3. 递归出口 `root == nullptr` 很容易漏
4. 层序遍历要用队列，不是栈
5. 中序遍历有序这个性质只对 BST 成立
6. 左右子树访问顺序不要写反
7. 后序遍历常常是最容易在脑子里绕乱的
8. 不同题目选遍历方式时，先看“根节点什么时候该处理”

---

## 十四、最简模板

### 前序模板

```cpp
void preorder(TreeNode* root) {
	if (root == nullptr) return;
	cout << root->val << '\\n';
	preorder(root->left);
	preorder(root->right);
}
```

### 中序模板

```cpp
void inorder(TreeNode* root) {
	if (root == nullptr) return;
	inorder(root->left);
	cout << root->val << '\\n';
	inorder(root->right);
}
```

### 后序模板

```cpp
void postorder(TreeNode* root) {
	if (root == nullptr) return;
	postorder(root->left);
	postorder(root->right);
	cout << root->val << '\\n';
}
```

### 层序模板

```cpp
void levelOrder(TreeNode* root) {
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

<aside> 📌

这一节真正要掌握的，不只是“二叉树遍历有哪些名字”，而是看到题目时能立刻判断：**根节点应该先处理还是后处理、该用递归还是队列、以及当前题目的信息会在哪一种遍历顺序里最自然地出现。**

</aside>