#data-structure #union-find #dsu #disjoint-set #cpp

## ⚡ TL;DR（快速决策）

- 并查集本质是：**维护若干不相交集合的合并与查询**
- 一看到这些需求，要优先想到并查集：
    - 动态连通性判断
    - 合并两个集合
    - 判断两个元素是否属于同一组
- 核心操作只有两个：
    - `find(x)`
    - `union(x, y)`
- 它特别适合“是否同组”，不擅长给你具体路径

## 🧩 Core Idea（核心本质）

- 每个集合通常用一棵树表示
- 树根是该集合的代表元
- 一句话理解：
    - **并查集就是专门管理“元素分组关系”的结构。**
- 两个高频优化：
    - 路径压缩
    - 按秩 / 按大小合并

## 🔧 Usage Patterns（可复用代码模板）

1. 基础模板

```cpp
vector<int> fa;

int find(int x) {
    return fa[x] == x ? x : fa[x] = find(fa[x]);
}

void unite(int x, int y) {
    x = find(x);
    y = find(y);
    if (x != y) fa[x] = y;
}
```

1. 初始化

```cpp
fa.resize(n + 1);
for (int i = 1; i <= n; ++i) fa[i] = i;
```

1. 判断是否同组

```cpp
if (find(a) == find(b)) {
    cout << "same\\n";
}
```

1. 统计连通块思路

```cpp
// 最后统计有多少个 i 满足 find(i) == i
```

## ⚠️ Pitfalls（高频错误）

- 合并前忘记先找根
- 初始化 `fa[i] = i` 漏掉
- 路径压缩位置写错
- 误以为并查集能直接维护最短路径、拓扑关系等复杂结构

## 🚀 Performance / Tips（性能优化）

- 路径压缩后，单次操作通常非常快
- 高频经验：
    - “连边” + “问是否连通” 基本就要想并查集
    - Kruskal 算法里几乎是标配
- 适合做：
    - 连通块
    - 好友分组
    - 动态集合合并

## 🧪 Common Scenarios（常见使用场景）

- 连通性判断
- Kruskal 最小生成树
- 好友分组 / 岛屿合并
- 图中集合归并

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;
vector<int> fa;
int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }
int main() {
    fa = {0, 1, 2, 3};
    fa[find(1)] = find(2);
    cout << (find(1) == find(2)) << '\\n';
}
```

## 📌 One-liner Summary（一句话总结）

- **并查集就是：高效维护元素分组与连通关系的集合结构。**