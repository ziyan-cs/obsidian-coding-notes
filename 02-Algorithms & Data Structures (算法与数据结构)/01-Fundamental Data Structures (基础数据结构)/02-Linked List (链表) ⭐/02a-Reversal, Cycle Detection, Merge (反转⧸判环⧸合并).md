---
tags:
  - cs/algorithm
status: 🌱
---

# Reversal, Cycle Detection, Merge — 反转⧸判环⧸合并

> [!important] **核心考点**：链表反转的迭代与递归实现、环检测 Floyd 算法、有序链表合并、链表归并排序


```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
```

## 反转链表（迭代）

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

## 环检测（Floyd 判圈）

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

## 合并两个有序链表

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

## 关联笔记

- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Hash Table (哈希表)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)
