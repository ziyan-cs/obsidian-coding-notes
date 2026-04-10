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
// 时间 O(N) 空间 O(1)
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

```cpp
// 时间 O(N) 空间 O(N)
ListNode* reverse(ListNode* pre,ListNode* cur){
    if(cur == NULL) return pre;
    ListNode* temp = cur->next;
    cur->next = pre;
    return reverse(cur,temp);
}
ListNode* reverseList(ListNode* head) {
    return reverse(NULL, head);
}
```

1. Dummy（哨兵）节点

```cpp
ListNode dummy(0);
dummy.next = head;
```

# 3. 经典案例

### 3.1 判断否为回文

```cpp
bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;

    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
	
    ListNode* right = reverse(slow->next);
    ListNode* left = head;
    bool res = true;
	
    while (right) {
        if (left->val != right->val) {
            res = false;
            break;
        }
        left = left->next;
        right = right->next;
    }
	
    slow->next = reverse(right);
    return res;
}
```

### 3.2 荷兰国旗问题（链表版）

```cpp
ListNode* netherlandsFlag(ListNode* head, int pivot) {
    ListNode *sh = NULL, *st = NULL;
    ListNode *eh = NULL, *et = NULL;
    ListNode *bh = NULL, *bt = NULL;
		
    ListNode* cur = head;
    while (cur) {
        ListNode* next = cur->next;
        cur->next = NULL;
		
        if (cur->val < pivot) {
            if (!sh) sh = st = cur;
            else st = st->next = cur;
        } else if (cur->val == pivot) {
            if (!eh) eh = et = cur;
            else et = et->next = cur;
        } else {
            if (!bh) bh = bt = cur;
            else bt = bt->next = cur;
        }
        cur = next;
    }
		
    ListNode dummy(0);
    ListNode* p = &dummy;
		
    if (sh) { p->next = sh; p = st; }
    if (eh) { p->next = eh; p = et; }
    if (bh) { p->next = bh; }

    return dummy.next;
}
```

### 3.3 复制带随机指针的链表

```cpp
// rand：随机指针，可指向链表中任意节点，也可指向 null
class Node {
public:
    int value;
    Node* next;
    Node* rand;
    Node(int val) : value(val), next(nullptr), rand(nullptr) {}
};
	
//时间 O(N)，空间 O(N)
Node* copyRandomListHash(Node* head) {
    if (!head) return nullptr;
    unordered_map<Node*, Node*> map;
		
    // 第一次遍历：创建所有克隆节点，存入哈希表
    Node* cur = head;
    while (cur) {
        map[cur] = new Node(cur->value);
        cur = cur->next;
    }
		
    // 第二次遍历：设置next和rand指针
    cur = head;
    while (cur) {
        map[cur]->next = map[cur->next];
        map[cur]->rand = map[cur->rand];
        cur = cur->next;
    }
		
    return map[head];
}
```

```cpp
//时间 O(N)，空间 O(1)
Node* copyRandomList(Node* head) {
    if (!head) return nullptr;
		
    // 1. 插入克隆节点：原节点后紧跟克隆节点
    Node* cur = head;
    while (cur) {
        Node* clone = new Node(cur->value);
        clone->next = cur->next;
        cur->next = clone;
        cur = clone->next;
    }
		
    // 2. 设置克隆节点的rand指针
    cur = head;
    while (cur) {
        Node* clone = cur->next;
        clone->rand = cur->rand ? cur->rand->next : nullptr;
        cur = clone->next;
    }
		
    // 3. 拆分链表，分离原链表与克隆链表
    Node* newHead = head->next;
    cur = head;
    Node* cloneCur = newHead;
    while (cur) {
        cur->next = cloneCur->next;
        cur = cur->next;
        if (cur) cloneCur->next = cur->next;
        cloneCur = cloneCur->next;
    }
		
    return newHead;
}
```

### 3.4 相交链表

```cpp
// 找到第一个入环节点，如果无环，返回 null
Node* getLoopNode(Node* head) {
	if (!head || !head->next || !head->next->next) {
		return nullptr;
	}
	Node* slow = head->next;
	Node* fast = head->next->next;
	while (slow != fast) {
		if (!slow->next || !fast->next->next) {
			return nullptr;
		}
		slow = slow->next;
		fast = fast->next->next;
	}
	fast = head;
	while (slow != fast) {
		slow = sloe->next;
		fast = fast->next;
	}
	return fast;
}
// 如果都无环返回第一个相交节点，如果不相交返回 null
Node* noLoop(Node* head1, Node* head2) {
	if (!head1 || !head2) {
		return nullptr;
	}
	Node* cur1 = head1;
	Node* cur2 = head2;
	int cnt = 0;
	while (cur1->next) {
		cnt++;
		cur1 = cur1->next;
	}
	while (cur2->next) {
		cnt--;
		cur2 = cur2->next;
	}
	if (n > 0) {
		cur1 = head1;
		cur2 = head2;
	} else {
		cur1 = head2;
		cur2 = head1;
	}
	cnt = abs(cnt);
	while (cnt != 0) {
		n--;
		cur1 = cur1->next;
	}
	while (cur1 != cur2) {
		cur1 = cur1->next;
		cur2 = cur2->next;
	}
	return cur1;
}
```
