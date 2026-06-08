---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：二叉树 BFS 层序遍历模板、按层输出的变体、之字形/Zigzag 遍历
>见 [03b-BFS](02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md) 模板，二叉树层序遍历已包含

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

- [DFS： Preorder ⧸ Inorder ⧸ Postorder (前中后序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02%20·%20树与图/06-Binary%20Tree%20(二叉树)%20⭐/06a-DFS：%20Preorder%20⧸%20Inorder%20⧸%20Postorder%20(前中后序).md)
- [LCA & Path Problems (公共祖先与路径)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/02%20·%20树与图/06-Binary%20Tree%20(二叉树)%20⭐/06c-LCA%20&%20Path%20Problems%20(公共祖先与路径).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
