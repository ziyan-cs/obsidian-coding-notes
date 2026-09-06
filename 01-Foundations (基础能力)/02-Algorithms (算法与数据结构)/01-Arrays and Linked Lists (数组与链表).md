---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# 30 秒回答

**核心结论**：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

# Arrays and Two Pointers (数组与双指针)

> [!note] 本节重点：核心考点：双指针的四种模式、滑动窗口、前缀和

# 双指针四种模式

## 模式一：对撞指针（左右夹逼）

两指针从两端向中间移动，适合**有序数组**的搜索问题：

```cpp
// 有序数组两数之和
vector<int> twoSum(vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;
    while (l < r) {
        int s = nums[l] + nums[r];
        if      (s == target) return {l, r};
        else if (s < target)  l++;
        else                  r--;
    }
    return {};
}
```

典型题：Two Sum II、三数之和、盛最多水的容器、回文判断。

## 模式二：快慢指针

两指针同向，速度不同，用于**链表环检测、找中点**（详见链表章节）。

## 模式三：滑动窗口

维护一个可变长度的窗口 `[l, r]`，右指针扩张，左指针收缩：

```cpp
// 长度最小的子数组（子数组和 >= target）
int minSubArrayLen(int target, vector<int>& nums) {
    int l = 0, total = 0, res = INT_MAX;
    for (int r = 0; r < (int)nums.size(); r++) {
        total += nums[r];
        while (total >= target) {
            res = min(res, r - l + 1);
            total -= nums[l++];
        }
    }
    return res == INT_MAX ? 0 : res;
}
```

**无重复字符的最长子串（变长窗口 + 哈希）：**

```cpp
// 无重复字符的最长子串
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> seen;
    int l = 0, res = 0;
    for (int r = 0; r < (int)s.size(); r++) {
        if (seen.count(s[r]) && seen[s[r]] >= l)
            l = seen[s[r]] + 1;
        seen[s[r]] = r;
        res = max(res, r - l + 1);
    }
    return res;
}
```

## 模式四：快速分区（原地操作）

```cpp
// 颜色分类 0/1/2，O(n) 时间 O(1) 空间
void sortColors(vector<int>& nums) {
    int lo = 0, mid = 0, hi = (int)nums.size() - 1;
    while (mid <= hi) {
        if      (nums[mid] == 0) swap(nums[lo++], nums[mid++]);
        else if (nums[mid] == 1) mid++;
        else                     swap(nums[mid], nums[hi--]);
    }
}
```

# 前缀和

```cpp
// 构建前缀和，O(1) 区间查询
vector<int> prefix(nums.size() + 1, 0);
for (int i = 0; i < (int)nums.size(); i++)
    prefix[i + 1] = prefix[i] + nums[i];

// 区间 [l, r] 的和
int rangeSum = prefix[r + 1] - prefix[l];
```

二维前缀和、差分数组是进阶变体，用于范围加减操作。

---

# 关联笔记

- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)

---

# Linked List Patterns (链表反转判环与合并)

> [!note] 本节重点：核心考点：链表反转的迭代与递归实现、环检测 Floyd 算法、有序链表合并、链表归并排序

```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
```

# 反转链表（迭代）

```cpp
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr) {
        ListNode* nxt = curr->next;
        curr->next = prev;
        prev = curr;
        curr = nxt;
    }
    return prev;
}
```

**反转区间 [left, right]（92题）：** 找到 left-1 位置的节点，断开后反转子链表，再拼接。

# 环检测（Floyd 判圈）

```cpp
bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// 找环入口
ListNode* detectCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            slow = head;
            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }
            return slow;  // 入口节点
        }
    }
    return nullptr;
}
```

**原理：** 设链表头到环入口距离为 a，环长为 b。相遇时 slow 走了 a+k，fast 走了 a+k+nb（n 圈），因 fast=2×slow，所以 nb=a+k，即从相遇点再走 a 步回到入口。

# 合并两个有序链表

```cpp
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* curr = &dummy;
    while (l1 && l2) {
        if (l1->val <= l2->val) { curr->next = l1; l1 = l1->next; }
        else                    { curr->next = l2; l2 = l2->next; }
        curr = curr->next;
    }
    curr->next = l1 ? l1 : l2;
    return dummy.next;
}

// 合并 K 个有序链表（最小堆）
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b){ return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);
    for (auto node : lists)
        if (node) pq.push(node);

    ListNode dummy(0);
    ListNode* curr = &dummy;
    while (!pq.empty()) {
        curr->next = pq.top(); pq.pop();
        curr = curr->next;
        if (curr->next) pq.push(curr->next);
    }
    return dummy.next;
}
```

**合并 K 个有序链表：** 用最小堆，初始将 K 个链表头入堆，每次取最小节点，将其 next 入堆，O(N log K)。

---

# 关联笔记 · 延伸要点 2
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)

---

# Fast and Slow Pointers (快慢指针)

> [!note] 本节重点：核心考点：快慢指针找中点/倒数第 K 个/环入口、回文链表判断、Floyd 判环算法

|问题|技巧|
|---|---|
|链表中点|快慢指针，fast 到尾时 slow 在中间|
|倒数第 K 个节点|fast 先走 K 步，然后同速，fast 到尾时 slow 即目标|
|判断回文链表|找中点 + 反转后半段 + 比较|
|环入口|Floyd 算法（见上）|

```cpp
// 找链表中点（slow 停在中间偏左）
ListNode* findMiddle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 倒数第 K 个节点
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;
    for (int i = 0; i <= n; i++) fast = fast->next;
    while (fast) { slow = slow->next; fast = fast->next; }
    slow->next = slow->next->next;
    return dummy.next;
}
```

---

# 关联笔记 · 延伸要点 3
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Monotonic Stack (单调栈)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)

# 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

# 常见误区

- 把 **01-Arrays and Linked Lists (数组与链表)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Arrays and Linked Lists (数组与链表)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
