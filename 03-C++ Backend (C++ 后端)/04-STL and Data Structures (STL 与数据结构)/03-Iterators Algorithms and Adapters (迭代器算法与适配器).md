---
study_stage: backlog
---


> [!abstract] 学习目标
> 能根据迭代器能力判断算法是否可用；知道算法的前提条件与复杂度；使用适配器和 ranges 时不误判底层容器、顺序或生命周期。

# 适配器：只暴露需要的队列语义

`std::stack`（LIFO）和 `std::queue`（FIFO）默认包装 `deque`；`std::priority_queue` 默认以 `vector` 管理堆，`top()` 是最高优先级元素。这些类型**不提供遍历底层容器的公开迭代器**。调用 `top`、`front` 或 `pop` 前先确认非空；`pop` 只移除，不返回元素。

| 适配器 | 主要操作 | 复杂度要点 |
| --- | --- | --- |
| `stack` | `push` / `top` / `pop` | 默认底层两端操作为常数级 |
| `queue` | `push` / `front` / `pop` | 默认底层两端操作为常数级 |
| `priority_queue` | `push` / `top` / `pop` | 插入/弹出 O(log N)，读取顶部 O(1) |

```cpp
#include <cassert>
#include <cstddef>
#include <functional>
#include <queue>
#include <stack>
#include <vector>

struct Task { int priority; std::size_t arrival; };
struct LowerPriority {
    bool operator()(const Task& a, const Task& b) const {
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.arrival > b.arrival; // 同优先级：先到者在顶部
    }
};

int main() {
    std::stack<int> undo;
    undo.push(1);
    assert(undo.top() == 1);
    undo.pop();

    std::queue<int> pending;
    pending.push(2);
    assert(pending.front() == 2);
    pending.pop();

    std::vector<int> input{3, 1, 4, 2};
    std::priority_queue<int> maximum(input.begin(), input.end());
    assert(maximum.top() == 4); // 范围构造可用堆化，O(N)

    std::priority_queue<Task, std::vector<Task>, LowerPriority> tasks;
    tasks.push({10, 1});
    tasks.push({10, 0});
    assert(tasks.top().arrival == 0);
}
```

`priority_queue` 对同优先级元素**不保证稳定顺序**。上例把到达序号纳入比较规则，才得到“同优先级先到先处理”；实际系统还要处理序号溢出与任务更新。无“原地更新某个任务”的通用公开接口，可重建堆或插入新版本并在取出时核对版本。

# 迭代器能力决定算法能做什么

| 能力 | 代表操作 | 常见来源 |
| --- | --- | --- |
| 输入（input） | 单遍读取、前进 | `istream_iterator` |
| 输出（output） | 单遍写入、前进 | `back_insert_iterator` |
| 前向（forward） | 可多遍前进 | `forward_list`、无序容器 |
| 双向（bidirectional） | 再支持后退 | `list`、`map` |
| 随机访问（random access） | `it+n`、距离计算 | `vector`、`deque` |
| 连续（contiguous，C++20） | 随机访问且元素连续 | `vector`（`bool` 特化除外）、`array` |

能力不是“输入 → 输出 → 前向”的单链：输出迭代器侧重写，不能简单当作可读的前向迭代器。`std::sort` 需要随机访问，`list` 只有双向迭代器，因此用 `list::sort()`。`std::advance(it,n)` 对随机访问迭代器可常数前进，对链表通常要走 `|n|` 步；不要在链表循环中反复把它当 O(1) 下标。

`reverse_iterator::base()` 指向反向迭代器所指元素的**下一个**正向位置；反向遍历与正向 `erase` 混用时要重新算边界。对任何容器，`end()` 都是尾后位置，不能解引用。

## 失效规则属于容器和操作，不属于算法名

算法通常通过迭代器读写元素，容器结构修改则可能让旧迭代器失效。最容易混淆的几条：

- `vector` 重分配使所有旧迭代器、引用、指针失效；未重分配的尾插仍使旧 `end()` 失效。
- `deque` 两端插入使**所有旧迭代器**失效，但原有元素引用仍有效；中间插入使二者都失效。
- `list`、`map` 插入不使现存元素迭代器失效；擦除只使被删元素失效。
- `unordered_map` 的 `rehash` 使迭代器失效，但不使仍存在元素的引用失效。

