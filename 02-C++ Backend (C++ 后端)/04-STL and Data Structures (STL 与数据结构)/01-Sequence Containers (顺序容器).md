---
status: stable
confidence: high
verified: 2026-09-06
---

> [!abstract] 学习定位：本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

# vector Dynamic Array and Reallocation (动态扩容原理)

> [!note] 本节重点：核心考点：动态数组的连续内存布局、扩容策略、迭代器失效场景、与 `std::array`/原始数组的抉择

# 内存布局与核心特性

`vector` 管理一块连续存储；实现常以起始、结束、容量边界表示状态，但具体内部布局不由标准规定：

```cpp
// 伪代码：典型 vector 内部结构
struct vector_internals {
    T* _start;       // 数据起始
    T* _finish;      // 已构造元素末尾
    T* _end_of_storage; // 已申请内存末尾
};
// size() = _finish - _start
// capacity() = _end_of_storage - _start
```

**连续内存是核心优势**：Cache locality 极好，迭代、随机访问都是 O(1)。

# 扩容策略（Reallocation）

当 `size() == capacity()` 时 `push_back` 触发扩容：

```cpp
// 典型扩容流程
void push_back(const T& val) {
    if (_finish == _end_of_storage) {
        size_t new_cap = grow();  // 计算新容量
        T* new_buf = allocate(new_cap);
        move_or_copy(_start, _finish, new_buf);  // 转移已有元素
        deallocate(_start);
        _start = new_buf;
        _finish = new_buf + old_size;
        _end_of_storage = new_buf + new_cap;
    }
    construct(_finish, val);
    ++_finish;
}
```

**扩容倍数对比（工程选择）**：

| 策略 | 实现 | 均摊 O(1) | 内存浪费 | 典型使用者 |
|------|------|-----------|----------|-----------|
| 固定增量 | `cap += N` | ❌ O(N) | 低 | — |
| 几何增长 | 实现定义 | ✅ | 与增长因子有关 | 标准不规定具体倍数 |

> **工程选择**：标准只保证扩容后的复杂度语义，不保证增长倍数。不要把某编译器版本的容量序列写进业务逻辑；已知数量时用 `reserve`，未知数量时依据 profile 判断是否值得优化。

# 关键操作与复杂度

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `operator[]` / `at()` | O(1) | `at()` 带边界检查（抛异常） |
| `push_back` | **均摊** O(1) | 扩容时 O(N)，极少数次 |
| `pop_back` | O(1) | 不释放内存，只析构 |
| `insert(pos)` | O(N) | 后续元素全部后移 |
| `erase(pos)` | O(N) | 后续元素全部前移 |
| `emplace_back` | **均摊** O(1) | 原地构造，避免拷贝 |

# 迭代器失效（面试重点）

哪些操作会让迭代器**失效**：

| 操作 | 失效范围 | 原因 |
|------|---------|------|
| `push_back` / `emplace_back` | 发生重分配则全部；否则通常仅 `end()` | 重分配或末尾变化 |
| `insert` / `erase` | 发生重分配则全部；否则位置及之后失效 | 元素移动 |
| `reserve` | 仅当容量真的改变时全部失效 | 可能触发重分配 |
| `resize(n)` (n > capacity) | **全部** | 触发重分配 |
| `resize(n)` (n <= capacity) | 仅超出元素失效 | 仅析构超出部分 |
| `pop_back` | 仅被删除元素及 `end()` | — |

```cpp
// ❌ 错误：扩容导致迭代器失效
std::vector<int> v{1, 2, 3};
auto it = v.begin();
v.push_back(4);  // 可能扩容，it 悬空
*it = 10;        // 未定义行为！

// ✅ 正确：预留足够容量
v.reserve(100);
auto it2 = v.begin();
v.push_back(42);  // 不会扩容
*it2 = 10;        // ✅ 安全
```

# 工程最佳实践

```cpp
// ✅ 预分配避免频繁扩容
std::vector<int> v;
v.reserve(1000);  // 已知大约数量时提前预留

// ✅ 在直接传构造参数时可用 emplace_back；已有对象时 push_back 同样清晰
v.emplace_back(args...);

// ✅ shrink_to_fit 释放多余内存（O(N) 拷贝, 谨慎使用）
v.shrink_to_fit();

// ❌ 不要把 vector<bool> 当普通 vector 用
// vector<bool> 是位压缩特化，operator[] 返回代理对象而非引用
```

> **面试常见题**：重分配需要在新存储中构造元素，并处理移动/拷贝的异常安全；不能把非平凡 C++ 对象按字节 `realloc`。实现会依据元素类型的移动/拷贝性质选择策略。

# 30 秒回答

