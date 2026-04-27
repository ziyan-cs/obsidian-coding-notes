

# 1. 定义

- 左子树所有结点值 < 根结点值
- 右子树所有结点值 > 根结点值
- 左右子树也必须是 BST

# 2. 核心性质

- 中序遍历结果是**严格递增序列**
- 查找效率接近二分查找，时间复杂度 O(logn)（平衡时）

# 3. 常见操作模板

- 查找：

```cpp
TreeNode* searchBST(TreeNode* root, int val) {
    if (!root || root->val == val) return root;
    if (val < root->val) return searchBST(root->left, val);
    else return searchBST(root->right, val);
}
```

- 插入：

```cpp
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);
    if (val < root->val) root->left = insertIntoBST(root->left, val);
    else root->right = insertIntoBST(root->right, val);
    return root;
}
```