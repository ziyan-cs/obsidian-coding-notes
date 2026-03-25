#cpp #stl #priority-queue

## ⚡ TL;DR（快速决策）

- 需要“每次取当前最大值” → 默认 `priority_queue`
- 需要“每次取当前最小值” → 小根堆写法
- 只关心堆顶最优元素，不关心整体有序 → `priority_queue`
- 需要频繁插入并反复取最值 → 优先队列很合适
- 需要遍历全部有序结果 → 通常边 `top()` 边 `pop()`，或改用排序 / `multiset`

## 🧩 Core Idea（核心本质）

- **本质**：`priority_queue` 是基于堆实现的容器适配器
- **关键机制**：每次只能高效访问并取出“当前优先级最高”的元素
- **核心优势**：插入和取最值都高效，特别适合动态维护最优元素

## 🔧 Usage Patterns（可复用代码模板）

### 1. 默认大根堆

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

### 3. 判空与大小

```cpp
cout << pq.empty() << '\n';
cout << pq.size() << '\n';
```

### 4. 循环取堆顶

```cpp
while (!pq.empty()) {
	cout << pq.top() << '\n';
	pq.pop();
}
```

### 5. 存 `pair`

```cpp
priority_queue<pair<int, int>> pq;
pq.push({5, 2});
pq.push({3, 7});
cout << pq.top().first << ' ' << pq.top().second << '\n';
```

### 6. 小根堆存 `pair`

```cpp
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
pq.push({5, 2});
pq.push({3, 7});
cout << pq.top().first << ' ' << pq.top().second << '\n';
```

### 7. Top K 基础模型

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
for (int x : nums) {
	pq.push(x);
	if (pq.size() > k) pq.pop();
}
```

### 8. Dijkstra / 调度类先有印象

```cpp
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
pq.push({0, start});
```

## ⚠️ Pitfalls（高频错误）

- 默认 `priority_queue<int>` 是大根堆，不是小根堆
- `pop()` 没有返回值，不能写成 `x = pq.pop()`
- 空优先队列不能直接 `top()` 或 `pop()`
- 以为优先队列里所有元素都是整体有序的，其实只保证堆顶最优
- 想遍历所有元素时，通常只能不断 `pop()`，会破坏原结构
- `pair` 的比较是先比 `first`，再比 `second`
- 自定义比较器写错时，堆顺序会完全反掉
- 需要删除任意元素时，`priority_queue` 通常不合适

## 🚀 Performance / Tips（性能优化）

- 动态维护最值时，优先队列通常比“每次排序”更高效
- Top K 问题常用“小根堆维护前 k 大”
- 最短路、任务调度、合并多个有序流时，优先队列非常高频
- 只要需求是“反复取当前最优”，就该先想到堆
- 需要更灵活删除 / 查找时，可以考虑 `multiset`

## 🧪 Common Scenarios（常见使用场景）

- **Top K 问题**：第 k 大 / 前 k 大元素
- **Dijkstra**：每次取当前最短距离节点
- **任务调度**：每次处理优先级最高任务
- **合并多个有序序列**：每次取当前最小值
- **数据流最值维护**：动态插入并实时取最优元素

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

	cout << pq.top() << '\\n';
	pq.pop();
	cout << pq.top() << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `priority_queue` 的核心不是“像队列一样存数据”，而是它能让你始终以较低代价拿到 **当前最优元素**，这正好对应 Top K、最短路、调度和动态最值维护。