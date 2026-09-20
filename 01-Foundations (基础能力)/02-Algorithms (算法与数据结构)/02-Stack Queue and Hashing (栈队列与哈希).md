---
study_stage: backlog
tags: [algorithm/data-structure, algorithm/bfs, learning/foundation]
---

> [!abstract] 学习目标
> 理解栈、队列和哈希表分别保存怎样的历史信息，并能从不变量推导单调栈、BFS 和哈希计数方案。

# 栈与单调栈

栈是后进先出（LIFO）结构，适合表达尚未闭合的调用、括号、区间或状态。函数调用栈、表达式求值和 DFS 的迭代实现都使用这一性质。

**单调栈（monotonic stack）**在栈内维护单调序列，用弹栈动作一次性确定某些元素的最近更大/更小边界。每个元素至多入栈、出栈一次，所以总复杂度为 `O(n)`，而不是把每次 `while` 误判为 `O(n²)`。

## 下一个严格更大元素

```cpp
std::vector<int> next_greater(const std::vector<int>& a) {
    std::vector<int> answer(a.size(), -1);
    std::vector<std::size_t> stack; // 下标；对应值保持单调不增

    for (std::size_t i = 0; i < a.size(); ++i) {
        while (!stack.empty() && a[stack.back()] < a[i]) {
            answer[stack.back()] = static_cast<int>(i);
            stack.pop_back();
        }
        stack.push_back(i);
    }
    return answer;
}
```

栈中保存“右侧答案尚未出现”的下标。严格/非严格比较必须按题意选择：重复值是否能成为边界，会直接改变 `<` 与 `<=`。

## 柱状图与接雨水

柱状图最大矩形使用单调递增栈；当较矮柱出现时，被弹柱子的右边界确定，弹栈后的新栈顶给出左边界。首尾哨兵可统一清空逻辑。接雨水既可用单调栈按“凹槽层”结算，也可用相向双指针维护左右最大值；选择前先写清每次结算的是面积、宽度还是高度。

# 队列与 BFS

队列先进先出（FIFO），适合按发现顺序处理状态。无权图 BFS 按边数逐层扩展，因此第一次到达节点时得到最短边数。

```cpp
std::vector<int> bfs_distance(
    const std::vector<std::vector<int>>& graph, int source) {
    std::vector<int> dist(graph.size(), -1);
    std::queue<int> q;
    dist[source] = 0;      // 入队时标记，避免重复入队
    q.push(source);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : graph[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
    return dist;
}
```

复杂度为 `O(V + E)`，前提是邻接表表示；若使用邻接矩阵，扫描邻居会改变复杂度。

## 多源 BFS

把所有距离为 0 的源点同时入队，相当于增加一个连接所有源点的虚拟超级源。腐烂橘子、最近设施距离等题都可由此统一。层数可通过 `dist` 记录，也可按当前队列长度分层；不要混用导致多算一步。

## 双端队列

`deque` 支持两端操作。边权仅为 0/1 时可用 0-1 BFS：权 0 的转移压到队首，权 1 压到队尾，得到 `O(V + E)`；它不是普通 BFS 对任意权重的替代。

# 哈希表

哈希表用散列函数把键映射到桶，再通过链式结构或开放寻址解决冲突。平均查找/插入可接近 `O(1)`，但最坏情况可达 `O(n)`；性能还受负载因子、哈希质量、扩容、内存布局和攻击性输入影响。

## 三种常用模式

1. **集合判重**：已见元素、访问状态。
2. **计数映射**：频率统计、异位词、窗口计数。
3. **值到位置/状态**：两数和、前缀和第一次出现位置。

```cpp
std::optional<std::pair<std::size_t, std::size_t>> two_sum(
    const std::vector<int>& a, int target) {
    std::unordered_map<int, std::size_t> position;
    for (std::size_t i = 0; i < a.size(); ++i) {
        long long need64 = static_cast<long long>(target) - a[i];
        if (need64 >= std::numeric_limits<int>::min() &&
            need64 <= std::numeric_limits<int>::max()) {
            auto it = position.find(static_cast<int>(need64));
            if (it != position.end()) return std::pair{it->second, i};
        }
        position.try_emplace(a[i], i);
    }
    return std::nullopt;
}
```

对 `unordered_map`：

- `operator[]` 在键不存在时会插入默认值；只查询时用 `find`/`contains`。
- `reserve` 可减少已知规模下的 rehash，但不保证迭代顺序。
- rehash 会使迭代器失效；引用和指针的失效规则需查对应标准接口。
- 自定义键需要一致的相等关系与哈希：相等的键必须产生相同哈希值。

# 如何选择

| 需求 | 首选结构 | 判断依据 |
|---|---|---|
| 最近未匹配对象 | 栈 | LIFO |
| 按层/发现顺序扩展 | 队列 | FIFO |
| 两端优先级为 0/1 | 双端队列 | 0-1 BFS |
| 近似常数时间判重/映射 | 哈希表 | 不需要有序遍历 |
| 需要有序键与范围查询 | 平衡树/有序容器 | `O(log n)` 换顺序语义 |

# 检查理解

1. 单调栈为什么总体是 `O(n)`？
2. BFS 为什么必须在入队时标记，而不是出队时？
3. 普通 BFS 为什么不能直接处理任意非负权图？
4. 哈希表平均 `O(1)` 隐藏了哪些工程条件？

> [!summary] 本篇结论
> 栈保存最近的未决状态，队列保存按发现顺序待处理的状态，哈希表用额外空间换快速定位。模板正确性的核心分别是单调性、最短层次和键映射语义。

下一步：[03-Heap and Top K (堆与 Top K)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Heap%20and%20Top%20K%20(堆与%20Top%20K).md)
