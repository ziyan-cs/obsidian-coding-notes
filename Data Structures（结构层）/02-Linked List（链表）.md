## 一页速览

- **链表**：由一个个节点通过指针连接起来
- **核心特点**：内存不要求连续
- **访问方式**：不能像数组那样随机下标访问
- **核心优势**：插入、删除灵活
- **核心限制**：查找慢、不能直接按下标取值
- **常见操作**：遍历、头插、尾插、删除节点、反转链表
- **最常错**：指针断链、漏判空、头节点处理错、`next` 顺序写反

## 一、先总后分：这一节到底学什么

### 1. 链表解决什么问题

- 数组虽然访问快，但长度固定
- 中间插入、删除元素时不方便
- 链表更适合频繁插入和删除的场景

### 2. 链表最核心看什么

- 节点怎么定义
- 节点之间怎么连接
- 怎么遍历
- 怎么插入和删除

### 3. 当前阶段先抓主线

- 会定义单链表节点
- 会画出 `next` 指向关系
- 会写最基本的遍历
- 会做头插、删除、反转这类基础题

---

## 二、链表是什么

### 1. 基本理解

- 链表由多个节点组成
- 每个节点存两部分：
    - 数据
    - 指向下一个节点的指针

```cpp
struct ListNode {
	int val;
	ListNode* next;
};
```

### 2. 怎么理解这个定义

- `val`：当前节点保存的数据
- `next`：指向下一个节点
- 最后一个节点的 `next` 一般是 `nullptr`

### 3. 当前阶段先记一句话

- **链表 = 一串靠指针连起来的节点**

---

## 三、链表节点的定义与初始化

### 1. 最基础定义

```cpp
struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) {
		val = x;
		next = nullptr;
	}
};
```

### 2. 创建单个节点

```cpp
ListNode* node = new ListNode(10);
```

### 3. 手动连接两个节点

```cpp
ListNode* a = new ListNode(1);
ListNode* b = new ListNode(2);
a->next = b;
```

### 4. 当前阶段怎么理解

- `a` 指向第一个节点
- `a->next` 指向第二个节点
- 第二个节点再指向后面节点，就形成链

---

## 四、链表和数组的区别

|对比项|数组|链表|
|---|---|---|
|内存|连续|不一定连续|
|访问|可随机访问|要顺着指针找|
|插入删除|通常较麻烦|通常更灵活|
|额外开销|少|每个节点多一个指针|

### 当前阶段先记结论

- **查找快**想数组
- **插删多**想链表

---

## 五、链表的遍历

### 1. 最基础写法

```cpp
ListNode* cur = head;
while (cur != nullptr) {
	cout << cur->val << '\\n';
	cur = cur->next;
}
```

### 2. 怎么理解

- `cur` 一开始指向头节点
- 每次处理当前节点后，往后走一步
- 直到走到 `nullptr`

### 3. 当前阶段先记

- **遍历链表最常见写法：`while (cur != nullptr)`**

---

## 六、链表最常见操作

### 1. 头插

```cpp
ListNode* node = new ListNode(10);
node->next = head;
head = node;
```

- 新节点先指向原头节点
- 再让 `head` 指向新节点

### 2. 删除下一个节点

```cpp
cur->next = cur->next->next;
```

- 本质是跳过中间节点
- 真实题目里通常还要先保存并释放被删节点

### 3. 尾插先有印象

- 找到最后一个节点
- 让最后一个节点的 `next` 指向新节点

### 4. 查找某个值

```cpp
ListNode* cur = head;
while (cur != nullptr) {
	if (cur->val == target) {
		break;
	}
	cur = cur->next;
}
```

---

## 七、头节点怎么理解

### 1. 什么是头节点

- `head` 是指向链表第一个节点的指针
- 它本身不是整个链表的数据，而是入口

### 2. 为什么头节点容易出错

- 插入第一个节点时可能会改 `head`
- 删除第一个节点时也可能会改 `head`

### 3. 当前阶段先记结论

- **链表题很多难点，本质都在头节点处理**

---

## 八、虚拟头节点先有印象

### 1. 为什么要有虚拟头节点

- 有些操作对第一个节点和其他节点处理不统一
- 加一个“假头”可以统一逻辑

```cpp
ListNode* dummy = new ListNode(0);
dummy->next = head;
```

### 2. 当前阶段怎么理解

- `dummy` 不存真正业务数据
- 它只是帮你更稳定地处理链表操作

### 3. 做题结论

- 删除节点、插入节点时，虚拟头节点很常用

---

## 九、链表反转先有印象

### 1. 反转链表最基础思路

- 让每个节点的 `next` 反过来指向前一个节点

### 2. 三指针写法

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

### 3. 当前阶段先记

- **反转链表的核心不是背代码，而是保存好下一个节点**

---

## 十、链表与函数

### 1. 链表常通过指针传参

```cpp
void printList(ListNode* head) {
	ListNode* cur = head;
	while (cur != nullptr) {
		cout << cur->val << '\\n';
		cur = cur->next;
	}
}
```

### 2. 为什么经常返回新头节点

- 因为很多操作可能会改变 `head`
- 比如头插、删除头节点、反转链表

```cpp
ListNode* reverseList(ListNode* head) {
	...
	return newHead;
}
```

### 3. 当前阶段先记

- **链表函数题里，先想：这次操作会不会改头节点？**

---

## 十一、当前阶段最常见写法

### 1. 节点定义模板

```cpp
struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) {
		val = x;
		next = nullptr;
	}
};
```

### 2. 遍历模板

```cpp
ListNode* cur = head;
while (cur != nullptr) {
	cout << cur->val << '\\n';
	cur = cur->next;
}
```

### 3. 头插模板

```cpp
ListNode* node = new ListNode(x);
node->next = head;
head = node;
```

### 4. 反转模板

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

---

## 十二、现代 C++ 的基础习惯

### 1. 先画图再写代码

- 链表题最怕脑子里没有指针方向
- 先画节点和箭头，能少很多错

### 2. 改指针前先想会不会断链

- 特别是删除和反转时
- 顺序写错就容易丢链表

### 3. 优先判空

- `head == nullptr`
- `cur == nullptr`
- `cur->next == nullptr`
- 这些判断很常见

### 4. 以后会接触

- 双链表
- 循环链表
- STL 里的 `list`
- 但单链表基础必须先稳

---

## 十三、高频易错点

1. `head` 是入口，不是普通节点变量
2. 遍历要写 `cur = cur->next`，不然容易死循环
3. 操作 `cur->next` 前要先判空
4. 删除节点时别把链断掉
5. 反转链表时要先保存下一个节点
6. 很多题目最后要返回新的 `head`
7. 头节点变化是链表题最大高频坑
8. 对象用 `.`，节点指针常用 `->`

---

## 十四、最简模板

### 节点定义模板

```cpp
struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) {
		val = x;
		next = nullptr;
	}
};
```

### 遍历模板

```cpp
ListNode* cur = head;
while (cur != nullptr) {
	cout << cur->val << '\\n';
	cur = cur->next;
}
```

### 头插模板

```cpp
ListNode* node = new ListNode(x);
node->next = head;
head = node;
```

### 反转模板

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

<aside> 📌

这一节真正要掌握的，不只是“链表是什么”，而是看到代码时能立刻判断：**当前指针指向谁、下一步会不会断链、这次操作会不会改头节点。**

</aside>