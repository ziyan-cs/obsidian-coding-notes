---
tags:
  - cs/algorithm
status: 🌱
---

> **核心考点**：快慢指针找中点/倒数第 K 个/环入口、回文链表判断、Floyd 判环算法


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

## 关联笔记

- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Monotonic Stack (单调栈)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03a-Monotonic%20Stack%20(单调栈).md)
- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Hash Table (哈希表)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/04-Hash%20Table%20(哈希表)%20⭐.md)
