---
study_stage: backlog
tags: [algorithm/data-structure, algorithm/selection, learning/foundation]
---

> [!abstract] 学习目标
> 理解二叉堆的形状与序性质，掌握建堆、优先队列、Top-K、堆排序和 Quickselect 的选择边界。

# 二叉堆

二叉堆同时满足：

1. **完全二叉树**：除最后一层外全部填满，最后一层从左到右填充；
2. **堆序性质**：最大堆中父节点不小于孩子，最小堆相反。

因此可以紧凑存入数组。对从 0 开始的下标 `i`：

```text
parent = (i - 1) / 2       // i > 0
left   = 2 * i + 1
right  = 2 * i + 2
```

堆只保证根是极值，不保证数组整体有序，也不能在 `O(log n)` 内查找任意值。

| 操作 | 复杂度 | 机制 |
|---|---:|---|
| 读取极值 | `O(1)` | 根节点 |
| 插入 | `O(log n)` | 尾部插入后上浮 |
| 删除极值 | `O(log n)` | 末尾换根后下沉 |
| 自底向上建堆 | `O(n)` | 从最后一个非叶节点下沉 |

建堆不是 `O(n log n)`：多数节点接近叶子，下沉距离很短，各高度工作量求和为 `O(n)`。

# C++ 优先队列

`std::priority_queue<T>` 默认是最大堆：

```cpp
std::priority_queue<int> max_heap;
std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
```

自定义比较器表达“优先级较低”的关系，语义容易写反。可先放入少量值，验证 `top()` 是否为预期元素。标准适配器不提供任意元素更新/删除；Dijkstra 常采用重复入堆，并在弹出时丢弃过期条目。

# Top-K 的三种主方案

假设从 `n` 个元素找最大的 `k` 个：

| 方案 | 时间 | 额外空间 | 适用场景 |
|---|---:|---:|---|
| 全排序 | `O(n log n)` | 取决于排序 | 还需要全局顺序 |
| 大小为 `k` 的最小堆 | `O(n log k)` | `O(k)` | 流式输入、`k` 小、需维护集合 |
| Quickselect | 平均 `O(n)`，最坏 `O(n²)` | 原地时较小 | 离线数组，只需阈值/第 k 个 |

## 最小堆维护最大 K 个

```cpp
std::vector<int> top_k_largest(const std::vector<int>& a, std::size_t k) {
    if (k == 0) return {};
    k = std::min(k, a.size());
    std::priority_queue<int, std::vector<int>, std::greater<int>> heap;

    for (int x : a) {
        if (heap.size() < k) heap.push(x);
        else if (x > heap.top()) {
            heap.pop();
            heap.push(x);
        }
    }

    std::vector<int> result;
    while (!heap.empty()) {
        result.push_back(heap.top());
        heap.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
```

不变量：堆内始终是已处理前缀中最大的至多 `k` 个元素，堆顶是它们的门槛。若要求去重 Top-K，必须另外定义重复值语义。

# Quickselect

Quickselect 使用 partition 把枢轴放到最终位置，只递归/迭代进入包含目标下标的一侧。随机枢轴或更强的选择策略能降低退化风险；它会修改输入，且前 `k` 个元素通常无序。

C++ 已提供 `std::nth_element`：执行后，第 `nth` 个元素与完全排序时相同，之前元素不大于它之后的元素（按比较器语义），但两侧内部不保证有序。工程代码优先使用标准算法。

# 堆的其他应用

- 合并 K 个有序序列：堆中保存每个序列当前候选，复杂度 `O(N log k)`。
- 数据流中位数：最大堆保存较小一半，最小堆保存较大一半，保持大小差不超过 1。
- 任务调度：优先队列表达可比较的下一候选，但取消、更新优先级和公平性需要额外设计。
- 堆排序：原地 `O(n log n)`、通常不稳定，缓存局部性往往不如高度优化的库排序。

# 检查理解

1. 为什么最小堆适合维护“最大的 K 个”？
2. 自底向上建堆为什么是 `O(n)`？
3. Quickselect 得到前 K 个后，为什么仍可能需要排序？
4. `priority_queue` 中出现重复过期条目时怎样保证结果正确？

> [!summary] 本篇结论
> 堆用部分有序换取极值更新效率；Top-K 选择取决于是否流式、是否需要全序、是否允许修改输入以及最坏复杂度要求。先定义输出语义，再选全排序、受限堆或选择算法。

下一步：[04-Tree Traversal and Paths (树遍历与路径)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/04-Tree%20Traversal%20and%20Paths%20(树遍历与路径).md)
