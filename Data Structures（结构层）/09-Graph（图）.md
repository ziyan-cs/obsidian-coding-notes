#cpp #data-structure #graph

## ⚡ TL;DR（快速决策）

- 需要表示“点和点之间的关系” → 图
- 题目出现连接、路径、连通性、依赖、网络 → 先想到图模型
- 需要遍历所有可达点 → DFS / BFS
- 需要最短步数 → 无权图优先 BFS
- 图题真正关键通常不在“图是什么”，而在“怎么建图 + 用什么遍历 / 算法”

## 🧩 Core Idea（核心本质）

- **本质**：图是由节点（顶点）和边（连接关系）组成的数据结构
- **关键机制**：关系不再是线性的，也不一定有树那样的层次结构，因此访问时通常要显式记录邻接关系和访问状态
- **核心优势**：可以统一表达连接、依赖、路径、网络、状态转移等大量问题

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础邻接表

```cpp
vector<int> g[1005];
```

### 2. 有向图加边

```cpp
g[u].push_back(v);
```

### 3. 无向图加边

```cpp
g[u].push_back(v);
g[v].push_back(u);
```

### 4. 邻接矩阵先有印象

```cpp
int g[105][105] = {0};
g[u][v] = 1;
```

### 5. DFS 模板

```cpp
bool vis[1005] = {false};

void dfs(int u) {
	vis[u] = true;
	for (int v : g[u]) {
		if (!vis[v]) {
			dfs(v);
		}
	}
}
```

### 6. BFS 模板

```cpp
bool vis[1005] = {false};

void bfs(int start) {
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
}
```

### 7. 统计连通块

```cpp
int cnt = 0;
for (int i = 1; i <= n; ++i) {
	if (!vis[i]) {
		++cnt;
		dfs(i);
	}
}
```

### 8. 无权图最短路先有印象

```cpp
queue<int> q;
vector<int> dist(n + 1, -1);
q.push(start);
dist[start] = 0;
while (!q.empty()) {
	int u = q.front();
	q.pop();
	for (int v : g[u]) {
		if (dist[v] == -1) {
			dist[v] = dist[u] + 1;
			q.push(v);
		}
	}
}
```

## ⚠️ Pitfalls（高频错误）

- 图不是树，可能有环，遍历时通常必须判重
- 无向图加边通常要加两次，有向图只加一次
- 邻接表和邻接矩阵不要混，适用场景不同
- `vis` 标记时机写错，容易重复访问甚至死循环
- BFS 和 DFS 思路不要混，一个是分层扩展，一个是一路走深
- 普通图不保证层次结构，不能套树的直觉硬想
- 图题第一步常常不是写算法，而是先想“节点和边分别表示什么”
- 编号从 `0` 开始还是从 `1` 开始，必须先统一清楚

## 🚀 Performance / Tips（性能优化）

- 稀疏图优先邻接表，稠密图才更可能考虑邻接矩阵
- 一看到“最短步数”先想 BFS，而不是 DFS
- 一看到“连通性 / 能否到达 / 组件个数”先想 DFS 或 BFS
- 建图时优先先明确：

```cpp
节点表示什么
边表示什么
有向还是无向
是否带权
```

- 刷题里很多图题，本质是“建图 + 遍历”，算法本身并不复杂

## 🧪 Common Scenarios（常见使用场景）

- **连通性判断**：两个点是否连通
- **路径搜索**：是否存在路径 / 找到一条路径
- **连通块统计**：图里有几个部分
- **最短步数**：无权图 BFS
- **依赖关系**：课程表、任务依赖、拓扑排序前置问题
- **网络 / 城市 / 网格抽象**：把现实关系建成图

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> g[1005];
bool vis[1005] = {false};

void dfs(int u) {
	vis[u] = true;
	for (int v : g[u]) {
		if (!vis[v]) dfs(v);
	}
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n = 4;
	g[1].push_back(2);
	g[2].push_back(1);
	g[2].push_back(3);
	g[3].push_back(2);

	dfs(1);
	cout << vis[3] << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 图的核心不是“很多点连起来”，而是先把问题抽象成 **节点 + 边 + 关系类型**，然后用合适的存图方式和遍历算法去处理连通性、路径和依赖关系。