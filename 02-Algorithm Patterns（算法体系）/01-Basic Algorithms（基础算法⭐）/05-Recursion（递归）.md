#algorithm #recursion #dfs #divide

## 核心

- 递归 = 函数调用自己
- 先想 3 件事：
    - 终止条件
    - 当前层做什么
    - 子问题怎么缩小

## 通用模板

```cpp
int dfs(int x) {
    if (终止条件) return 答案;
    // 当前层逻辑
    return dfs(更小子问题);
}
```

## 高频例子

### 阶乘

```cpp
long long fact(int n) {
    if (n <= 1) return 1;
    return 1LL * n * fact(n - 1);
}
```

### 二叉树前序遍历

```cpp
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->val << ' ';
    preorder(root->left);
    preorder(root->right);
}
```

### 递归反转字符串

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

## 只记这个

- 递归 = 终止条件 + 当前层 + 子问题

## 一句话

- **递归的关键不是“自己调自己”，而是把问题缩小到同结构子问题。**