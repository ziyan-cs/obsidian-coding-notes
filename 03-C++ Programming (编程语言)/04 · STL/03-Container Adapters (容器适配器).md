> **核心考点**：stack/queue/priority_queue 都是适配器而非独立容器，底层容器可替换

## 适配器模式

容器适配器**封装**底层容器，只暴露特定接口：

| 适配器 | 默认底层容器 | 特点 |
|--------|-------------|------|
| `stack`（栈） | `deque` | LIFO |
| `queue`（队列） | `deque` | FIFO |
| `priority_queue`（优先队列） | `vector` | 大根堆（默认）|

## stack

```cpp
// 默认 deque 做底层，也可以指定 vector/list
std::stack<int> s;                      // deque 底层
std::stack<int, std::vector<int>> sv;   // vector 底层
std::stack<int, std::list<int>> sl;     // list 底层

// 接口
s.push(1);  s.pop();     // 入栈/出栈（pop 不返回值！）
s.top();                 // 访问栈顶
s.empty(); s.size();
```

**为什么默认用 deque 而不是 vector？**
- deque 的尾部操作 O(1) 且**不会触发重分配**
- vector 尾部也是 O(1) 但可能重分配（拷贝所有元素）
- list 尾部 O(1) 但内存开销大、缓存不友好

## queue

```cpp
std::queue<int> q;
q.push(1);  q.pop();    // 入队/出队（pop 不返回值！）
q.front();  q.back();   // 访问队首/队尾
```

**不支持迭代器遍历**— 只能逐个出队。

## priority_queue

```cpp
// 默认大根堆（最大元素在 top）
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(4);
pq.top();  // 4（最大元素）

// 小根堆
std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;

// 自定义比较
auto cmp = [](int a, int b) { return a > b; };
std::priority_queue<int, std::vector<int>, decltype(cmp)> custom_pq(cmp);
```

**底层原理**：

```text
vector 上的堆操作：
push → push_back + push_heap (sift-up O(log N))
pop → pop_heap (sift-down O(log N)) + pop_back
top → front (O(1))
```

**性能**：
| 操作 | 复杂度 |
|------|--------|
| `push` | O(log N) |
| `pop` | O(log N) |
| `top` | O(1) |

```cpp
// 高效更新：不直接提供修改元素 API
// 替代方案：push 新值 + lazy 忽略旧值
// 或自己操作底层容器（不推荐）

// 构建优先队列的 3 种方法对比：
// 1. 逐个 push：O(N log N) ← 一般不这么做
// 2. 用底层容器的迭代器构造：O(N)  ← heapify 构造
std::vector<int> data{3, 1, 4, 1, 5, 9};
std::priority_queue<int> pq(data.begin(), data.end());  // O(N) 构造
```

> **工程要点**：`priority_queue` **不是 stable 的**（相同优先级元素顺序不确定）。需要稳定优先队列时，可以用 `(priority, counter)` 做 pair 来解决。
