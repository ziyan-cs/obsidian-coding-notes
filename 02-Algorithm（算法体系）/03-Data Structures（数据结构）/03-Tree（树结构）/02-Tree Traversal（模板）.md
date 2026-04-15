#data-structure #tree-traversal #binary-tree #dfs #bfs

## ⚡ TL;DR（快速决策）

- 二叉树遍历本质是：**按某种固定顺序访问所有节点一次**
- 高频遍历方式：
    - 前序：根-左-右
    - 中序：左-根-右
    - 后序：左-右-根
    - 层序：按层访问
- 一看到“按某种顺序输出节点”或“递归处理整棵树”，就要先想到遍历

## 🧩 Core Idea（核心本质）

- DFS 强调深度方向：先深入子树再回来
- BFS 强调层次方向：一层一层推进
- 一句话理解：
    - **遍历就是按规则访问每个节点，且只访问一次。**
- 很多树题不是“不会做”，而是没看清楚该用哪种遍历顺序

## 🔧 Usage Patterns（可复用代码模板）

1. 前序遍历
- 递归实现
```cpp
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->val << ' ';
    preorder(root->left);
    preorder(root->right);
}
```

- 圈占实现

```cpp
vector<int> preorderTraversal(TreeNode* root) {
    vector<int> res;       // 保存遍历结果
    stack<TreeNode*> st;   // 栈，用来模拟递归
    if (root == NULL) return res;
    st.push(root);  // 根节点先入栈
    while (!st.empty()) {
        // 1. 出栈 → 访问
        TreeNode* cur = st.top();
        st.pop();
        res.push_back(cur->val);
        // 2. 先压右，再压左（栈后进先出，保证下次先访问左）
        if (cur->right != NULL) {
            st.push(cur->right);
        }
        if (cur->left != NULL) {
            st.push(cur->left);
        }
    }
    return res;
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

1. 后序遍历

```cpp
void postorder(TreeNode* root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    cout << root->val << ' ';
}
```

1. 层序遍历

```cpp
queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
    TreeNode* cur = q.front();
    q.pop();
    if (!cur) continue;
    cout << cur->val << ' ';
    q.push(cur->left);
    q.push(cur->right);
}
```
