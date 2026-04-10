#algorithm #recursion #dfs #divide

# 0. 核心

- **函数调用自己**
- 递归想 3 件事：
    - 终止条件
    - 当前层做什么
    - 子问题怎么缩小

# 1. 经典案例：

### 1.1 阶乘

```cpp
long long fact(int n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);
}
```

### 1.2 斐波那契数列

```cpp
long long fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}
```

### 1.3 汉诺塔问题（经典）

```cpp
// 把 n 个盘子从 from 移到 to，借助 aux
void hanoi(int n, char from, char aux, char to) {
    if (n == 1) {          
        cout << "Move " << n << " from " << from << " to " << to << endl;
        return;
    }
    hanoi(n - 1, from, to, aux);
    cout << "Move " << n << " from " << from << " to " << to << endl;
    hanoi(n - 1, aux, from, to);
}
```

### 1.4 二叉树前序遍历（递归版，最经典）

```cpp
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void preOrder(TreeNode* root, vector<int>& res) {
    if (root == nullptr) return;   // 递归终止：空节点
    res.push_back(root->val);      // 根
    preOrder(root->left, res);     // 左
    preOrder(root->right, res);    // 右
}
```

### 1.5 二叉树中序遍历（递归版）

```cpp
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void inOrder(TreeNode* root, vector<int>& res) {
    if (root == nullptr) return;
    inOrder(root->left, res);      // 左
    res.push_back(root->val);      // 根
    inOrder(root->right, res);     // 右
}
```

### 1.6 二叉树后序遍历（递归版）

```cpp
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void postOrder(TreeNode* root, vector<int>& res) {
    if (root == nullptr) return;
    postOrder(root->left, res);    // 左
    postOrder(root->right, res);   // 右
    res.push_back(root->val);      // 根
}
```

### 1.7 链表反转（递归版）

```cpp
struct ListNode {
    int val;
    ListNode* next
};

ListNode* reverseList(ListNode* head) {
    if (head == nullptr || head->next == nullptr) return head;
    ListNode* newHead = reverseList(head->next);  // 递归反转后续链表
    head->next->next = head;  // 反转当前节点的指针
    head->next = nullptr;
    return newHead;
}
```

### 1.8 递归反转字符串

```cpp
void reverseString(string& s, int l, int r) {
    if (l >= r) return;
    swap(s[l], s[r]);
    reverseString(s, l + 1, r - 1);
}
```