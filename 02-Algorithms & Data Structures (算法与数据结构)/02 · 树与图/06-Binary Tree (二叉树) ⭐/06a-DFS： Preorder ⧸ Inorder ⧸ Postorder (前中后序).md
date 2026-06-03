---
tags:
  - algorithm
  - tree-graph
---

> **核心考点**：二叉树 DFS 前序/中序/后序遍历、递归与迭代栈实现、遍历序列还原
> **核心考点**：二叉树 DFS 前序/中序/后序三种遍历方式、递归与迭代实现、遍历序列还原树结构

### 递归模板

```cpp
void preorder(TreeNode* root, vector<int>& res) {
    if (!root) return;
    res.push_back(root->val);
    preorder(root->left, res);
    preorder(root->right, res);
}
```

### 迭代实现（必须掌握）

```cpp
// 迭代中序（面试高频）
vector<int> inorderIter(TreeNode* root) {
    vector<int> res;
    stack<TreeNode*> st;
    TreeNode* curr = root;
    while (curr || !st.empty()) {
        while (curr) { st.push(curr); curr = curr->left; }  // 一路向左
        curr = st.top(); st.pop();
        res.push_back(curr->val);
        curr = curr->right;
    }
    return res;
}

// 迭代前序
vector<int> preorderIter(TreeNode* root) {
    if (!root) return {};
    vector<int> res;
    stack<TreeNode*> st;
    st.push(root);
    while (!st.empty()) {
        auto node = st.top(); st.pop();
        res.push_back(node->val);
        if (node->right) st.push(node->right);  // 先压右
        if (node->left)  st.push(node->left);
    }
    return res;
}

// 迭代后序 = 前序（根右左）结果反转
vector<int> postorderIter(TreeNode* root) {
    if (!root) return {};
    vector<int> res;
    stack<TreeNode*> st;
    st.push(root);
    while (!st.empty()) {
        auto node = st.top(); st.pop();
        res.push_back(node->val);
        if (node->left)  st.push(node->left);
        if (node->right) st.push(node->right);
    }
    reverse(res.begin(), res.end());
    return res;
}
```

### 通用 DFS 解题思路

解二叉树题时问自己两个问题：

1. **这个函数需要返回什么值？**（高度、路径和、节点数、是否满足条件…）
2. **当前节点需要做什么？** 利用左右子树的返回值怎么计算当前节点的答案？

```cpp
// 最大深度
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(maxDepth(root->left), maxDepth(root->right));
}

// 判断平衡二叉树（-1 表示不平衡）
int checkHeight(TreeNode* node) {
    if (!node) return 0;
    int l = checkHeight(node->left);
    int r = checkHeight(node->right);
    if (l == -1 || r == -1 || abs(l - r) > 1) return -1;
    return 1 + max(l, r);
}
bool isBalanced(TreeNode* root) { return checkHeight(root) != -1; }

// 二叉树直径
int ans = 0;
int depth(TreeNode* node) {
    if (!node) return 0;
    int l = depth(node->left), r = depth(node->right);
    ans = max(ans, l + r);
    return 1 + max(l, r);
}
int diameterOfBinaryTree(TreeNode* root) { ans = 0; depth(root); return ans; }

// 二叉树中最大路径和
int maxSum = INT_MIN;
int gain(TreeNode* node) {
    if (!node) return 0;
    int l = max(gain(node->left),  0);
    int r = max(gain(node->right), 0);
    maxSum = max(maxSum, node->val + l + r);
    return node->val + max(l, r);
}
int maxPathSum(TreeNode* root) { maxSum = INT_MIN; gain(root); return maxSum; }
```