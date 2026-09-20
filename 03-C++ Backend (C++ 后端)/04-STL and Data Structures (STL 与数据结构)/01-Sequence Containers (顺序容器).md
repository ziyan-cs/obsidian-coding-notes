---
study_stage: backlog
---


> [!abstract] 学习目标
> 能按“连续内存、两端操作、节点稳定性”选容器；能对每次插删指出迭代器、引用与指针各自是否仍有效。以下以 C++17 为基线。

# 先选数据布局，而不是背容器排名

| 容器 | 存储与访问 | 擅长的操作 | 主要代价 |
| --- | --- | --- | --- |
| `std::array<T,N>` | 固定大小、连续存储，随机访问 O(1) | 大小编译期已知、无需动态增长 | 容量不能变 |
| `std::vector<T>` | 可增长的连续存储，随机访问 O(1) | 顺序遍历、尾部追加、与连续内存 API 交互 | 中间插删移动后续元素；扩容使旧地址失效 |
| `std::deque<T>` | 分段存储，随机访问 O(1) | 两端增删 | 不能当一整块数组使用；迭代器规则不同于引用 |
| `std::list<T>` | 双向节点链，双向迭代 | 已知位置插删、节点转移、稳定元素地址 | 查位置 O(N)，每节点额外存储与分配 |

不存在固定的“少于 100 个元素必用 vector”或“list 遍历一定慢十倍”。先根据接口要求排除不合适容器，再用实际元素大小、操作分布和负载测量常数成本。

# `vector`：连续存储与重分配

`size()` 是已构造元素数，`capacity()` 是不需重新分配就能容纳的元素数，`data()` 指向连续元素区。标准保证连续存储、尾部追加均摊常数复杂度，但**不规定容量每次增长 1.5 倍还是 2 倍**。一次扩容可能移动或复制既有元素；`emplace_back` 仅在尾部直接构造新元素，不保证扩容时没有移动/复制。

| 操作 | 复杂度 | 失效规则的重点 |
| --- | --- | --- |
| `push_back` / `emplace_back` | 均摊 O(1)，扩容时 O(N) | 重新分配：全部迭代器、引用、指针失效；否则旧元素有效，旧 `end()` 失效 |
| `reserve(n)` | 最多 O(N) | 仅当发生重新分配时使全部失效；不改变 `size()` |
| `insert(pos)` | 通常 O(N) | 重新分配则全部失效；否则插入点及之后的迭代器/引用失效 |
| `erase(pos)` | O(N) | 删除点及之后的迭代器/引用失效，包含旧 `end()` |
| `pop_back` | O(1) | 被删除元素和旧 `end()` 失效 |
| `resize` | 可能 O(N) | 增长且重分配则全失效；缩小时被删元素失效；旧 `end()` 需重取 |

以下示例先 `reserve` 再取得迭代器，且追加后仍未超过容量，因此旧元素迭代器有效。若把 `reserve` 放在保存迭代器**之后**，它反而可能使该迭代器失效。

```cpp
#include <cassert>
#include <vector>

int main() {
    std::vector<int> values;
    values.reserve(4);
    values.push_back(1);
    auto first = values.begin();
    values.push_back(2); // size <= capacity，无重分配
    assert(*first == 1);

    for (auto it = values.begin(); it != values.end();) {
        if (*it % 2 == 0) it = values.erase(it);
        else ++it;
    }
    assert(values.size() == 1 && values[0] == 1);
}
```

已知大致规模时 `reserve` 可减少重分配，但过度预留浪费内存；`shrink_to_fit` 是**非强制请求**，不能写成“调用就会释放容量”。`vector<bool>` 是标准特化，其元素访问可能是代理而非 `bool&`；需要真正的独立 `bool` 引用时不要用它。

## 为何不能用 `realloc` 搬运一般对象

非平凡 C++ 对象有构造、移动、析构和异常安全规则；把其字节直接挪到新地址不能代替这些操作。学习 `vector` 扩容时记住“申请新存储 → 构造新元素/转移旧元素 → 成功后销毁旧元素并释放旧存储”，但具体顺序和异常处理由实现与元素类型决定，不能把概念流程当源码布局。

# `deque`：两端操作与迭代器陷阱

`deque` 支持随机访问以及两端单元素常数时间增删。常见实现用多个块与索引结构，但块大小、指针层数、增长倍数不是标准保证；不能把“每块 512 字节”当可移植事实。

尤其要区分**迭代器**和**元素引用**：

| 操作 | 旧迭代器 | 旧元素引用/指针 |
| --- | --- | --- |
| 两端插入一个元素 | 全部失效 | 指向原有元素的仍有效 |
| 中间插入 | 全部失效 | 全部失效 |
| 擦除首元素但不擦最后元素 | 仅被删元素失效 | 仅被删元素失效 |
| 擦除最后元素 | 被删元素和旧 `end()` 失效 | 仅被删元素失效 |
| 擦除中间元素 | 全部失效 | 全部失效 |

```cpp
#include <cassert>
#include <deque>

int main() {
    std::deque<int> queue{1, 2};
    int& original = queue.front();
    [[maybe_unused]] auto old_begin = queue.begin();
    queue.push_back(3);
    assert(original == 1); // 原元素引用仍有效
    // old_begin 已失效，不能再比较、解引用或递增
    assert(queue[1] == 2);
}
```

想继续遍历时，插入后重新取得 `begin()` / `end()`。`deque` 不是一整块连续数组，不能把 `&d[0]` 当成长度为 `d.size()` 的 C 数组传入接口。若主要需求是连续扫描而非双端操作，应先考虑 `vector` 并基准验证。

# `list`：稳定节点与已知位置的 O(1)

`list` 的插入和擦除不会使**其他元素**的迭代器、引用失效。给定正确位置后，单元素插入或擦除是常数时间；如果要先按值查找该位置，查找仍是 O(N)。没有随机访问迭代器，因此不能写 `list[i]` 或交给 `std::sort`；使用 `list::sort`。

```cpp
#include <cassert>
#include <list>

int main() {
    std::list<int> left{1, 3, 5};
    std::list<int> right{2, 4, 6};
    auto keep = left.begin();       // 指向 1
    left.merge(right);              // 两边已按同一比较器排序
    assert(right.empty() && *keep == 1);

    std::list<int> extra{9};
    left.splice(left.end(), extra); // 整表转移；extra 变空
    assert(extra.empty() && *keep == 1);
    left.sort();
}
```

`splice` 转移节点而非复制元素，但复杂度要看重载：整表或单节点转移为常数时间；**跨容器区间**转移通常要线性计数。不同 list 的分配器不相等时，不能随意拼接。`merge` 要求两个链表都已按同一比较关系排序；上例先归并再拼接 `9`，最后重新排序。`list::unique` 仅移除**相邻**重复值，不是全局去重。

在需要长期保存元素地址、从已知位置频繁转移节点时 `list` 可能合适；仅因“中间插入 O(1)”就选它，常忽略了寻找位置、分配和遍历成本。

参考：[`vector` 容量](https://eel.is/c++draft/vector.capacity)、[`deque` 插删失效规则](https://eel.is/c++draft/deque.modifiers)、[`list` 节点操作](https://eel.is/c++draft/list.ops)。
