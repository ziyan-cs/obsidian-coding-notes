> **核心考点**：双向链表的节点级内存分配、插入删除不失效、与 vector 的性能反转

## 1. 内存布局

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

## 2. 核心操作复杂度

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

## 3. list vs vector：性能反转场景

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

## 4. 唯一特性：拼接与归并

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

## 5. 工程建议

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
