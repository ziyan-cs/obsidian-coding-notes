---
tags:
  - algorithm
  - tree-graph
---

> **核心考点**：二叉树 BFS 层序遍历模板、按层输出的变体、之字形/Zigzag 遍历
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