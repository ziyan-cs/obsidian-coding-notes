#algorithm #recursion #dfs #divide

## 核心

- 递归 = **函数调用自己**
- 递归想 3 件事：
    - 终止条件
    - 当前层做什么
    - 子问题怎么缩小

## 经典例子：

### 1. 阶乘

```cpp
long long fact(int n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);
}
```

### 2. 斐波那契数列（入门）

```cpp
long long fact(int n) {
    if (n <= 1) return 1;
    return f
}
```

### 3. **汉诺塔问题**（经典）

```cpp
long long fact(int n) {
    if (n <= 1) return 1;
    return 1LL * n * fact(n - 1);
}
```

### 4. 二叉树前序遍历（递归版，最经典）

```cpp
void preOrder(TreeNode* root, vector<int>& res) {
    if (root == nullptr) return;
    res.push_back(root->val);  // 处理当前节点
    preorder(root->left, res);  // 递归左子树
    preorder(root->right, res);  // 递归右子树
}
```

### 5. 链表反转（递归版）

```cpp
ListNode* reverseList(ListNode* head) {
    if (head == nullptr || head->next == nullptr) return head;
    ListNode* newHead = reverseList(head->next);  // 递归反转后续链表
    head->next->next = head;  // 反转当前节点的指针
    head->next = nullptr;
    return newHead;
}
```


### 5. 递归反转字符串

```cpp
void reverseString(string& s, int l, int r) {
    if (l >= r) return;
    swap(s[l], s[r]);
    reverseString(s, l + 1, r - 1);
}
```

## 什么时候用

- 问题天然能拆成同结构子问题
- 树、分治、回溯、DFS
- 写迭代很别扭时

## 高频坑点

- 没有终止条件
- 递归参数不收缩
- 返回值和副作用混着写
- 爆栈但自己没意识到