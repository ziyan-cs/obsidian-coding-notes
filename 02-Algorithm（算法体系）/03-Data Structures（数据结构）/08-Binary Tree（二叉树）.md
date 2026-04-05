#data-structure #binary-tree #tree #node #recursion

## ⚡ TL;DR（快速决策）

- 二叉树本质是：**每个节点最多只有两个孩子的树结构**
- 一看到这些需求，要优先想到二叉树：
    - 左右子树
    - 递归定义明显
    - 树高、节点数、路径和
    - 前中后序、层序遍历
- 二叉树题的核心通常是：
    - 当前节点从左右子树得到什么
    - 当前节点向父节点返回什么

## 🧩 Core Idea（核心本质）

- 每个节点通常包含：
    - 值 `val`
    - 左孩子 `left`
    - 右孩子 `right`
- 一句话理解：
    - **树的问题通常都可以拆成左右子树的问题。**
- 二叉树的关键不是数组下标，而是结构递归

## 🔧 Usage Patterns（可复用代码模板）

1. 节点定义

```cpp
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
```

1. 前序遍历

```cpp
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->val << ' ';
    preorder(root->left);
    preorder(root->right);
}
```

1. 求树高

```cpp
int height(TreeNode* root) {
    if (!root) return 0;
    return max(height(root->left), height(root->right)) + 1;
}
```

1. 节点计数

```cpp
int countNodes(TreeNode* root) {
    if (!root) return 0;
    return countNodes(root->left) + countNodes(root->right) + 1;
}
```

1. 层序遍历思想

```cpp
queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
    auto cur = q.front(); q.pop();
}
```

## ⚠️ Pitfalls（高频错误）

- 空节点忘记判空
- 左右子树逻辑和题意不匹配
- 递归返回值定义不清
- 把二叉树和 BST 混淆

## 🚀 Performance / Tips（性能优化）

- 遍历整棵树通常是 $O(n)$
- 递归空间通常和树高相关，为 $O(h)$
- 高频经验：
    - 先定义函数返回什么
    - 再想左右子树如何合并

## 🧪 Common Scenarios（常见使用场景）

- 前中后序遍历
- 树高 / 节点数 / 叶子数
- 路径和问题
- 判断平衡 / 对称 / 相同树
- 递归结构分析

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
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->val << ' ';
    preorder(root->left);
    preorder(root->right);
}
int main() {
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    preorder(root);
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **二叉树就是：每个节点最多有两个孩子、天然适合递归分解和遍历的树结构。**