---
tags:
  - cpp/stl
status: 🌱
---

# unordered_map Hash Table & Collision — 哈希表与冲突

> [!important] **核心考点**：哈希表结构（bucket + linked list）、rehash 策略、自定义哈希函数、碰撞解决

## 底层结构：Separate Chaining（链地址法）

```text
bucket array (vector of linked lists):
┌──────┐
│ [0]  │──→ node ──→ node
├──────┤
│ [1]  │──→ node
├──────┤
│ [2]  │──→ (empty)
├──────┤
│ [3]  │──→ node ──→ node ──→ node
├──────┤
│ ...  │
└──────┘
```

- 每个 bucket 是一个单向链表
- `hash(key) % bucket_count` 决定元素放入哪个 bucket
- 当 `load_factor > max_load_factor`（默认 1.0）时触发 rehash

## Load Factor & Rehash

```cpp
// load_factor = size / bucket_count
// max_load_factor 默认 1.0（可自定义）
std::unordered_map<int, std::string> m;
m.max_load_factor(0.75f);  // 自定义阈值

// rehash：
// 1. 创建更大的 bucket 数组（通常是 2x 左右）
// 2. 重新计算所有元素的 bucket index
// 3. 迁移元素
// rehash 后全部迭代器失效！
```

**rehash 的触发条件**：
- `insert` 后 `load_factor > max_load_factor` → 自动 rehash
- `rehash(n)` / `reserve(n)` 显式调用

## 哈希函数

```cpp
// 标准库已提供基础类型的哈希
std::hash<int>{} (42);
std::hash<std::string>{} ("hello");

// 自定义类型的哈希（两种方式）
struct Point { int x, y; };

// 方式 1：特化 std::hash
namespace std {
    template<> struct hash<Point> {
        size_t operator()(const Point& p) const {
            return hash<int>{}(p.x) ^ (hash<int>{}(p.y) << 1);
        }
    };
}
using PointMap = std::unordered_map<Point, int>;

// 方式 2：用结构化绑定（C++20 更简洁的写法）
// 或用 std::hash 组合
struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const {
        return hash<int>{}(p.first) ^ (hash<int>{}(p.second) << 1);
    }
};
```

**好的哈希函数标准**：
- **均匀分布**：避免哈希碰撞
- **高效计算**：不要为了"完美"而使用复杂的哈希（如加密哈希）
- 不要返回常量——所有元素都在同一 bucket 退化为链表（O(N)）

## 性能关键操作

| 操作 | 均摊复杂度 | 最坏情况 |
|------|-----------|----------|
| `operator[]` / `find` | O(1) | O(N)（全部在同一 bucket）|
| `insert` | O(1) | O(N)（rehash 时）|
| `erase` | O(1) | O(N) |
| `rehash` | O(N) | — |

## 工程建议

```cpp
// ✅ 预分配减少 rehash
std::unordered_map<int, int> m;
m.reserve(10000);  // 预先分配 10000 个 bucket

// ✅ 大批量插入前 reserve
std::vector<std::pair<Key, Val>> batch;
m.reserve(batch.size() * 1.5);  // 预留空间
m.insert(batch.begin(), batch.end());

// ✅ 选择合适容器
// 需要有序？→ map
// 需要 O(1) 查找？→ unordered_map
// key 是整数且范围小？→ vector 甚至更快（缓存友好）

// ❌ 不要在 unordered_map 中存大量自定义类型而不特化 hash
// ❌ 不要假设遍历顺序稳定（rehash 后 bucket index 改变）
// ❌ 对性能敏感时避免频繁 insert/erase（rehash O(N)）
```

> **面试重点**：哈希碰撞攻击——如果哈希函数对所有输入返回相同的值（或攻击者构造大量碰撞 key），`unordered_map` 退化为链表（O(N)）。C++11 后标准库使用**随机 seed** 的哈希函数缓解此问题，但自定义哈希函数仍可能容易受攻击。

---

红黑树实现的有序容器对比详见 → [map & set Red-Black Tree (红黑树与有序容器)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/02-Associative%20Containers%20Internals%20(关联容器底层)%20⭐/02a-map%20&%20set%20Red-Black%20Tree%20(红黑树与有序容器).md)
