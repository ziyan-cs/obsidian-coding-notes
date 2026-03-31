#algorithm #recursion #dfs #divide-and-conquer #function-call

## ⚡ TL;DR（快速决策）

- 递归本质是：**函数定义自己调用自己，把大问题拆成同类小问题**
- 一看到这些特征，要优先想到递归：
    - 树结构
    - 分治问题
    - 回溯搜索
    - 数学定义天然递归
- 递归最重要的不是“自己调自己”，而是：
    - 终止条件
    - 子问题定义
    - 返回值含义
- 写递归前先想清楚“函数到底在求什么”

## 🧩 Core Idea（核心本质）

- 递归通常包含三部分：
    1. 函数定义
    2. 终止条件
    3. 递归转移
- 一句话理解：
    - **递归就是把当前问题交给规模更小的同类问题去完成。**
- 递归特别适合处理：
    - 树
    - 分形结构
    - 层层拆分的子问题

## 🔧 Usage Patterns（可复用代码模板）

1. 阶乘

```cpp
int fact(int n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);
}
```

1. 递归遍历树

```cpp
void dfs(TreeNode* root) {
    if (!root) return;
    dfs(root->left);
    dfs(root->right);
}
```

1. 分治求和

```cpp
int sum(int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    return sum(l, mid) + sum(mid + 1, r);
}
```

1. 递归打印数组

```cpp
void print(int i) {
    if (i == n) return;
    cout << a[i] << ' ';
    print(i + 1);
}
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：没有终止条件，导致无限递归
- 最常见错误 2：终止条件写错，导致少算 / 多算
- 最常见错误 3：函数含义不清，返回值乱套
- 最常见错误 4：子问题没有变小
- 最常见错误 5：递归层数过深导致栈溢出

## 🚀 Performance / Tips（性能优化）

- 递归的时间复杂度取决于递归树结构
- 递归空间通常和调用深度有关
- 高频经验：
    - 先用一句话定义函数
    - 再写终止条件
    - 最后才写递归调用
- 对某些深度很大的问题，可能需要改成迭代

## 🧪 Common Scenarios（常见使用场景）

- 树遍历
- 分治
- 回溯
- 数学递推问题
- DFS 搜索

## 🆚 Recursion vs Iteration

- 递归：写法自然，适合树和分治
- 迭代：更节省调用栈，适合深度很大场景
- 核心区别：
    - **递归把状态放在调用栈里，迭代常把状态手动维护在数据结构里。**

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    cout << fib(5) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **递归就是：通过函数不断处理规模更小的同类问题，直到到达终止条件。**