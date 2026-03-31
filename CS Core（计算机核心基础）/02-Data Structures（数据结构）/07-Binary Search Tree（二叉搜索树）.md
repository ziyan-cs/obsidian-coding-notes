#data-structure #bst #binary-search-tree #ordered-tree #cpp

## ⚡ TL;DR（快速决策）

- 二叉搜索树（BST）本质是：**满足左小右大的特殊二叉树**
- 一看到这些需求，要优先想到 BST：
    - 有序查找
    - 插入 / 删除 / 搜索
    - 中序遍历得到有序序列
- 它把“二分查找”的思想放到了树结构上
- 但若树不平衡，性能会退化

## 🧩 Core Idea（核心本质）

- 对任意节点 `u`：
    - 左子树所有值 < `u`
    - 右子树所有值 > `u`（重复值按题目约定处理）
- 一句话理解：
    - **BST = 树上的二分查找。**
- 关键性质：
    - 查找某个值时，只需决定往左还是往右
    - 中序遍历结果天然有序

## 🔧 Usage Patterns（可复用代码模板）

1. 查找

```cpp
TreeNode* search(TreeNode* root, int x) {
    if (!root || root->val == x) return root;
    if (x < root->val) return search(root->left, x);
    return search(root->right, x);
}
```

1. 插入

```cpp
TreeNode* insert(TreeNode* root, int x) {
    if (!root) return new TreeNode(x);
    if (x < root->val) root->left = insert(root->left, x);
    else root->right = insert(root->right, x);
    return root;
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

1. 最小值节点

```cpp
TreeNode* getMin(TreeNode* root) {
    while (root && root->left) root = root->left;
    return root;
}
```

## ⚠️ Pitfalls（高频错误）

- 把普通二叉树和 BST 混淆
- 删除节点逻辑最容易出错
- 忘了考虑树可能退化成链
- 中序有序只对 BST 成立

## 🚀 Performance / Tips（性能优化）

- 平均查找 / 插入 / 删除：$O(log n)$
- 最坏情况下：$O(n)$
- 高频经验：
    - BST 题先抓住“左小右大”
    - 中序遍历常是关键突破口
    - 平衡性不保证时别盲信对数复杂度

## 🧪 Common Scenarios（常见使用场景）

- 有序查找
- 动态插入删除
- 第 k 小元素
- 验证是否为 BST
- 中序遍历恢复有序关系

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
void inorder(TreeNode* r) {
    if (!r) return;
    inorder(r->left);
    cout << r->val << ' ';
    inorder(r->right);
}
int main() {
    TreeNode* root = new TreeNode(2);
    root->left = new TreeNode(1);
    root->right = new TreeNode(3);
    inorder(root);
}
```

## 📌 One-liner Summary（一句话总结）

- **二叉搜索树就是：满足左小右大性质、支持有序查找的特殊二叉树。**