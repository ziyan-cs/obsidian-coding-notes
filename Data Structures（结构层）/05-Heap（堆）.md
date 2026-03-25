#cpp #data-structure #heap

## ⚡ TL;DR（快速决策）

- 需要动态维护当前最值 → 堆
- 反复取最大值 → 大根堆
- 反复取最小值 → 小根堆
- 只关心堆顶最优元素，不关心整体有序 → 堆很合适
- Top K、最短路、调度、合并有序流 → 第一反应先想堆

## 🧩 Core Idea（核心本质）

- **本质**：堆是一种满足局部有序性质的完全二叉树
- **关键机制**：父节点和子节点满足固定大小关系，但整棵树不是全局有序
- **核心优势**：可以高效维护“当前最优元素”，适合动态最值问题

## 🔧 Usage Patterns（可复用代码模板）

### 1. 大根堆

```cpp
priority_queue<int> pq;
pq.push(3);
pq.push(1);
pq.push(5);
cout << pq.top() << '\n';
```

### 2. 小根堆

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
pq.push(3);
pq.push(1);
pq.push(5);
cout << pq.top() << '\n';
```

### 3. 取堆顶并弹出

```cpp
int x = pq.top();
pq.pop();
```

### 4. 判空与大小

```cpp
cout << pq.empty() << '\n';
cout << pq.size() << '\n';
```

### 5. 存 `pair`

```cpp
priority_queue<pair<int, int>> pq;
pq.push({5, 2});
pq.push({3, 7});
cout << pq.top().first << ' ' << pq.top().second << '\n';
```

### 6. Top K 基础模型

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
for (int x : nums) {
	pq.push(x);
	if (pq.size() > k) pq.pop();
}
```

### 7. 合并多个有序流先有印象

```cpp
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
pq.push({value, index});
```

### 8. Dijkstra 先有印象

```cpp
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
pq.push({0, start});
```

## ⚠️ Pitfalls（高频错误）

- 默认 `priority_queue<int>` 是大根堆，不是小根堆
- 堆只保证堆顶最优，不保证整体有序
- `pop()` 没有返回值，不能写成 `x = pq.pop()`
- 空堆不能直接 `top()` 或 `pop()`
- `pair` 默认先比较 `first`，再比较 `second`
- 自定义比较器写反时，堆序会完全反掉
- 需要删除任意元素或查找任意元素时，堆通常不是最优结构
- 把“优先队列”和“排序结果”混为一谈，会导致理解偏差

## 🚀 Performance / Tips（性能优化）

- 堆适合“动态维护最值”，不适合“拿到整体有序结果”
- Top K 问题常用“小根堆维护前 k 大”
- 只要需求是“反复取当前最优”，就应该优先想到堆
- 需要更灵活的查找 / 删除时，可以考虑 `set` / `multiset`
- 刷题里很多堆题本质是：

```cpp
不断插入
不断取堆顶
维护一个最优候选集合
```

## 🧪 Common Scenarios（常见使用场景）

- **Top K 问题**：第 k 大、前 k 大、前 k 小
- **最短路**：Dijkstra
- **任务调度**：每次选最优任务
- **数据流最值维护**：动态插入并实时取最优
- **合并多个有序序列**：每次取当前最小值
- **贪心 + 数据结构**：反复维护候选最优解

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	priority_queue<int> pq;
	pq.push(3);
	pq.push(1);
	pq.push(5);

	cout << pq.top() << '\n';
	pq.pop();
	cout << pq.top() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 堆的核心不是“像树一样存元素”，而是利用 **局部有序 + 堆顶最优** 的性质，持续高效地维护“当前最值得先处理的元素”。