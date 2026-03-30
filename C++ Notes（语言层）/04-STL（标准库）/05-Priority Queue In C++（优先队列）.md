#stl #priority-queue #heap #cpp #container-adapter

## ⚡ TL;DR（快速决策）

- `priority_queue` 本质是：**每次都先取出当前优先级最高的元素**
- 默认情况下，它是**大根堆**
- 一看到这些需求，要优先想到优先队列：
    - 动态维护当前最大值 / 最小值
    - 每次取最优元素继续处理
    - Top K 问题
    - 贪心过程中的“反复选最好 / 最小”
- 高频操作：`push()`、`pop()`、`top()`
- 如果题目是“反复取当前最值”，默认先想 `priority_queue`

## 🧩 Core Idea（核心本质）

- 优先队列不是普通排队，而是按“优先级”出队
- 谁优先级高，谁先出来
- STL 里的 `priority_queue` 底层通常基于堆
- 一句话理解：
    - **不是谁先来谁先出，而是谁更重要谁先出。**
- 默认是最大元素先出
- 想要最小元素先出，需要手动改成小根堆

## 🔧 Usage Patterns（可复用代码模板）

1. 默认大根堆

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    priority_queue<int> pq;
    pq.push(3);
    pq.push(10);
    pq.push(5);

    cout << pq.top() << '\\n';
    pq.pop();
    cout << pq.top() << '\\n';
    return 0;
}
```

1. 小根堆写法

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
    priority_queue<int, vector<int>, greater<int>> pq;
    pq.push(3);
    pq.push(10);
    pq.push(5);

    cout << pq.top() << '\\n';
    return 0;
}
```

1. Top K 基础模板

```cpp
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int main() {
    vector<int> nums = {7, 2, 9, 1, 5};
    int k = 3;

    priority_queue<int, vector<int>, greater<int>> pq;
    for (int x : nums) {
        pq.push(x);
        if ((int)pq.size() > k) pq.pop();
    }

    cout << pq.top() << '\\n';
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 默认是大根堆，不是小根堆
- `pop()` 不返回被删元素
- 不能像数组一样遍历全部元素
- 比较器写反会直接导致结果全错
- 空优先队列不能直接 `top()`

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `push()`：$O(log n)$
    - `pop()`：$O(log n)$
    - `top()`：$O(1)$
- Top K、动态最值、Dijkstra、贪心里非常常见
- 如果只要一次性排序，不一定非要优先队列

## 🧪 Common Scenarios（常见使用场景）

- Top K 问题
- 动态最值维护
- 堆排序思想
- Dijkstra
- 贪心选当前最优

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    priority_queue<int> pq;
    pq.push(4);
    pq.push(1);
    pq.push(7);

    while (!pq.empty()) {
        cout << pq.top() << ' ';
        pq.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`priority_queue` 就是：一个每次优先取出当前最高优先级元素的堆式容器。**