#cpp #data-structure #linked-list

## ⚡ TL;DR（快速决策）

- 频繁头插 / 中间插删，且不要求随机访问 → 链表
- 需要按下标 O(1) 访问 → 不适合链表，优先数组 / `vector`
- 题目出现“节点重连”“删除当前节点”“反转一段” → 先想到链表
- 处理链表题时，优先画节点关系图，不要只盯代码
- 大多数链表题难点不在结构本身，而在指针更新顺序

## 🧩 Core Idea（核心本质）

- **本质**：链表是用节点和指针把元素串起来的线性结构
- **关键机制**：每个节点保存值和下一个节点的位置，访问依赖指针跳转而不是连续内存
- **核心优势**：插入删除时不需要整体搬移元素，适合做结构重连类操作

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础单链表节点

```cpp
struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) : val(x), next(nullptr) {}
};
```

### 2. 遍历链表

```cpp
ListNode* cur = head;
while (cur != nullptr) {
	cout << cur->val << '\\n';
	cur = cur->next;
}
```

### 3. 头插法

```cpp
ListNode* node = new ListNode(5);
node->next = head;
head = node;
```

### 4. 删除下一个节点

```cpp
if (cur != nullptr && cur->next != nullptr) {
	ListNode* del = cur->next;
	cur->next = del->next;
	delete del;
}
```

### 5. 虚拟头结点（强烈推荐）

```cpp
ListNode* dummy = new ListNode(0);
dummy->next = head;
```

### 6. 反转链表

```cpp
ListNode* prev = nullptr;
ListNode* cur = head;
while (cur != nullptr) {
	ListNode* nxt = cur->next;
	cur->next = prev;
	prev = cur;
	cur = nxt;
}
head = prev;
```

### 7. 快慢指针找中点

```cpp
ListNode* slow = head;
ListNode* fast = head;
while (fast != nullptr && fast->next != nullptr) {
	slow = slow->next;
	fast = fast->next->next;
}
```

### 8. 带虚拟头结点删除目标值

```cpp
ListNode* dummy = new ListNode(0);
dummy->next = head;
ListNode* cur = dummy;
while (cur->next != nullptr) {
	if (cur->next->val == target) {
		ListNode* del = cur->next;
		cur->next = del->next;
		delete del;
	} else {
		cur = cur->next;
	}
}
head = dummy->next;
```

## ⚠️ Pitfalls（高频错误）

- 链表不能随机访问，`第 k 个节点` 通常只能顺着走
- 改指针前不先保存 `next`，很容易把链断掉
- 删除节点时只改了指针，忘了释放节点或遗漏边界
- 头结点参与插删时，不用虚拟头结点会让分类讨论变多
- 快慢指针题最容易把循环条件写错
- `cur`、`cur->next`、`cur->next->next` 这三层判空必须分清
- 反转链表本质是改边方向，不是改节点值
- 链表题里变量名混乱会直接导致思路崩掉

## 🚀 Performance / Tips（性能优化）

- 链表强项是插删，不是查找
- 写链表题优先用：

```cpp
prev / cur / nxt
```

- 头结点可能变化时，优先先想虚拟头结点
- 反转、合并、分段处理时，先画 3 到 4 个节点示意图再写代码
- 刷题里很多链表题都能转成“找前驱节点 + 重连指针”

## 🧪 Common Scenarios（常见使用场景）

- **删除指定节点 / 指定值**：重连前驱和后继
- **反转链表**：指针方向反转
- **合并两个有序链表**：双指针 + 重连
- **找中点 / 找环**：快慢指针
- **分隔链表 / 重排链表**：多段链表拼接
- **面试基础题**：虚拟头结点、反转、环检测

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) : val(x), next(nullptr) {}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	ListNode* head = new ListNode(1);
	head->next = new ListNode(2);
	head->next->next = new ListNode(3);

	ListNode* cur = head;
	while (cur != nullptr) {
		cout << cur->val << ' ';
		cur = cur->next;
	}
	cout << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 链表的核心不是“节点连起来”，而是理解 **访问靠指针跳转、修改靠重连关系**，所以真正的重点永远是指针更新顺序和边界处理。