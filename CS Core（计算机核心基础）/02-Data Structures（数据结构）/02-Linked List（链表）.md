#data-structure #linked-list #pointer #node #cpp

## ⚡ TL;DR（快速决策）

- 链表本质是：**通过指针把一个个节点串起来的线性结构**
- 一看到这些需求，要优先想到链表：
    - 需要频繁插入 / 删除节点
    - 题目重点在“连接关系”而不是随机访问
    - 反转链表、合并链表、判环、找中点
- 优势：结构修改灵活
- 弱点：不能随机访问，指针操作容易错
- 链表题核心不是值，而是：**指针怎么改**

## 🧩 Core Idea（核心本质）

- 每个节点通常包含：
    - `val`
    - `next`
- 双向链表还会多一个 `prev`
- 一句话理解：
    - **链表就是“节点靠指针连接”的结构。**
- 和数组最大不同：
    - 数组靠位置访问
    - 链表靠指针顺着走

## 🔧 Usage Patterns（可复用代码模板）

1. 节点定义

```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
```

1. 遍历链表

```cpp
for (ListNode* cur = head; cur; cur = cur->next) {
    cout << cur->val << ' ';
}
```

1. 头插法

```cpp
ListNode* node = new ListNode(5);
node->next = head;
head = node;
```

1. 反转链表

```cpp
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;
    while (cur) {
        ListNode* nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }
    return prev;
}
```

1. Dummy 节点

```cpp
ListNode dummy(0);
dummy.next = head;
```

## ⚠️ Pitfalls（高频错误）

- 改指针前没保存下一个节点
- 空指针解引用
- 头节点变化时没更新 `head`
- 把删除节点写成改值
- 快慢指针边界没处理好

## 🚀 Performance / Tips（性能优化）

- 遍历：$O(n)$
- 已知节点后插删：$O(1)$
- 查找第 k 个节点：$O(n)$
- 高频经验：
    - 链表题优先画图
    - 头节点问题优先想 dummy

## 🧪 Common Scenarios（常见使用场景）

- 反转链表
- 合并两个有序链表
- 删除倒数第 k 个节点
- 找中点 / 判环
- 双向链表配合缓存结构

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
int main() {
    ListNode* a = new ListNode(1);
    ListNode* b = new ListNode(2);
    a->next = b;
    for (ListNode* cur = a; cur; cur = cur->next) cout << cur->val << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **链表就是：通过指针连接节点、擅长结构修改但不擅长随机访问的线性结构。**