#data-structure #heap #priority-queue #complete-binary-tree #cpp

## ⚡ TL;DR（快速决策）

- **支持快速维护和取出当前最值的完全二叉树结构**
- 相关特征：
    - 动态维护最大值 / 最小值
    - 每次都取当前最优元素
    - Top K
    - 贪心过程中的“反复取最好”
- 最常见实现接口：`priority_queue`

## 🧩 Core Idea（核心本质）

- 堆满足两层结构特征：
    1. 形态上是完全二叉树
    2. 数值上满足堆序性质
- 大根堆：父节点 >= 子节点
- 小根堆：父节点 <= 子节点
- 一句话理解：
    - **堆就是“局部有序，堆顶最优”。**

## 🔧 Usage Patterns（可复用代码模板）

1. 默认大根堆

```cpp
#include <iostream>
#include <queue>
using namespace std;
int main() {
    priority_queue<int> pq;
    pq.push(3); pq.push(10); pq.push(5);
    cout << pq.top() << '\\n';
}
```

1. 小根堆

```cpp
priority_queue<int, vector<int>, greater<int>> pq;
```

1. Top K

```cpp
for (int x : nums) {
    pq.push(x);
    if ((int)pq.size() > k) pq.pop();
}
```

1. 反复取最值

```cpp
while (!pq.empty()) {
    cout << pq.top() << ' ';
    pq.pop();
}
```

## ⚠️ Pitfalls（高频错误）

- 默认优先队列是大根堆
- `pop()` 不返回元素
- 堆不是完全排序容器
- 比较器方向容易写反

## 🚀 Performance / Tips（性能优化）

- 插入：$O(log n)$
- 删除堆顶：$O(log n)$
- 查看堆顶：$O(1)$
- Dijkstra、Top K、调度类都高频使用

## 🧪 Common Scenarios（常见使用场景）

- Top K
- 动态最值维护
- Dijkstra
- 贪心每次取当前最优
- 优先级任务调度

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
using namespace std;
int main() {
    priority_queue<int> pq;
    pq.push(4); pq.push(1); pq.push(7);
    while (!pq.empty()) {
        cout << pq.top() << ' ';
        pq.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **堆就是：通过堆序性质快速维护并取出当前最值的树形结构。**