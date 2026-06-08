---
tags:
  - cpp/stl
status: 🌱
---

> [!important] **核心考点**：容器选择决策、复杂度一览、面试前快速复习用

## 容器选择树

```text
需要连续内存？
├── 是 → 随机访问为主？→ vector ⭐
│       双端操作？    → deque
│       固定大小编译期已知？→ array
└── 否 → 保持迭代器稳定？
        ├── 是 → 双向遍历？→ list
        │        单向遍历？→ forward_list
        └── 否 → 排序/范围查询？
                ├── 是 → map / set / multiset ⭐
                └── 否 → O(1) 查找？→ unordered_map / unordered_set ⭐
```

## 复杂度对比

| 容器 | 随机访问 | 头插 | 尾插 | 中间插入 | 查找 |
|------|---------|------|------|---------|------|
| `vector` | O(1) ✅ | O(N) ❌ | 均摊 O(1) | O(N) | O(N) |
| `deque` | O(1) | O(1) ✅ | O(1) ✅ | O(N) | O(N) |
| `list` | ❌ | O(1) | O(1) | O(1) ✅ | O(N) |
| `map` | ❌ | — | — | O(log N) | O(log N) |
| `unordered_map` | ❌ | — | — | 均摊 O(1) | 均摊 O(1) ✅ |

## 内存与迭代器稳定性

| 容器 | 元素存储 | 插入迭代器影响 | erase 迭代器影响 |
|------|---------|---------------|-----------------|
| `vector` | 连续 | 扩容时全失效 | pos 后全失效 |
| `deque` | 分块 | 中间插入全失效，两端不影响 | 中间擦除全失效 |
| `list` | 节点分散 | ❌ 不影响 | 仅被删元素 |
| `map/set` | 树节点 | ❌ 不影响 | 仅被删元素 |
| `unordered_map` | 哈希桶 | rehash 时全失效 | 仅被删元素 |

## 记忆口诀

```text
vector 数组动态长  随机访问最在行
deque 双端效率高  中间插入受不了
list 链表插入稳  遍历起来慢慢等
map set 红黑树  排序查找两不误
unordered 哈希表  飞一般的查找
```

## 面试前必记

### 扩容策略
- `vector`：1.5x-2x 扩容，重分配时移动/拷贝所有元素
- `unordered_map`：超过 max_load_factor 时 rehash（通常 2x bucket 数）
- `deque`：不移动已有元素，只分配新 block

### 迭代器失效
- `vector` push_back 触发扩容 → **全部**失效
- `deque` 中间插入 → **全部**失效（两端插入不影响）
- `vector` erase(pos) → pos **之后**全部失效

### 特例
- `vector<bool>` 不是标准容器（bit-packed，`operator[]` 返回代理对象）
- `list::sort` 用归并排序，不是 `std::sort`
- `stack`/`queue` 默认底层是 `deque` 不是 `vector`
- `priority_queue` 默认底层是 `vector`

---

容器适配器与算法详见 → [Container Adapters (容器适配器)](/03-C++%20Programming%20(编程语言)/04%20·%20STL/03-Container%20Adapters%20(容器适配器).md) · [Algorithm Library (算法库速查)](/03-C++%20Programming%20(编程语言)/04%20·%20STL/05-Algorithm%20Library%20(算法库速查).md)
