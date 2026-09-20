---
study_stage: backlog
tags: [algorithm/data-structure, algorithm/two-pointers, learning/foundation]
---

> [!abstract] 学习目标
> 从内存布局、操作代价和不变量理解数组与链表；能够识别双指针、前缀和、链表反转、判环和合并，而不是只背函数模板。

# 数组：连续存储带来的能力与代价

数组把同类型元素连续存放，因此可用 `base + i × sizeof(T)` 在 `O(1)` 时间定位第 `i` 个元素，并具有良好的空间局部性。代价是中间插入/删除通常要搬移后缀；动态数组扩容时还可能重新分配并移动全部元素。

| 操作 | 动态数组 `vector` | 单链表 |
|---|---:|---:|
| 按下标访问 | `O(1)` | `O(n)` |
| 已知位置后插入 | 尾部均摊 `O(1)`；中间 `O(n)` | `O(1)` |
| 查找某个值 | `O(n)` | `O(n)` |
| 内存局部性 | 好 | 通常较差 |
| 迭代器/指针稳定性 | 扩容或搬移可能失效 | 删除目标节点时失效 |

复杂度不能脱离前提：“链表删除是 `O(1)`”要求已经拿到待删节点的前驱；若先按值查找，整体仍是 `O(n)`。

# 双指针：维护区间不变量

双指针不是“放两个变量”，而是用两个边界压缩尚未确定的搜索空间。

## 相向指针

适合有序数组两数和、回文判断、盛水容器等。以升序数组两数和为例：

```cpp
std::optional<std::pair<int, int>> two_sum_sorted(
    const std::vector<int>& a, int target) {
    std::size_t left = 0;
    if (a.empty()) return std::nullopt;
    std::size_t right = a.size() - 1;

    while (left < right) {
        const long long sum = static_cast<long long>(a[left]) + a[right];
        if (sum == target) {
            return std::pair{static_cast<int>(left), static_cast<int>(right)};
        }
        if (sum < target) ++left;
        else --right;
    }
    return std::nullopt;
}
```

循环不变量是：若解存在，它仍位于闭区间 `[left, right]`。移动左端是因为当前和过小，而固定右端时更小的左值都不可能成功；右端同理。

## 快慢指针与原地压缩

快指针读取，慢指针指向下一写入位置：

```cpp
std::size_t remove_value(std::vector<int>& a, int value) {
    std::size_t write = 0;
    for (int x : a) {
        if (x != value) a[write++] = x;
    }
    return write; // 有效区间是 [0, write)
}
```

关键不变量：`[0, write)` 始终是已经处理元素中过滤后的正确结果。函数返回逻辑长度；是否 `resize` 取决于接口约定。

## 分区指针

快速排序 partition、颜色分类等问题把区间划分为“已满足条件 / 未处理 / 另一类”。写代码前先写清每段的开闭边界，否则最容易发生漏元素或重复处理。

# 前缀和：把区间求和变成边界相减

定义 `prefix[0] = 0`，`prefix[i + 1] = a[0] + ... + a[i]`，则半开区间 `[l, r)` 的和为：

```text
sum(l, r) = prefix[r] - prefix[l]
```

```cpp
std::vector<long long> prefix_sum(const std::vector<int>& a) {
    std::vector<long long> prefix(a.size() + 1, 0);
    for (std::size_t i = 0; i < a.size(); ++i) {
        prefix[i + 1] = prefix[i] + a[i];
    }
    return prefix;
}
```

使用 `long long` 是为了降低累计溢出风险，但仍要根据输入上界证明类型足够。二维前缀和、差分数组和“前缀和 + 哈希表”都源于同一思想：缓存可组合的历史信息。

# 链表：先画指针变化

链表题的核心是保持可达性。修改 `next` 前先保存仍需访问的节点。

## 反转单链表

```cpp
struct ListNode {
    int value;
    ListNode* next;
};

ListNode* reverse(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;
    while (cur != nullptr) {
        ListNode* next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    return prev;
}
```

不变量：`prev` 指向已经反转好的前缀，`cur` 指向尚未处理的后缀。结束时后缀为空，`prev` 就是新表头。

## Floyd 判环与入口

快指针每次走两步、慢指针走一步。若存在环，两者终会在环内相遇；若快指针到达空指针，则无环。相遇后把一个指针移回表头，二者都每次走一步，再次相遇处就是环入口。

证明设表头到入口距离为 `μ`，环长为 `λ`。第一次相遇时快慢路程差是 `λ` 的整数倍，可推出从相遇点继续到入口的距离与 `μ` 在模 `λ` 意义下相同。

## 合并有序链表

使用哑节点统一首节点处理。每次把较小节点接到结果尾部，并推进对应链表；循环不变量是结果链表有序，且包含所有已消费节点。若节点所有权由智能指针管理，必须按实际所有权模型重写，不能机械照搬裸指针代码。

# 选择结构的方法

- 需要随机访问、遍历吞吐和紧凑存储：优先数组/`vector`。
- 需要稳定节点地址、频繁在已知位置链接/摘除：考虑链式结构。
- 需要频繁从中间查找后再插入：链表未必更快，因为查找和缓存未命中可能主导成本。
- 实际工程优先使用标准容器，并根据 profiling 而非复杂度表猜测性能。

# 检查理解

1. 为什么 `vector::push_back` 是均摊 `O(1)`，却不是每次 `O(1)`？
2. 相向双指针依赖数组的什么单调性？
3. 为什么前缀和多开一个元素能统一空区间和边界？
4. 反转链表时若先执行 `cur = cur->next` 再改指针，会丢失什么？

> [!summary] 本篇结论
> 数组依靠连续布局提供随机访问与局部性，链表依靠显式链接提供节点级重连。双指针、前缀和与链表模板真正需要记忆的是区间、状态和可达性不变量。

下一步：[02-Stack Queue and Hashing (栈队列与哈希)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/02-Stack%20Queue%20and%20Hashing%20(栈队列与哈希).md)
