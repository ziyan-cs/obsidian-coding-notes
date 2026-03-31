#data-structure #tree-traversal #binary-tree #dfs #bfs

## ⚡ TL;DR（快速决策）

- 二叉树遍历本质是：**按某种固定顺序访问所有节点一次**
- 高频遍历方式：
    - 前序：根-左-右
    - 中序：左-根-右
    - 后序：左-右-根
    - 层序：按层访问
- 一看到“按某种顺序输出节点”或“递归处理整棵树”，就要先想到遍历

## 🧩 Core Idea（核心本质）

- DFS 强调深度方向：先深入子树再回来
- BFS 强调层次方向：一层一层推进
- 一句话理解：
    - **遍历就是按规则访问每个节点，且只访问一次。**
- 很多树题不是“不会做”，而是没看清楚该用哪种遍历顺序

## 🔧 Usage Patterns（可复用代码模板）

1. 前序遍历

```cpp
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->val << ' ';
    preorder(root->left);
    preorder(root->right);
}
```

1. 中序遍历

```cpp
void inorder(TreeNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->val << ' ';
    inorder(root->right);
}
```

1. 后序遍历

```cpp
void postorder(TreeNode* root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    cout << root->val << ' ';
}
```

1. 层序遍历

```cpp
queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
    TreeNode* cur = q.front();
    q.pop();
    if (!cur) continue;
    cout << cur->val << ' ';
    q.push(cur->left);
    q.push(cur->right);
}
```

## ⚠️ Pitfalls（高频错误）

- 递归访问顺序写反
- 层序遍历忘记判空
- 中序有序只对 BST 成立
- 迭代写法中栈 / 队列用错

## 🚀 Performance / Tips（性能优化）

- 遍历整棵树通常是 $O(n)$
- 递归写法最自然，迭代常配栈 / 队列
- 高频经验：
    - 要根节点先处理：前序
    - 要最后合并子树：后序
    - 要有序输出 BST：中序
    - 要按层推进：层序

## 🧪 Common Scenarios（常见使用场景）

- 输出遍历序列
- 统计节点数 / 树高 / 子树信息
- 构造树、恢复树
- 层序打印
- 路径与递归信息合并

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
void preorder(TreeNode* r) {
    if (!r) return;
    cout << r->val << ' ';
    preorder(r->left);
    preorder(r->right);
}
int main() {
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    preorder(root);
}
```

## 📌 One-liner Summary（一句话总结）

- **二叉树遍历就是：按前序、中序、后序或层序等固定规则访问整棵树。**