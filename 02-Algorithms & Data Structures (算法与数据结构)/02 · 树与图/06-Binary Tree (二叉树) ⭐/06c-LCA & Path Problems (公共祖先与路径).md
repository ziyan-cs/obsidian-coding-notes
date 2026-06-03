---
tags:
  - algorithm
  - tree-graph
---

> **核心考点**：最近公共祖先 LCA 递归解法、二叉树路径问题模式、根到叶路径 DFS 求和
> **核心考点**：最近公共祖先 LCA 的递归解法、二叉树路径问题模式、根到叶路径的 DFS 求和

### 最近公共祖先（LCA）

```cpp
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    auto left  = lowestCommonAncestor(root->left,  p, q);
    auto right = lowestCommonAncestor(root->right, p, q);
    if (left && right) return root;  // p q 分别在左右子树
    return left ? left : right;
}
```

**原理：** 后序遍历，自底向上，当某节点的左右子树分别找到 p 和 q 时，该节点即 LCA。

### 路径总和问题

```cpp
// 路径总和（根到叶）
bool hasPathSum(TreeNode* root, int target) {
    if (!root) return false;
    if (!root->left && !root->right) return root->val == target;
    return hasPathSum(root->left,  target - root->val) ||
           hasPathSum(root->right, target - root->val);
}

// 路径总和 III（前缀和 + 回溯）
int res = 0;
unordered_map<long long, int> prefix{{0, 1}};

void dfs(TreeNode* node, long long curr, int target) {
    if (!node) return;
    curr += node->val;
    res += prefix[curr - target];
    prefix[curr]++;
    dfs(node->left,  curr, target);
    dfs(node->right, curr, target);
    prefix[curr]--;  // 回溯
}
int pathSum(TreeNode* root, int target) {
    res = 0; prefix = {{0, 1}};
    dfs(root, 0, target);
    return res;
}
```