`vector` 用连续存储换取随机访问与 cache locality，代价是中间插删和扩容时可能移动元素。重分配会使全部迭代器、指针和引用失效；未重分配时要按具体操作判断失效范围。容量增长倍数是实现细节，已知规模才用 `reserve` 明确表达预分配意图。

# vector vs 其他容器

| 场景 | 推荐 |
|------|------|
| 随机访问为主 | `vector` |
| 频繁头插/头删 | `deque` 或 `list` |
| 频繁中间插入 | `list` 或 `deque` |
| 大小固定且编译期已知 | `std::array` |
| 需要稳定迭代器 | `list`（插入删除不影响已有迭代器） |

---

其他序列容器对比详见 → [deque Block-based Storage (分块存储)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01b-deque%20Block-based%20Storage%20(分块存储).md) · [list Doubly Linked List (双向链表)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01c-list%20Doubly%20Linked%20List%20(双向链表).md)

---

# deque Block Based Storage (分块存储)

> [!note] 本节重点：核心考点：双端队列的分块存储结构、中间段指针管理、与 vector 的性能取舍

# 内存布局

`deque` 由**多个固定大小的块（buffer）** 和一个**中控器（map）** 组成：

```text
deque Memory Layout:

  Map (Middle Controller / Pointer Array)
  ┌─────────────────────────────────────┐
  │  blk ptr 0  │  blk ptr 1  │  blk ptr 2  │  blk ptr 3  │
  └──────┬──────────────────┬──────────────┴──────────────┘
         │                  │              │
         ↓                  ↓              ↓
  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
  │  Data Block 0 │  │  Data Block 1 │  │  Data Block 2 │
  ├──────────────┤  ├──────────────┤  ├──────────────┤
  │ elem │ elem │  │ elem │ elem │  │ elem │ elem │
  │ elem │ elem │  │ elem │ elem │  │ elem │ elem │
  └──────────────┘  └──────────────┘  └──────────────┘

Properties:
  - Each block is fixed size (typically 512 bytes)
  - push_front / push_back: O(1) amortized
  - Random access operator[]: O(1) but requires double indirection
    (map → buffer → element)
```

- 每个 buffer 固定大小（通常是 512 字节，或 `max(1, 512/sizeof(T))` 个元素）
- map 是中控器（指针数组），元素指向各个 buffer
- 向两端插入时，头/尾 buffer 满了就分配新 buffer 并更新 map

# 核心操作

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `push_front` / `push_back` | O(1) | 只有新 buffer 时才分配 |
| `pop_front` / `pop_back` | O(1) | 释放整个 buffer 达到阈值时才真正释放 |
| `operator[]` | O(1) | **两次间接跳转**（map → buffer → element）|
| `insert` 中间 | O(N) | 需要搬移元素 |
| `begin` / `end` | O(1) | |

# deque 与 vector 关键区别

| 特性 | `vector` | `deque` |
|------|----------|---------|
| 内存布局 | 单一连续块 | 多块不连续 |
| 头插 | O(N) ❌ | O(1) ✅ |
| 尾插 | 均摊 O(1) | O(1) |
| 随机访问 | O(1) **一次间接** | O(1) **两次间接** |
| 扩容影响 | **全部**迭代器失效 | 仅**指向被移动元素的**迭代器失效 |
| `push_front` | 不支持 | 支持 |
| 与 C API 兼容 | ✅ `&v[0]` | ❌ |
| 内存碎片 | 低 | 略高 |

# 迭代器失效规则

```cpp
deque<int> d = {1, 2, 3, 4, 5};

// ✅ 头尾插入：不影响已有元素的迭代器
auto it = d.begin() + 2;  // 指向 3
d.push_front(0);
d.push_back(6);
assert(*it == 3);  // ✅ 安全

// ❌ 中间插入/删除：全部失效
d.insert(d.begin() + 2, 99);
*it;  // 未定义行为！
```

# 工程建议

```cpp
// ✅ deque 适合：双端队列、滑动窗口
std::deque<int> window;

// ✅ 也适合：任务队列（两端调度）
std::deque<std::function<void()>> tasks;

// ✅ 与 vector 混合：vector 存大数据时，重分配拷贝代价高
// 但 deque 分段存储，扩容不拷贝已有元素

// ❌ 不要假设 deque 元素是连续存储的
// ❌ 不要用 &d[0] 传给 C 接口
// ❌ 不适合频繁随机访问——两次间接开销高于 vector
```

> **必知**：`deque` 的 `operator[]` 虽然也是 O(1)，但常数比 `vector` 大得多（多一次指针跳转）。实测在连续随机访问场景 `deque` 比 `vector` 慢 2-5 倍。如果只做**双端操作**，`deque` 是正确选择；如果需要随机访问 + 双端操作，考虑 `vector + ring buffer` 或 `boost::circular_buffer`。

---