详细操作表见 [01-Sequence Containers (顺序容器)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/01-Sequence%20Containers%20(顺序容器).md) 与 [02-Associative Containers (关联容器)](/03-C%2B%2B%20Backend%20(C%2B%2B%20后端)/04-STL%20and%20Data%20Structures%20(STL%20与数据结构)/02-Associative%20Containers%20(关联容器).md)；这里不再复制一套容易漂移的速查表。

# 常用算法：先检查区间与前提

| 任务 | 算法 | 前提与复杂度 |
| --- | --- | --- |
| 顺序找元素 | `find` / `find_if` | 输入迭代器；线性扫描 |
| 排序 | `sort` | 随机访问；O(N log N) 比较；不稳定 |
| 保持同键原有次序 | `stable_sort` | 随机访问；通常可能用额外内存 |
| 只找第 k 个位置 | `nth_element` | 随机访问；平均线性比较，两侧不保证有序 |
| 找有序区间第一个不小于目标值 | `lower_bound` | 前向迭代器；区间需按目标谓词分区 |
| 从有序区间去相邻重复 | `unique` | 只整理相邻重复，不改变容器大小 |
| 删除满足条件的元素 | `remove_if` + `erase` | 前者移动保留元素，后者真正缩短容器 |

对排序、堆与关联容器，比较器应满足**严格弱序**；`a <= b` 不适合作为 `sort` 的比较器。`lower_bound` 的最小前提是区间对该目标值已分区，整体排序是常见且更易维护的充分条件。非随机访问迭代器也能用 `lower_bound`，比较次数是对数，但前进步数可能是线性的；对 `map` 查键范围优先用成员 `lower_bound`。

```cpp
#include <algorithm>
#include <cassert>
#include <vector>

int main() {
    std::vector<int> values{5, 2, 3, 2, 1};
    auto new_end = std::remove(values.begin(), values.end(), 2);
    values.erase(new_end, values.end()); // 真正删除尾段
    std::sort(values.begin(), values.end());
    assert((values == std::vector<int>{1, 3, 5}));

    auto first_ge_3 = std::lower_bound(values.begin(), values.end(), 3);
    assert(first_ge_3 != values.end() && *first_ge_3 == 3);

    auto middle = values.begin() + 1;
    std::nth_element(values.begin(), middle, values.end());
    assert(*middle == 3); // 两边不保证各自排序
}
```

`remove` 不改变容器大小，返回新的逻辑末尾；该位置之后的元素仍有有效但值不指定的状态，不能假定都是旧值或已经析构。C++20 对 `vector` 等提供 `std::erase` / `std::erase_if` 便捷接口，但理解 erase-remove 仍有助于理解算法与容器职责分离。

## 插入迭代器负责增长目标容器

`std::copy` 只写向提供的输出位置；对空 `vector` 直接传 `dst.begin()` 不会自动扩容。使用 `std::back_inserter` 把赋值转换为 `push_back`：

```cpp
#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> source{1, 2, 3};
    std::vector<int> target;
    std::copy(source.begin(), source.end(),
              std::back_inserter(target));
    assert(target == source);
}
```

# C++20 ranges：组合视图，同时留意借用寿命

`views::filter` 等视图一般惰性求值，不为筛出的元素自动创建拥有副本。下面的 `evens` 观察 `values`；`values` 必须活得足够久，也不要在遍历视图时改变使迭代器失效的底层容器结构。

```cpp
#include <cassert>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3, 4};
    auto evens = values | std::views::filter(
        [](int x) { return x % 2 == 0; });
    int sum = 0;
    for (int x : evens) sum += x;
    assert(sum == 6);
}
```

不是所有 ranges 算法都惰性：`std::ranges::sort(values)` 会立即修改 `values`。区分“算法”与“视图”，同时检查返回迭代器是否在输入临时对象销毁后仍有效。对过滤视图中的元素做会让它不再满足谓词的修改，还可能违反其迭代语义。

参考：[算法排序前提](https://eel.is/c++draft/alg.sorting.general)、[二分查找前提](https://eel.is/c++draft/alg.binary.search)、[remove 尾段状态](https://eel.is/c++draft/alg.remove)。
