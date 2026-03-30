#data-structure #linked-list #pointer #list-node #algorithm

## ⚡ TL;DR（快速决策）

- 链表（Linked List）本质是：**用节点和指针把一串数据连起来，而不是连续存放在数组里**
- 一看到这些特征，要优先想到链表：
    - 题目要求频繁插入 / 删除节点
    - 题目让你“反转链表”“合并链表”“删除倒数第 k 个节点”
    - 只给你头结点，要求原地修改结构
    - 涉及快慢指针、虚拟头结点、链表断开 / 重连
- 链表和数组的核心区别：
    - 数组：支持随机访问，插删中间元素代价高
    - 链表：不支持随机访问，但已知位置后插删很方便
- 链表题很多看起来花，但高频套路其实就这些：
    - **虚拟头结点**
    - **双指针 / 快慢指针**
    - **局部反转**
    - **断开 + 重连**
- 如果题目操作的是“节点之间的连接关系”，大概率就是链表题

## 🧩 Core Idea（核心本质）

- 链表由一个个节点组成，每个节点通常包含：
    - 当前值 `val`
    - 指向下一个节点的指针 `next`
- 单链表的访问方式不是下标，而是：
    - 从头节点开始
    - 顺着 `next` 一个个往后走
- 所以链表的核心不是“找第几个元素”，而是：
    - **当前节点是谁**
    - **前一个节点是谁**
    - **下一个节点是谁**
- 链表题的本质往往是对指针关系进行维护：
    - 谁连向谁
    - 哪里断开
    - 哪里接回去
- 一句话理解：
    - **链表题本质上是在改指针，不是在改值。**
- 常见分类：
    - 单链表
    - 双链表
    - 循环链表
- 算法题里最常见的是：**单链表**

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础链表节点定义

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;

    ListNode(int x) : val(x), next(nullptr) {}
};
```

要点：

- 大多数链表题都基于这个结构
- `next` 指向下一个节点，没有就为 `nullptr`

1. 遍历链表

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

void printList(ListNode* head) {
    ListNode* cur = head;
    while (cur != nullptr) {
        cout << cur->val << ' ';
        cur = cur->next;
    }
    cout << '\\n';
}
```

要点：

- 链表遍历靠 `cur = cur->next`
- 结束条件通常是 `cur == nullptr`

1. 头插法

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

int main() {
    ListNode* head = nullptr;

    for (int x : {3, 2, 1}) {
        ListNode* node = new ListNode(x);
        node->next = head;
        head = node;
    }

    for (ListNode* cur = head; cur; cur = cur->next) {
        cout << cur->val << ' ';
    }
    return 0;
}
```

要点：

- 新节点先指向原头节点
- 再更新 `head`
- 头插法常导致顺序反转

1. 尾插法

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

int main() {
    ListNode* head = new ListNode(1);
    ListNode* tail = head;

    for (int x : {2, 3, 4}) {
        tail->next = new ListNode(x);
        tail = tail->next;
    }

    for (ListNode* cur = head; cur; cur = cur->next) {
        cout << cur->val << ' ';
    }
    return 0;
}
```

要点：

- `tail` 始终指向尾节点
- 尾插法能保持原顺序

1. 删除指定值节点（虚拟头结点版）

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* removeElements(ListNode* head, int target) {
    ListNode dummy(0);
    dummy.next = head;

    ListNode* cur = &dummy;
    while (cur->next != nullptr) {
        if (cur->next->val == target) {
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }

    return dummy.next;
}
```

要点：

- **虚拟头结点** 是链表题最强套路之一
- 它能统一“删除头节点”和“删除普通节点”的写法

1. 反转链表

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;

    while (cur != nullptr) {
        ListNode* nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }

    return prev;
}
```

要点：

- 反转链表是链表题核心母题
- 牢记三件事：
    - 先存下一个节点
    - 再反转指向
    - 再整体推进

1. 快慢指针找中点

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}
```

要点：

- `slow` 一次走一步，`fast` 一次走两步
- 快慢指针高频用于：
    - 找中点
    - 找环
    - 找倒数第 k 个节点

1. 删除倒数第 k 个节点

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;

    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    for (int i = 0; i < n; ++i) {
        fast = fast->next;
    }

    while (fast->next != nullptr) {
        fast = fast->next;
        slow = slow->next;
    }

    slow->next = slow->next->next;
    return dummy.next;
}
```

