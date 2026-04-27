

# 1. 二叉树的高度 / 深度

```
int height(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(height(root->left), height(root->right));
}
```

# 2. 叶子结点数

```
int leafCount(TreeNode* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1;
    return leafCount(root->left) + leafCount(root->right);
}
```

# 3. 二叉树的直径（左右子树高度和的最大值）

```
int ans = 0;
int dfs(TreeNode* root) {
    if (!root) return 0;
    int l = dfs(root->left);
    int r = dfs(root->right);
    ans = max(ans, l + r);
    return 1 + max(l, r);
}
```

### 4. 二叉树的镜像 / 翻转

cpp

运行

```
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;
    swap(root->left, root->right);
    invertTree(root->left);
    invertTree(root->right);
    return root;
}
```

### 5. 路径和问题

- 经典题：是否存在根到叶子的路径和等于 target

cpp

运行

```
bool hasPathSum(TreeNode* root, int sum) {
    if (!root) return false;
    if (!root->left && !root->right) return sum == root->val;
    return hasPathSum(root->left, sum - root->val) || hasPathSum(root->right, sum - root->val);
}
```

### 最近公共祖先