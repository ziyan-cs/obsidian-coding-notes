---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 04-Tree Traversal and Paths (树遍历与路径)

> [!abstract] 阅读定位
>
> 本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

## 08-Tree Depth First Traversal (树的深度优先遍历)

> [!abstract] 核心考点：二叉树 DFS 前序/中序/后序遍历、递归与迭代栈实现、遍历序列还原
> [!abstract] 核心考点：二叉树 DFS 前序/中序/后序三种遍历方式、递归与迭代实现、遍历序列还原树结构

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

---

## 关联笔记

- [BFS： Level Order Traversal (层序遍历)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06b-BFS：%20Level%20Order%20Traversal%20(层序遍历).md)
- [LCA & Path Problems (公共祖先与路径)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06c-LCA%20&%20Path%20Problems%20(公共祖先与路径).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)

---

## 09-Tree Breadth First Traversal (树的层序遍历)

> [!abstract] 核心考点：二叉树 BFS 层序遍历模板、按层输出的变体、之字形/Zigzag 遍历
>见 [03b-BFS](02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md) 模板，二叉树层序遍历已包含

层序遍历的变体：

python

```cpp
// 锯齿形层序
vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
    if (!root) return {};
    queue<TreeNode*> q;
    q.push(root);
    vector<vector<int>> res;
    bool l2r = true;
    while (!q.empty()) {
        int sz = q.size();
        deque<int> level;
        for (int i = 0; i < sz; i++) {
            auto node = q.front(); q.pop();
            if (l2r) level.push_back(node->val);
            else     level.push_front(node->val);
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
        res.push_back(vector<int>(level.begin(), level.end()));
        l2r = !l2r;
    }
    return res;
}

// 右视图
vector<int> rightSideView(TreeNode* root) {
    if (!root) return {};
    queue<TreeNode*> q;
    q.push(root);
    vector<int> res;
    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            auto node = q.front(); q.pop();
            if (i == sz - 1) res.push_back(node->val);
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return res;
}
```

---

## 关联笔记

- [DFS： Preorder ⧸ Inorder ⧸ Postorder (前中后序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06a-DFS：%20Preorder%20⧸%20Inorder%20⧸%20Postorder%20(前中后序).md)
- [LCA & Path Problems (公共祖先与路径)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06c-LCA%20&%20Path%20Problems%20(公共祖先与路径).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)

---

## 10-LCA and Tree Paths (最近公共祖先与路径)

> [!abstract] 核心考点：最近公共祖先 LCA 递归解法、二叉树路径问题模式、根到叶路径 DFS 求和
> [!abstract] 核心考点：最近公共祖先 LCA 的递归解法、二叉树路径问题模式、根到叶路径的 DFS 求和

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

- [DFS： Preorder ⧸ Inorder ⧸ Postorder (前中后序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06a-DFS：%20Preorder%20⧸%20Inorder%20⧸%20Postorder%20(前中后序).md)
- [BFS： Level Order Traversal (层序遍历)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Trees%20&%20Graphs%20(树与图)/06-Binary%20Tree%20(二叉树)%20⭐/06b-BFS：%20Level%20Order%20Traversal%20(层序遍历).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)