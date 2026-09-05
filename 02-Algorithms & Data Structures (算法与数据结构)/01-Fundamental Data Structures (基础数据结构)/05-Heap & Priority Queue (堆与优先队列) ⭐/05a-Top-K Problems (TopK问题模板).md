---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：堆的 push/pop 操作、TopK 的堆解法 vs 快速选择、海量数据处理的堆思路

## 堆的性质

- **最小堆**：父节点 ≤ 子节点，堆顶是最小值
- **最大堆**：父节点 ≥ 子节点，堆顶是最大值
- 完全二叉树，用数组存储：节点 i 的左子 = 2i+1，右子 = 2i+2，父 = (i-1)//2

核心操作：

- `push`：加入末尾，向上冒泡（sift up），O(log n)
- `pop`：移除堆顶，末尾元素放到顶，向下沉（sift down），O(log n)
- `heapify`：将任意数组原地建堆，O(n)

```cpp
#include <queue>

// C++ priority_queue 默认最大堆
priority_queue<int> maxHeap;                              // 最大堆
priority_queue<int, vector<int>, greater<int>> minHeap;  // 最小堆

// 存 pair：按第一个元素排序
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
```

---

## Top-K Problems 解决

### 方案对比

|方案|时间复杂度|适用场景|
|---|---|---|
|全排序|O(n log n)|简单，n 较小|
|最小堆维护大小为 K 的窗口|O(n log K)|**最常用**，n 大 K 小|
|快速选择（QuickSelect）|O(n) 平均|只需第 K 大，不需要排序|

### 最小堆求 Top-K 最大值

```cpp
vector<int> topKLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minH;
    for (int x : nums) {
        minH.push(x);
        if ((int)minH.size() > k) minH.pop();
    }
    vector<int> res;
    while (!minH.empty()) { res.push_back(minH.top()); minH.pop(); }
    return res;
}
```

### QuickSelect（平均 O(n) 找第 K 大）

```cpp
int findKthLargest(vector<int>& nums, int k) {
    int l = 0, r = (int)nums.size() - 1, target = k - 1;
    while (l <= r) {
        // 随机化 pivot
        int ri = l + rand() % (r - l + 1);
        swap(nums[ri], nums[r]);
        int pivot = nums[r], store = l;
        for (int i = l; i < r; i++)
            if (nums[i] >= pivot) swap(nums[i], nums[store++]);
        swap(nums[store], nums[r]);
        if      (store == target) return nums[store];
        else if (store < target)  l = store + 1;
        else                      r = store - 1;
    }
    return -1;
}
```

### 经典变体

```cpp
// 前 K 个高频元素
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int,int> freq;
    for (int x : nums) freq[x]++;
    using P = pair<int,int>;
    priority_queue<P, vector<P>, greater<P>> pq;  // 按频率最小堆
    for (auto& [val, cnt] : freq) {
        pq.push({cnt, val});
        if ((int)pq.size() > k) pq.pop();
    }
    vector<int> res;
    while (!pq.empty()) { res.push_back(pq.top().second); pq.pop(); }
    return res;
}
```

```cpp
// 数据流中的第 K 大
class KthLargest {
    priority_queue<int, vector<int>, greater<int>> minH;
    int k;
public:
    KthLargest(int k, vector<int>& nums) : k(k) {
        for (int x : nums) add(x);
    }
    int add(int val) {
        minH.push(val);
        if ((int)minH.size() > k) minH.pop();
        return minH.top();
    }
};
```

```cpp
// 合并 K 个有序链表
struct Cmp {
    bool operator()(ListNode* a, ListNode* b) { return a->val > b->val; }
};
ListNode* mergeKLists(vector<ListNode*>& lists) {
    priority_queue<ListNode*, vector<ListNode*>, Cmp> pq;
    for (auto* node : lists)
        if (node) pq.push(node);
    ListNode dummy(0), *cur = &dummy;
    while (!pq.empty()) {
        auto* node = pq.top(); pq.pop();
        cur->next = node;
        cur = cur->next;
        if (node->next) pq.push(node->next);
    }
    return dummy.next;
}
```

---

## 关联笔记

- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
