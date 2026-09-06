---
tags:
  - cpp/stl
status: 🌱
---

# map & set Red-Black Tree — 红黑树与有序容器

> [!important] **核心考点**：红黑树的平衡规则、有序关联容器的底层实现、与哈希容器的选择

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
