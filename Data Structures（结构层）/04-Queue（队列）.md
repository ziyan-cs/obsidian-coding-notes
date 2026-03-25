#cpp #data-structure #queue

## ⚡ TL;DR（快速决策）

- 需要“先进先出”处理顺序 → 队列
- 只关心最早进入、最早该处理的元素 → 优先想队列
- 题目出现层序遍历、BFS、任务排队、最短步数扩展 → 先想到队列模型
- 需要随机访问或中间删除 → 队列通常不合适
- 队列题的关键通常不在结构，而在“入队 / 出队时机”和“访问标记时机”

## 🧩 Core Idea（核心本质）

- **本质**：队列是先进先出（FIFO）的线性结构
- **关键机制**：最早进入队列的元素，最先被访问或移除
- **核心优势**：天然适合按时间顺序、层次顺序或等待顺序处理问题

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础队列操作

```cpp
queue<int> q;
q.push(1);
q.push(2);
cout << q.front() << '\n';
q.pop();
```

### 2. 判空后处理队头

```cpp
if (!q.empty()) {
	cout << q.front() << '\n';
}
```

### 3. 循环出队

```cpp
while (!q.empty()) {
	cout << q.front() << '\n';
	q.pop();
}
```

### 4. BFS 基础模型

```cpp
queue<int> q;
q.push(start);
vis[start] = true;

while (!q.empty()) {
	int u = q.front();
	q.pop();
	for (int v : g[u]) {
		if (!vis[v]) {
			vis[v] = true;
			q.push(v);
		}
	}
}
```

### 5. 二叉树层序遍历

```cpp
queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
	TreeNode* cur = q.front();
	q.pop();
	if (cur->left) q.push(cur->left);
	if (cur->right) q.push(cur->right);
}
```

### 6. 分层处理当前层节点数

```cpp
while (!q.empty()) {
	int sz = q.size();
	while (sz--) {
		int u = q.front();
		q.pop();
	}
}
```

### 7. 用 `deque` 模拟队列

```cpp
deque<int> q;
q.push_back(1);
q.push_back(2);
cout << q.front() << '\n';
q.pop_front();
```

### 8. 最短步数问题先有印象

```cpp
queue<pair<int, int>> q;
q.push({start, 0});
```

## ⚠️ Pitfalls（高频错误）

- 空队列不能直接 `front()`、`back()` 或 `pop()`
- `pop()` 没有返回值，不能写成 `x = q.pop()`
- BFS 里忘记标记访问状态，容易重复入队
- “入队时标记” 和 “出队时标记” 混乱，会影响复杂度甚至正确性
- 层序遍历里把“当前层元素个数”和“整个队列长度”混在一起，逻辑容易乱
- 队列不支持随机访问，也不提供普通下标接口
- 需要双端进出时，普通队列可能不是最优，应考虑 `deque`
- 很多队列题真正难点是“什么时候入队”，不是“怎么写 `push/pop`”

## 🚀 Performance / Tips（性能优化）

- 只要问题本质是“按进入顺序处理等待中的元素”，就该先想到队列
- BFS 题里，优先先想清楚：

```cpp
何时入队
何时标记
何时更新答案
```

- 分层问题优先用 `q.size()` 固定当前层节点数
- 需要双端操作时，优先考虑 `deque`
- 队列强项是维护处理顺序，不是做复杂查找

## 🧪 Common Scenarios（常见使用场景）

- **图的 BFS**：按层扩展节点
- **二叉树层序遍历**：从上到下逐层处理
- **最短步数问题**：无权图 / 网格最短路
- **任务调度 / 等待队列**：先来先服务
- **消息处理模型**：按到达顺序消费数据
- **多源 BFS**：多个起点一起入队扩展

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	queue<int> q;
	q.push(1);
	q.push(2);

	cout << q.front() << '\n';
	q.pop();

	if (!q.empty()) {
		cout << q.front() << '\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 队列的核心不是“存一批元素”，而是利用 **先进先出** 的顺序持续处理“最早进入、最早该处理”的状态，这正好对应 BFS、层序遍历、最短步数扩展和排队模型。