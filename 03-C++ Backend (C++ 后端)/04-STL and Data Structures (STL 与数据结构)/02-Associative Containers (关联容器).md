---
study_stage: backlog
---

> [!abstract] 学习目标
> 根据顺序与范围查询需求选择容器；解释复杂度前提、键等价关系，以及 `rehash` 后迭代器和引用为何不同。

# 有序与无序：先按接口语义选择

| 类型 | 关键语义 | 查找、插入、删除 |
| --- | --- | --- |
| `map` / `set` | 按比较器顺序遍历，可做键范围查询 | O(log N) |
| `unordered_map` / `unordered_set` | 不保证遍历顺序，按哈希与相等关系找键 | 平均 O(1)，碰撞时可能退化 |
| `multi...` 版本 | 允许一个等价键出现多次 | 按所属有序/无序家族的规则 |

`map` 常以红黑树实现，但标准**没有指定树种或旋转次数**。`unordered_map` 常用桶加节点，但标准也不规定“桶一定是链表”或取模公式。先看需求是否需要有序遍历、`lower_bound`/`upper_bound`，再比较实际负载中的性能与内存成本。

## 常见实现直觉：红黑树为何能保持对数高度

红黑树是一种在 BST 顺序不变量之外加入颜色约束的平衡树。常见表述为：节点非红即黑；根与空叶（NIL）为黑；红节点不能有红孩子；从任一节点到其下方 NIL 的路径具有相同黑节点数。根为黑也可在更新结束时统一恢复。

这些约束允许局部不完全平衡，但禁止连续红节点；最长根叶路径不会超过最短路径的两倍，树高因而为 `O(log N)`。插入通常先把新节点设为红：若父节点为黑则结束；若父节点为红，再按叔节点颜色选择变色或旋转并向上修复。删除修复更复杂，应先掌握 BST 删除与黑高度为何可能改变，再研究具体 case，不必把某个库的旋转次数当标准保证。

这解释了**常见实现**怎样满足有序容器的对数复杂度；代码不能依赖其节点布局。先修 [05-Binary Search Trees (二叉搜索树)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/05-Binary%20Search%20Trees%20(二叉搜索树).md) 中的 BST/AVL 高度概念，再回来对比红黑树更宽松的平衡约束。

## 有序容器：比较器同时定义等价

`map` 默认使用 `std::less<Key>`；自定义比较器可改变遍历顺序。若 `!comp(a,b) && !comp(b,a)`，两个键被视为等价；比较器必须满足严格弱序，不能随外部状态随意变化。

```cpp
#include <cassert>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> stock;
    stock.try_emplace("apple", 3); // 已有键不会覆盖
    stock.insert_or_assign("pear", 5);
    stock["apple"] += 1;          // 缺键时会插入默认值
    assert(stock.at("apple") == 4);

    auto first = stock.lower_bound("banana");
    auto last = stock.upper_bound("pear");
    for (auto it = first; it != last; ++it)
        assert(it->first == "pear");

    assert(stock.find("orange") == stock.end());
}
```

`find` 不插入，`at` 缺键抛异常，`operator[]` 缺键则插入默认构造值。`map` 的键不能通过迭代器改坏排序；值可以改。有序容器插入不使旧迭代器/引用失效，擦除只使被删元素失效。

`map::lower_bound` 是成员操作。通用 `std::lower_bound(map.begin(), map.end(), ...)` 即使比较次数对数，双向迭代器的前进次数也可能线性，因此查键范围优先用成员函数。

# 无序容器：哈希与相等必须一致

相等的键必须得到相同哈希值；不同键允许碰撞。默认 `std::hash` 不保证随机种子或抗攻击能力。不可信键、大规模碰撞或错误的自定义哈希可能让平均常数复杂度退化。

```cpp
#include <cassert>
#include <cstddef>
#include <functional>
#include <unordered_map>

struct Point { int x, y; };
struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        auto x = std::hash<int>{}(p.x);
        auto y = std::hash<int>{}(p.y);
        return x ^ (y << 1); // 教学组合，不是抗碰撞哈希
    }
};
struct PointEqual {
    bool operator()(const Point& a, const Point& b) const noexcept {
        return a.x == b.x && a.y == b.y;
    }
};

int main() {
    std::unordered_map<Point, int, PointHash, PointEqual> cells;
    cells.reserve(100); // 为约 100 个元素准备容量，不是恰好 100 个桶
    cells.emplace(Point{1, 2}, 7);
    int& value = cells.at(Point{1, 2});
    [[maybe_unused]] auto old_iterator = cells.begin();
    cells.rehash(1000); // 旧迭代器失效；指向现存元素的引用仍有效
    value = 8;
    assert(cells.at(Point{1, 2}) == 8);
}
```

`load_factor()` 是元素数与桶数之比；`max_load_factor()` 影响扩桶。`reserve(n)` 以预期**元素数**为参数，`rehash(n)` 以桶数为参数。`rehash` 使所有迭代器失效，但不使现存元素的指针/引用失效；`erase` 只使被删元素的引用失效。不要在 `rehash` 后比较或解引用旧迭代器，即便特定实现看似还能用。

## 使用边界

- 范围查询或按键顺序输出：优先有序容器；不需要排序且频繁按键访问：比较哈希容器。
- 需要重复键时选 `multi...`，不要靠后插入覆盖旧值模拟多值。
- 并发修改并不因迭代器稳定就自动安全；容器的同步协议仍需单独设计。
- 对外部可控输入评估碰撞退化、内存上限和限流，不能假设标准库已经替你随机加盐。

参考：[有序关联容器要求](https://eel.is/c++draft/associative.reqmts.general)、[无序容器与 `rehash`](https://eel.is/c++draft/unord.req.general)。
