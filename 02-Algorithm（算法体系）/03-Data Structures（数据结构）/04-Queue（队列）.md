#data-structure #queue #fifo #linear-structure #cpp

## ⚡ TL;DR（快速决策）

- 队列本质是：**先进先出（FIFO）的受限线性结构**
- 相关特征：
    - 先来的先处理
    - 按层推进
    - 任务排队调度
    - 缓冲区、事件流处理
- 核心限制：
    - 队尾进
    - 队头出

## 🧩 Core Idea（核心本质）

- 队列中的元素遵循严格先后顺序
- 最早进入的元素最先离开
- 一句话理解：
    - **像现实中的排队：先来先服务。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础使用

```cpp
#include <iostream>
#include <queue>
using namespace std;
int main() {
    queue<int> q;
    q.push(10); q.push(20); q.push(30);
    cout << q.front() << '\\n';
    q.pop();
    cout << q.front() << '\\n';
}
```

1. BFS 基础框架

```cpp
queue<int> q;
q.push(start);
while (!q.empty()) {
    int u = q.front();
    q.pop();
}
```

1. 层序遍历

```cpp
queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
    auto cur = q.front(); q.pop();
    if (cur->left) q.push(cur->left);
    if (cur->right) q.push(cur->right);
}
```

1. 查看队头和队尾

```cpp
cout << q.front() << '\\n';
cout << q.back() << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- 空队列上直接 `front()` / `back()` / `pop()`
- `pop()` 不返回元素
- 和栈顺序搞混
- BFS 中忘记标记访问导致重复入队

## 🚀 Performance / Tips（性能优化）

- `push/pop/front/back` 常为 $O(1)$
- BFS、层序遍历、调度问题特别常见
- 如果需要两端操作，更适合 `deque`

## 🧪 Common Scenarios（常见使用场景）

- BFS / 层序遍历
- 任务调度
- 缓冲区模型
- 事件流处理
- 模拟排队问题

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
using namespace std;
int main() {
    queue<int> q;
    q.push(1); q.push(2); q.push(3);
    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop();
    }
    return 0;
}
```