要点：

- 双指针保持间距
- 这是“快慢指针 + 虚拟头结点”的组合模板

1. 判断链表是否有环

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }

    return false;
}
```

要点：

- 经典 Floyd 判环
- 只要有环，快慢指针最终会相遇

1. 合并两个有序链表

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (l1 != nullptr && l2 != nullptr) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }

    tail->next = (l1 != nullptr ? l1 : l2);
    return dummy.next;
}
```

要点：

- 合并过程和归并排序里的 merge 思想很像
- `tail` 负责始终接在结果链表末尾

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：修改指针前没保存下一个节点
    - 反转链表时尤其容易把后面链断掉
- 最常见错误 2：头节点特殊处理写崩
    - 所以高频建议：**优先用虚拟头结点**
- 最常见错误 3：`nullptr` 判断漏掉
    - 链表题里空指针判断特别关键
- 最常见错误 4：快慢指针循环条件写错
    - 常见写法：`while (fast != nullptr && fast->next != nullptr)`
- 最常见错误 5：把“改值”当成“改结构”
    - 题目很多时候要你调整节点连接关系，不是只交换数值
- 最常见错误 6：断开后没接回去
    - 链表题里“断链”和“重连”必须成对考虑
- 最常见错误 7：忘记返回新的头节点
    - 特别是删除 / 反转后，头节点往往已经变了

## 🚀 Performance / Tips（性能优化）

- 链表常见复杂度记忆：
    - 遍历：$O(n)$
    - 已知节点后插入 / 删除：$O(1)$
    - 按下标访问第 k 个节点：$O(n)$
- 所以链表的短板是：
    - **不支持随机访问**
- 链表的优势是：
    - **结构调整灵活**
- 实战建议：
    - 一看到头节点操作，先想 `dummy`
    - 一看到中点 / 环 / 倒数第 k 个，先想快慢指针
    - 一看到反转，就先写 `prev / cur / nxt`
- 链表题想清楚这 3 件事，代码就稳很多：
    1. 当前节点是谁？
    2. 前驱节点是谁？
    3. 改完之后链还连得上吗？
- 如果题目只是查询很多次，不频繁插删，数组往往更合适

## 🧪 Common Scenarios（常见使用场景）

- **基础操作**：遍历、插入、删除、查找
- **结构调整**：反转链表、局部反转、重排链表
- **双指针问题**：中点、倒数第 k 个、判环
- **合并问题**：合并两个有序链表、归并链表
- **经典面试题**：
    - 反转链表
    - 删除倒数第 N 个节点
    - 判断是否有环
    - 找环入口
    - 合并 K 个链表

## 🆚 Linked List vs 其他常见结构

- **vs Array（数组）**
    - 数组：随机访问强，插删中间慢
    - 链表：随机访问弱，结构调整灵活
- **vs Vector**
    - `vector` 底层连续空间，缓存友好
    - 链表节点分散，访问局部性较差
- **vs Stack / Queue**
    - 栈和队列是“操作受限的线性结构”
    - 链表是一种实现这些结构的底层方式之一

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;

    while (cur != nullptr) {
        ListNode* nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }

    return prev;
}

int main() {
    ListNode* a = new ListNode(1);
    ListNode* b = new ListNode(2);
    ListNode* c = new ListNode(3);
    a->next = b;
    b->next = c;

    ListNode* head = reverseList(a);
    for (ListNode* cur = head; cur; cur = cur->next) {
        cout << cur->val << ' ';
    }

    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **链表就是：用指针把节点串起来，核心难点不在值本身，而在节点连接关系的维护。**