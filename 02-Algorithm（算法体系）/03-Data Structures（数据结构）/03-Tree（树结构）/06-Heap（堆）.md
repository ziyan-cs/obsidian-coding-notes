

# 1. 定义与性质

- 堆是一棵**完全二叉树**，同时满足：
    
    - 大顶堆：父结点值 ≥ 左右孩子值
    - 小顶堆：父结点值 ≤ 左右孩子值
    
- 存储：用数组（顺序存储）实现，父子下标关系与完全二叉树一致

# 2. 常见操作

- 建堆、插入、删除堆顶元素，时间复杂度 O(logn)
- C++ 中直接用 `priority_queue`：

```cpp
// 大顶堆（默认）
priority_queue<int> maxHeap;
// 小顶堆
priority_queue<int, vector<int>, greater<int>> minHeap;
```

# 3. 典型应用

- 优先队列、TopK 问题、哈夫曼树构造（就是你锯木头那道题）