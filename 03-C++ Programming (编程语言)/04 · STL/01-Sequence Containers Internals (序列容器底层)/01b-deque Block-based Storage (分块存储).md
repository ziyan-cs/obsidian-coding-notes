---
tags:
  - cpp/stl
status: 🌱
---

> [!important] **核心考点**：双端队列的分块存储结构、中间段指针管理、与 vector 的性能取舍

## 内存布局

`deque` 由**多个固定大小的块（buffer）** 和一个**中控器（map）** 组成：

```mermaid
graph TD
    subgraph Map["中控器 (map)<br/>指针数组"]
        B0["块指针 0"]
        B1["块指针 1"]
        B2["块指针 2"]
        B3["块指针 3"]
    end
    
    subgraph Block0["数据块 0"]
        E00["elem"]
        E01["elem"]
        E02["elem"]
        E03["elem"]
    end
    
    subgraph Block1["数据块 1"]
        E10["elem"]
        E11["elem"]
        E12["elem"]
        E13["elem"]
    end
    
    subgraph Block2["数据块 2"]
        E20["elem"]
        E21["elem"]
        E22["elem"]
        E23["elem"]
    end
    
    B0 --> Block0
    B1 --> Block1
    B2 --> Block2
    
    note right of Block2
        每块固定大小 (通常 512 字节)
        两端插入 O(1)
        随机访问 O(1) 但需两次间接
    end note
```

- 每个 buffer 固定大小（通常是 512 字节，或 `max(1, 512/sizeof(T))` 个元素）
- map 是中控器（指针数组），元素指向各个 buffer
- 向两端插入时，头/尾 buffer 满了就分配新 buffer 并更新 map

## 核心操作

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `push_front` / `push_back` | O(1) | 只有新 buffer 时才分配 |
| `pop_front` / `pop_back` | O(1) | 释放整个 buffer 达到阈值时才真正释放 |
| `operator[]` | O(1) | **两次间接跳转**（map → buffer → element）|
| `insert` 中间 | O(N) | 需要搬移元素 |
| `begin` / `end` | O(1) | |

## deque 与 vector 关键区别

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

## 迭代器失效规则

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

## 工程建议

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

vector 的动态扩容机制对比详见 → [vector Dynamic Array & Reallocation (动态扩容原理)](/03-C++%20Programming%20(编程语言)/04%20·%20STL/01-Sequence%20Containers%20Internals%20(序列容器底层)/01a-vector%20Dynamic%20Array%20&%20Reallocation%20(动态扩容原理)%20⭐.md)