vector 的动态扩容机制对比详见 → [vector Dynamic Array & Reallocation (动态扩容原理)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01a-vector%20Dynamic%20Array%20&%20Reallocation%20(动态扩容原理)%20⭐.md)

---

# list Doubly Linked List (双向链表)

> [!note] 本节重点：核心考点：双向链表的节点级内存分配、插入删除不失效、与 vector 的性能反转

# 内存布局 · 延伸要点 2
`list` 是一个**双向循环链表**（GCC 实现为带哨兵节点的循环链表）：

```cpp
// 节点结构
struct _List_node {
    _List_node* _M_next;  // 指向下一个节点
    _List_node* _M_prev;  // 指向上一个节点
    T           _M_data;  // 存储的元素
};

// 哨兵节点（list 自身持有一个哨兵节点）
// 空 list: 哨兵._M_next = 哨兵._M_prev = &哨兵
// 非空: 哨兵  ↔  node1  ↔  node2  ↔  哨兵
```

**每个节点独立分配在堆上**——这是 `list` 最核心的性能特征。

# 核心操作复杂度

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `push_back` / `push_front` | O(1) | 只操作指针 |
| `pop_back` / `pop_front` | O(1) | 只操作指针，析构节点 |
| `insert(pos, val)` | O(1) | **已知迭代器时**，只改邻近指针 |
| `erase(pos)` | O(1) | 同上 |
| `operator[]` | ❌ **不支持** | 需要遍历 |
| `find(val)` | O(N) | 线性遍历 |
| `size()` | O(1) (C++11+) | C++11 前是 O(N) |
| `splice` | O(1) | 只移动节点（不拷贝数据） |

# list vs vector：性能反转场景

```cpp
// vector: 中间插入 O(N) ❌
// list:   中间插入 O(1) ✅ (已知位置)
std::list<int> l = {1, 2, 3, 4, 5};
auto it = l.begin(); std::advance(it, 2);
l.insert(it, 99);  // O(1) — 只改指针
```

| 场景 | `vector` | `list` |
|------|----------|--------|
| 遍历全部元素 | ✅ 极快（连续内存，缓存友好） | ❌ 慢（跳跃访问，缓存不友好）|
| 任意位置插入 | ❌ O(N) | ✅ O(1) |
| 随机访问 | ✅ O(1) | ❌ 不支持 |
| 内存开销 | 低（只有数据） | 高（每节点 2 个指针 + 可能的内存碎片）|
| 排序 | ✅ `std::sort` | 只能 `list::sort` (归并) |
| 迭代器稳定性 | 扩容时全失效 | **插入/删除不影响其他迭代器** |

> **关键认知**：遍历 `list` 比 `vector` **慢一个数量级**。因为 `list` 的节点在堆上随机分布，每次访问 `_M_next` 都可能 cache miss。如果你需要**大量遍历**（一次遍历耗时超过数十次插入节省的时间），宁愿用 `vector`。

# 唯一特性：拼接与归并

```cpp
std::list<int> a{1, 3, 5}, b{2, 4, 6};

// splice：把 b 的所有节点转移到 a（O(1) 指针操作）
a.splice(a.end(), b);  // 之后 b 为空
// b 的节点被"嫁接"到 a——零拷贝！

// merge：归并两个有序链表
a.sort();
b.sort();
a.merge(b);  // O(N) 比较 + 指针操作

// unique：去除连续重复元素
a.unique();
```

# 工程建议 · 延伸要点 2
```cpp
// ✅ list 适合：需要维护指向元素的稳定指针/迭代器
struct Request { int id; /* big data */ };
std::list<Request> pending;
auto iter = pending.insert(pending.end(), {42});
// 后续大量插入/删除不影响 iter

// ✅ 适合：大对象的容器（拷贝代价高，无法移动的场景）
// ✅ 适合：需要 O(1) 的 splice/merge

// ❌ 数据量小（< 100 元素）时不必用 list，vector 遍历更快
// ❌ 频繁 size() 调用（C++11 后没问题，但 list::size O(1) 有额外计数开销）
// ❌ 缓存不敏感场景：如果不做频繁中间插入，vector + reserve 几乎总是更好
```

> **面试重点**：为什么 `list::size()` 在 C++11 前是 O(N)？原因是某些实现（GCC）为了让 `splice` 保持 O(1) 而不维护 `_M_size`。C++11 规定 `size()` 必须是 O(1)，因此 GCC 增加了 `_M_size` 计数器，splice 时手动调整。

---

vector 连续内存性能对比详见 → [vector Dynamic Array & Reallocation (动态扩容原理)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01a-vector%20Dynamic%20Array%20&%20Reallocation%20(动态扩容原理)%20⭐.md)

# 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

# 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

# 学习闭环

## 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **01-Sequence Containers (顺序容器)**。

## 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

## 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
