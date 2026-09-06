---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 02-Associative Containers (关联容器)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## 30 秒回答

**核心结论**：阅读定位  本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。


## map and set (红黑树与有序容器)

> [!abstract] 核心考点：红黑树的平衡规则、有序关联容器的底层实现、与哈希容器的选择

## 底层结构：红黑树

`map`、`set`、`multimap`、`multiset` 底层是**红黑树**（Red-Black Tree），一种近似平衡的 BST：

### 红黑树的 5 条规则

```text
1. 每个节点是红色或黑色
2. 根节点是黑色
3. 叶子（NIL）是黑色
4. 红色节点的子节点必须是黑色（不能有连续红节点）
5. 任一节点到其每个叶子路径上的黑节点数相同
```

### 为什么用红黑树而不是 AVL？

| 特性 | 红黑树 | AVL |
|------|--------|-----|
| 平衡标准 | 最长路径 ≤ 2 倍最短路径 | 左右子树高度差 ≤ 1 |
| 查询性能 | O(log N)，常数比 AVL 略大 | O(log N)，更严格 |
| **插入/删除** | **O(log N) 且重平衡更快**（最多 3 次旋转） | O(log N) 但可能需要多次旋转 |
| 适用场景 | **插入删除频繁**（STL 的选择） | 查询远多于修改 |

> **STL 选择红黑树**的原因：map/set 是通用容器，需要同时兼顾查询和修改性能。红黑树的插入/删除重平衡代价更低（均摊 O(1) 次旋转）。

## map/set vs unordered_map/unordered_set

| | `map` (红黑树) | `unordered_map` (哈希表) |
|---|---|---|
| 元素顺序 | ✅ **有序**（按 key 升序） | ❌ 无序 |
| 插入/查找/删除 | O(log N) | O(1) 均摊，O(N) 最坏 |
| 需要 | `operator<` | `std::hash<T>` + `operator==` |
| 范围查询 | ✅ `lower_bound`/`upper_bound` | ❌ 不支持 |
| 遍历顺序稳定 | ✅ **始终有序遍历** | ❌ 扩容后元素位置会变 |

```cpp
// map：有序，适合范围查询
std::map<int, std::string> scores;
scores.insert({1, "Alice"});
scores.insert({3, "Bob"});
scores.insert({2, "Charlie"});
for (auto& [k, v] : scores)  // 输出: 1:Alice, 2:Charlie, 3:Bob
    std::cout << k << ":" << v << " ";

// 范围查询
auto it = scores.lower_bound(2);  // 第一个 >= 2 的元素
auto it2 = scores.upper_bound(3); // 第一个 > 3 的元素

// unordered_map：哈希，更快
std::unordered_map<int, std::string> fast;
```

## 迭代器稳定性

| 操作 | `map`/`set` | `unordered_map`/`unordered_set` |
|------|-------------|----------------------------------|
| 插入 | **不影响**已有迭代器 | 仅 rehash 时**全部**失效 |
| 删除 | 仅被删节点 | 仅被删节点（rehash 除外） |
| clear | 全部失效 | 全部失效 |

## map 的 operator[] 陷阱

```cpp
// operator[] 在 key 不存在时会 **插入默认构造的值**
std::map<std::string, int> m;
int v = m["nonexistent"];  // ❌ 插入了 {"nonexistent", 0}

// ✅ 使用 find 或 at() 来判断存在性
auto it = m.find("key");
if (it != m.end()) { /* 存在 */ }

int val = m.at("key");  // 不存在则抛 out_of_range
```

## 工程建议

```cpp
// ✅ 需要有序遍历 → map/set
// ✅ 需要范围查询 → map（lower_bound/upper_bound）
// ✅ 纯查找操作 → unordered_map（O(1) vs O(log N)）

// ✅ 自定义类型作为 key 时，map 需要 operator<
struct Key {
    int a, b;
    bool operator<(const Key& o) const {
        return std::tie(a, b) < std::tie(o.a, o.b);
    }
};

// ✅ emplace 避免临时对象
m.emplace(std::piecewise_construct,
          std::forward_as_tuple("key"),
          std::forward_as_tuple(42));

// ❌ 不要频繁在 map 和 unordered_map 之间切换——各有适用场景
// ❌ unordered_map 的 hash 碰撞攻击（C++11 后标准库使用随机 seed 缓解）
```

> **面试高频**：红黑树的旋转有几种？插入后如何恢复平衡？—— 插入时叔父节点的颜色决定是染色还是旋转。删除情况更复杂（4 种 case），但核心是"借兄弟节点来补足黑色数量"。

---

哈希表实现的关联容器对比详见 → [unordered_map Hash Table & Collision (哈希表与冲突)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/02-Associative%20Containers%20Internals%20(关联容器底层)%20⭐/02b-unordered_map%20Hash%20Table%20&%20Collision%20(哈希表与冲突).md)

---

## unordered map Hash Table (哈希表与冲突)

> [!abstract] 核心考点：哈希表结构（bucket + linked list）、rehash 策略、自定义哈希函数、碰撞解决

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

## 工程建议 · 延伸要点 2
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

## 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **
02-Associative Containers (关联容器)
**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
