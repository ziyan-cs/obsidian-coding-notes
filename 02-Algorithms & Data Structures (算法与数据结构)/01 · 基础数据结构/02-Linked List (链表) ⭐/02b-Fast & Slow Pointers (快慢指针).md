---
tags:
  - algorithm
  - basics
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