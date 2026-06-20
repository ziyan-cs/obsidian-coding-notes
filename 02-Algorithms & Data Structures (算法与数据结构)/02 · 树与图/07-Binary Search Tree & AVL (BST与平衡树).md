---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：BST 中序遍历递增性质、查找/插入/删除的迭代与递归、AVL 旋转平衡机制

## BST 性质

- 左子树所有节点 < 根 < 右子树所有节点
- **中序遍历结果严格递增**（验证 BST 的依据）
- 查找、插入、删除平均 O(log n)，最坏 O(n)（退化为链表）

```cpp
// BST 查找
TreeNode* searchBST(TreeNode* root, int val) {
    if (!root || root->val == val) return root;
    return val < root->val ? searchBST(root->left, val)
                           : searchBST(root->right, val);
}

// BST 插入
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if (!root) return new TreeNode(val);
    if (val < root->val) root->left  = insertIntoBST(root->left,  val);
    else                 root->right = insertIntoBST(root->right, val);
    return root;
}

// BST 删除
TreeNode* deleteNode(TreeNode* root, int key) {
    if (!root) return nullptr;
    if (key < root->val) {
        root->left  = deleteNode(root->left,  key);
    } else if (key > root->val) {
        root->right = deleteNode(root->right, key);
    } else {
        if (!root->left)  return root->right;
        if (!root->right) return root->left;
        // 找右子树最小节点（中序后继）
        TreeNode* succ = root->right;
        while (succ->left) succ = succ->left;
        root->val   = succ->val;
        root->right = deleteNode(root->right, succ->val);
    }
    return root;
}
```

## AVL 树

AVL 树是**自平衡 BST**，任意节点的左右子树高度差（平衡因子）不超过 1。

**平衡因子 = height(left) - height(right)**，维持在 {-1, 0, 1}。

### 旋转操作

```
LL 失衡（左左）→ 右旋：
      z                y
     / \              / \
    y   T4    →      x   z
   / \              / \ / \
  x   T3           T1 T2T3 T4

RR 失衡（右右）→ 左旋（镜像）

LR 失衡（左右）→ 先左旋 y，再右旋 z

RL 失衡（右左）→ 先右旋 y，再左旋 z
```

AVL 树保证严格平衡，查找 O(log n)；代价是插入/删除可能需要多次旋转。

**红黑树（C++ map/set 底层）**：放松平衡条件（最长路径 ≤ 2 × 最短路径），旋转次数少，工程中更常用。

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
