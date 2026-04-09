#data-structure #queue #fifo #linear-structure #cpp

# 0. 核心

- **先进先出（FIFO）的受限线性结构**
- **时间复杂度**：入队 / 出队 / 取队头 均为 $O(1)$
- 相关特征：
    - 先来的先处理
    - 按层推进
    - 任务排队调度
    - 缓冲区、事件流处理

# 1. C++ 标准容器

- `std::queue<T>` 的核心接口：
- **特点**：适配器模式，仅暴露 FIFO 接口，不支持随机访问、迭代器遍历

```cpp
queue<int> q;
q.push(x);    // 队尾入队
q.pop();      // 队头出队
q.front();    // 取队头元素
q.back();     // 取队尾元素
q.empty();    // 判断空
q.size();     // 元素个数
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