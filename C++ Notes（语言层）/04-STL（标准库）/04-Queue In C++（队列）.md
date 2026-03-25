#cpp #stl #queue

## ⚡ TL;DR（快速决策）

- 需要“先进先出” → `queue`
- 只关心队头和队尾 → `front()` / `back()`
- 频繁尾部加入、头部取出 → `push()` / `pop()`
- 需要遍历全部元素或随机访问 → `queue` 往往不合适
- 层序遍历、BFS、任务排队模型 → 第一反应先想队列

## 🧩 Core Idea（核心本质）

- **本质**：`queue` 是先进先出（FIFO）的容器适配器
- **关键机制**：最早进入队列的元素，最先被取出
- **核心优势**：天然适合处理“按进入顺序消费”的问题，比如分层扩展、任务调度和等待队列

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础定义

```cpp
queue<int> q;
```

### 2. 入队 / 出队 / 访问队头队尾

```cpp
q.push(10);
q.push(20);
cout << q.front() << '\n';
cout << q.back() << '\n';
q.pop();
```

### 3. 判空与大小

```cpp
cout << q.empty() << '\n';
cout << q.size() << '\n';
```

### 4. 循环出队

```cpp
while (!q.empty()) {
	cout << q.front() << '\n';
	q.pop();
}
```

### 5. BFS 基础模板

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

### 6. 二叉树层序遍历基础

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

### 7. 用 `deque` / `vector` 理解队列模型

```cpp
deque<int> q;
q.push_back(10);
q.push_back(20);
cout << q.front() << '\n';
q.pop_front();
```

## ⚠️ Pitfalls（高频错误）

- 空队列不能直接调用 `front()`、`back()` 或 `pop()`
- `pop()` 没有返回值，不能写成 `x = q.pop()`
- 以为 `queue` 可以随机访问，其实它不提供下标接口
- 以为 `queue` 可以直接遍历，其实它不提供普通迭代接口
- BFS 里忘记标记访问状态，容易重复入队
- 层序遍历里把“当前层”和“整个队列”概念混在一起，逻辑容易乱
- `queue` 是容器适配器，不是完整顺序容器
- 需要双端操作时，`queue` 往往不是最优选择，应考虑 `deque`

## 🚀 Performance / Tips（性能优化）

- `queue` 接口很轻，适合纯 FIFO 场景
- 一看到“按顺序处理等待中的元素”，就应该想到队列
- BFS 题里，真正关键的不只是队列，而是“何时入队、何时标记”
- 如果题目需要双端入队 / 出队，优先考虑 `deque`
- 刷题里常见的不是“队列操作难”，而是“什么时候该想到队列模型”

## 🧪 Common Scenarios（常见使用场景）

- **图的 BFS**：分层扩展节点
- **二叉树层序遍历**：从上到下逐层处理
- **任务调度 / 等待队列**：先来先服务
- **最短步数问题**：无权图 / 网格 BFS
- **消息处理模型**：按到达顺序消费数据

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
	cout << q.front() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `queue` 的核心不是“存一批数据”，而是它强制你按 **先进先出** 的顺序处理元素，这正好对应 BFS、层序遍历和各种排队消费模型。