

# 1. 递归遍历（模板）

- 前序（根左右）

```cpp
void preorder(TreeNode* root) {
    if (!root) return;
    // 访问根
    preorder(root->left);
    preorder(root->right);
}
```

- 中序（左根右）

```cpp
void inorder(TreeNode* root) {
    if (!root) return;
    inorder(root->left);
    // 访问根
    inorder(root->right);
}
```

- 后序（左右根）

```cpp
void postorder(TreeNode* root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    // 访问根
}
```

# 2. 层序遍历（BFS）

```cpp
#include <queue>
void levelorder(TreeNode* root) {
    if (!root) return;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        auto node = q.front(); q.pop();
        // 访问node
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }
}
```