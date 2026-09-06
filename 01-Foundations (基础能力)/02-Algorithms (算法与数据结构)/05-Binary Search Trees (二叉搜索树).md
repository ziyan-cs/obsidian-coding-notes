---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 05-Binary Search Trees (二叉搜索树)

> [!abstract] 阅读定位
>
> 本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

## Binary Search Tree and AVL (二叉搜索树与 AVL)

> [!abstract] 核心考点：BST 中序遍历递增性质、查找/插入/删除的迭代与递归、AVL 旋转平衡机制

## BST 性质

- 左子树键小于根、右子树键大于根；若允许重复键，必须在实现中规定“相等键放哪边”或额外计数
- **中序遍历结果递增**；不允许重复键时才严格递增
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
        if (!root->left)  return root->right;  // 教学简化：真实拥有节点时还需释放 root
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

**红黑树**：放松 AVL 的高度平衡条件，通常旋转更少。许多标准库实现会用红黑树实现有序关联容器，但 C++ 标准规定的是对数复杂度等行为，不规定必须使用哪一种树。

---

## 30 秒回答

**BST 和 AVL 的差别？** 普通 BST 的高度可能退化到 `O(n)`；AVL 把每个节点的左右子树高度差限制在 1 内，从而保证查找、插入、删除为 `O(log n)`，代价是维护高度和旋转。删除节点有两个孩子时，常复制中序后继/前驱的键再递归删除它。

**自测：** 为什么中序遍历能验证 BST？允许重复键时，需要额外规定什么？

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)

## 常见误区

- 把 **05-Binary Search Trees (二叉搜索树)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
05-Binary Search Trees (二叉搜索树)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
