# Container Adapters：stack, queue, priority_queue

## Stack
#stl #stack #cpp #lifo #container-adapter

## ⚡ TL;DR（快速决策）

- `stack` 本质是：**后进先出（LIFO）**
- 一看到这些特征，要优先想到栈：
    - 最近加入的元素要先处理
    - 括号匹配
    - 表达式求值
    - 单调栈前的基础容器理解
    - 需要“撤回上一步”式处理
- 常用操作就几个：
    - `push()`
    - `pop()`
    - `top()`
    - `empty()`
- `stack` 不支持随机访问，也不支持遍历接口
- 如果题目是“最后进去的先出来”，默认先想栈

## 🧩 Core Idea（核心本质）

- 栈是一种**操作受限的线性结构**
- 你只能在“栈顶”进行插入和删除
- 所以栈最像：
    - 盘子一层层叠起来
    - 只能先拿最上面那个
- 一句话理解：
    - **谁最后进去，谁最先出来。**
- C++ STL 里的 `stack` 是容器适配器，不是普通顺序容器

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础使用

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    st.push(10);
    st.push(20);
    st.push(30);

    cout << st.top() << '\\n';
    st.pop();
    cout << st.top() << '\\n';
    return 0;
}
```

1. 判断是否为空

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    cout << st.empty() << '\\n';
    st.push(1);
    cout << st.empty() << '\\n';
    return 0;
}
```

1. 括号匹配模板

```cpp
#include <iostream>
#include <stack>
#include <string>
using namespace std;

bool isValid(string s) {
    stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else {
            if (st.empty()) return false;
            char t = st.top();
            st.pop();
            if (c == ')' && t != '(') return false;
            if (c == ']' && t != '[') return false;
            if (c == '}' && t != '{') return false;
        }
    }
    return st.empty();
}
```

1. 逆序输出

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    for (int x : {1, 2, 3, 4}) st.push(x);

    while (!st.empty()) {
        cout << st.top() << ' ';
        st.pop();
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 空栈上不能直接 `top()` 或 `pop()`
- `pop()` 不返回被删元素
- `stack` 不能像 `vector` 那样遍历
- 别把队列和栈的顺序搞混
- 括号题里常见错误是：忘记最后检查栈是否为空

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `push()`：$O(1)$
    - `pop()`：$O(1)$
    - `top()`：$O(1)$
- 实战建议：
    - 题目只要是“最近元素优先”，先想栈
    - 需要遍历全部元素时，通常要边弹边处理
    - 后续学单调栈时，普通栈就是基础

## 🧪 Common Scenarios（常见使用场景）

- 括号匹配
- 逆序处理
- 表达式求值
- 撤销 / 回退逻辑
- 单调栈基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);

    while (!st.empty()) {
        cout << st.top() << ' ';
        st.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`stack` 就是：一个只允许在栈顶进出元素、遵循后进先出规则的容器。**
## Queue
#stl #queue #cpp #fifo #container-adapter

## ⚡ TL;DR（快速决策）

- `queue` 本质是：**先进先出（FIFO）**
- 一看到这些特征，要优先想到队列：
    - 先来的先处理
    - 按顺序排队处理任务
    - 层序遍历 / BFS
    - 缓冲区、事件流、等待队列
- 常用操作：`push()`、`pop()`、`front()`、`back()`、`empty()`
- `queue` 不支持随机访问，也不支持直接遍历
- 如果题目是“先进入的元素先出来”，默认先想队列

## 🧩 Core Idea（核心本质）

- 队列是一种**操作受限的线性结构**
- 插入通常在队尾，删除通常在队头
- 最像现实中的排队
- 一句话理解：
    - **谁先进去，谁先出来。**
- STL 的 `queue` 是容器适配器，底层常基于 `deque`

## 🔧 Usage Patterns（可复用代码模板）

1. 最基础使用

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);

    cout << q.front() << '\\n';
    q.pop();
    cout << q.front() << '\\n';
    return 0;
}
```

1. 查看队头和队尾

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    cout << q.front() << '\\n';
    cout << q.back() << '\\n';
    return 0;
}
```

1. BFS 基础模板

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);

    while (!q.empty()) {
        int x = q.front();
        q.pop();
        cout << x << ' ';
        if (x < 3) q.push(x + 1);
    }
    return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- 空队列上不能直接 `front()` / `back()` / `pop()`
- `pop()` 不返回被删元素
- 别把队列和栈顺序搞反
- `queue` 不能像 `vector` 一样下标访问

## 🚀 Performance / Tips（性能优化）

- 高频复杂度记忆：
    - `push()`：$O(1)$
    - `pop()`：$O(1)$
    - `front()` / `back()`：$O(1)$
- BFS、按顺序处理任务时特别常用
- 如果需要双端操作，更适合 `deque`

## 🧪 Common Scenarios（常见使用场景）

- BFS / 层序遍历
- 任务排队处理
- 事件流处理
- 缓冲区模型

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    while (!q.empty()) {
        cout << q.front() << ' ';
        q.pop();
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **`queue` 就是：一个遵循先进先出规则、只允许队头出队和队尾入队的容器。**
## Priority Queue
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