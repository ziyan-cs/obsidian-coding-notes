#data-structure #linked-list #pointer #node #cpp

# 0. 核心

- **通过指针把一个个节点串起来的线性结构**
- 相关特征：
    - 频繁插入 / 删除节点
    - 重点在“连接关系”而不是随机访问
    - 反转链表、合并链表、判环、找中点
- 优势：结构修改灵活
- 代价：不能随机访问，指针操作容易错
- 遍历：$O(n)$
- 已知节点后插删：$O(1)$
- 查找第 k 个节点：$O(n)$
# 1. 高频技巧

- 节点定义

```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
```

-  遍历链表

```cpp
for (ListNode* cur = head; cur; cur = cur->next) {
    cout << cur->val << ' ';
}
```

-  头插法

```cpp
ListNode* node = new ListNode(5);
node->next = head;
head = node;
```

-  反转链表

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

1. Dummy（哨兵）节点

```cpp
ListNode dummy(0);
dummy.next = head;
```


# 3. 经典案例

### 3.1


### 3.1 判断否为回文



### 3.2 荷兰国旗问题（链表版）

