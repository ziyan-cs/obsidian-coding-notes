---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：最近公共祖先 LCA 递归解法、二叉树路径问题模式、根到叶路径 DFS 求和
> [!important] **核心考点**：最近公共祖先 LCA 的递归解法、二叉树路径问题模式、根到叶路径的 DFS 求和

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

---

## 关联笔记

- [DFS： Preorder ⧸ Inorder ⧸ Postorder (前中后序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02%20·%20树与图/06-Binary%20Tree%20(二叉树)%20⭐/06a-DFS：%20Preorder%20⧸%20Inorder%20⧸%20Postorder%20(前中后序).md)
- [BFS： Level Order Traversal (层序遍历)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02%20·%20树与图/06-Binary%20Tree%20(二叉树)%20⭐/06b-BFS：%20Level%20Order%20Traversal%20(层序遍历).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
