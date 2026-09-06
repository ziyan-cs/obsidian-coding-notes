---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 03-Iterators Algorithms and Adapters (迭代器算法与适配器)

> [!abstract] 阅读定位
>
> 本专题合并同一学习动作中的机制、边界与实践内容；以完整理解代替碎片记忆。

## Container Adapters (容器适配器)

> [!note] 本节重点心考点：stack/queue/priority_queue 都是适配器而非独立容器，底层容器可替换

## 适配器模式

容器适配器**封装**底层容器，只暴露特定接口：

| 适配器 | 默认底层容器 | 特点 |
|--------|-------------|------|
| `stack`（栈） | `deque` | LIFO |
| `queue`（队列） | `deque` | FIFO |
| `priority_queue`（优先队列） | `vector` | 大根堆（默认）|

## stack

```cpp
// 默认 deque 做底层，也可以指定 vector/list
std::stack<int> s;                      // deque 底层
std::stack<int, std::vector<int>> sv;   // vector 底层
std::stack<int, std::list<int>> sl;     // list 底层

// 接口
s.push(1);  s.pop();     // 入栈/出栈（pop 不返回值！）
s.top();                 // 访问栈顶
s.empty(); s.size();
```

**为什么默认用 deque 而不是 vector？**
- deque 的尾部操作 O(1) 且**不会触发重分配**
- vector 尾部也是 O(1) 但可能重分配（拷贝所有元素）
- list 尾部 O(1) 但内存开销大、缓存不友好

## queue

```cpp
std::queue<int> q;
q.push(1);  q.pop();    // 入队/出队（pop 不返回值！）
q.front();  q.back();   // 访问队首/队尾
```

**不支持迭代器遍历**— 只能逐个出队。

## priority_queue

```cpp
// 默认大根堆（最大元素在 top）
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(4);
pq.top();  // 4（最大元素）

// 小根堆
std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;

// 自定义比较
auto cmp = [](int a, int b) { return a > b; };
std::priority_queue<int, std::vector<int>, decltype(cmp)> custom_pq(cmp);
```

**底层原理**：

```text
vector 上的堆操作：
push → push_back + push_heap (sift-up O(log N))
pop → pop_heap (sift-down O(log N)) + pop_back
top → front (O(1))
```

**性能**：
| 操作 | 复杂度 |
|------|--------|
| `push` | O(log N) |
| `pop` | O(log N) |
| `top` | O(1) |

```cpp
// 高效更新：不直接提供修改元素 API
// 替代方案：push 新值 + lazy 忽略旧值
// 或自己操作底层容器（不推荐）

// 构建优先队列的 3 种方法对比：
// 1. 逐个 push：O(N log N) ← 一般不这么做
// 2. 用底层容器的迭代器构造：O(N)  ← heapify 构造
std::vector<int> data{3, 1, 4, 1, 5, 9};
std::priority_queue<int> pq(data.begin(), data.end());  // O(N) 构造
```

> [!tip]- **工程要点**：`priority_queue` **不是 stable 的**（相同优先级元素顺序不确定）。需要稳定优先队列时，可以用 `(priority, counter)` 做 pair 来解决。

---

STL 容器全览详见 → [STL Cheat Sheet (STL速查总览)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/06-STL%20Cheat%20Sheet%20(STL速查总览).md)

---

## Iterators and Categories (迭代器分类)

> [!note] 本节重点心考点：迭代器分类是 STL 算法与容器的桥梁，决定了算法可以用于哪些容器

## 迭代器分类体系

```text
输入迭代器 (Input)
    |
前向迭代器 (Forward)
    |
双向迭代器 (Bidirectional)
    |
随机访问迭代器 (Random Access)
    |
连续迭代器 (Contiguous, C++20)
```

| 类别 | 能力 | 示例容器 |
|------|------|---------|
| **输入迭代器** | 只读、单遍扫描、`++it`、`*it` | `istream_iterator` |
| **输出迭代器** | 只写、单遍扫描、`++it`、`*it=val` | `ostream_iterator` |
| **前向迭代器** | 读+写、**多遍扫描** | `forward_list`、`unordered_map` |
| **双向迭代器** | 前向 + `--it` | `list`、`map`、`set` |
| **随机访问** | 双向 + `it+n`、`it-n`、`it[n]`、`it1-it2`、`<`/`>` | `vector`、`deque`、`array` |
| **连续迭代器** | 随机访问 + 元素在内存中连续 | `vector`、`array`、`string` |

## 迭代器标签与算法分发

```cpp
// 迭代器通过 tag 标识自己的类别
namespace std {
    struct input_iterator_tag {};
    struct forward_iterator_tag : input_iterator_tag {};
    struct bidirectional_iterator_tag : forward_iterator_tag {};
    struct random_access_iterator_tag : bidirectional_iterator_tag {};
}

// 算法利用 tag dispatch 选择最优实现
template<typename Iter, typename Dist>
void advance_impl(Iter& it, Dist n, std::random_access_iterator_tag) {
    it += n;  // O(1)
}

template<typename Iter, typename Dist>
void advance_impl(Iter& it, Dist n, std::bidirectional_iterator_tag) {
    while (n > 0) { ++it; --n; }  // O(N)
    while (n < 0) { --it; ++n; }
}

template<typename Iter, typename Dist>
void advance(Iter& it, Dist n) {
    advance_impl(it, n,
        typename std::iterator_traits<Iter>::iterator_category());
}
```

## 迭代器失效速查

| 容器 | insert | erase | push_back | resize | rehash |
|------|--------|-------|-----------|--------|--------|
| `vector` | 扩容则全失效；否则 `pos` 及之后失效 | 被删位置及之后失效 | 扩容则全失效；否则已有迭代器仍有效 | 可能因扩容全失效 | — |
| `deque` | 规则较复杂，保守视为可能失效 | 被删位置外的失效规则也与位置相关 | `push_front/back` 可能使迭代器失效；不要跨操作长期保存 | — | — |
| `list` | ❌ 不影响 | ❌ 仅被删元素 | ❌ 不影响 | — | — |
| `map`/`set` | ❌ 不影响 | ❌ 仅被删元素 | — | — | — |
| `unordered_map` | 不 rehash 时通常不影响；rehash 时全失效 | 仅被删元素失效 | — | — | **全失效** |

## 反向迭代器

```cpp
// rbegin() → 最后一个元素
// rend()   → 第一个元素之前
std::vector<int> v{1, 2, 3, 4, 5};
for (auto it = v.rbegin(); it != v.rend(); ++it)
    std::cout << *it;  // 54321

// base() 获取反向迭代器对应的正向迭代器
auto rit = v.rbegin();
auto fit = rit.base();  // fit 指向 rit 的下一个位置（正向视角）
```

## 迭代器适配器

| 适配器 | 作用 |
|--------|------|
| `reverse_iterator` | 反向遍历 |
| `back_insert_iterator` | 调用 `push_back` 插入 |
| `front_insert_iterator` | 调用 `push_front` 插入 |
| `insert_iterator` | 调用 `insert` 插入 |
| `istream_iterator` | 流输入作为迭代器 |
| `ostream_iterator` | 迭代器输出到流 |
| `move_iterator` | 迭代器解引用返回右值引用 |

```cpp
// 实用示例：拷贝到容器的后面
std::vector<int> src{1, 2, 3}, dst;
std::copy(src.begin(), src.end(),
          std::back_inserter(dst));  // push_back 自动调用

// 从流读取
std::istream_iterator<int> in(std::cin), end;
std::vector<int> data(in, end);
```

> **面试重点**：为什么 `list::sort` 不使用标准 `std::sort`？因为 `std::sort` 需要**随机访问迭代器**（它使用快速排序/内省排序），而 `list` 只提供双向迭代器，所以 `list` 自带了基于归并排序的 `list::sort()`。

## 30 秒回答

**迭代器失效如何避免？** 先看操作会不会重分配或 `rehash`；对 `vector`，插入/扩容后不要继续使用旧迭代器；遍历删除时使用 `it = container.erase(it)` 接住返回值。容器的精确规则随操作和位置不同，拿不准时查对应容器文档，不要套用“某容器永不失效”的口诀。

**自测：** 为什么 `std::sort(list.begin(), list.end())` 不可行？`vector::push_back` 在什么条件下会使所有迭代器失效？

---

算法库与迭代器分类紧密相关，详见 → [Algorithm Library (算法库速查)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/05-Algorithm%20Library%20(算法库速查).md)

---

## Algorithm Library (算法库速查)

> [!note] 本节重点心考点：STL 算法的分类、迭代器要求、Lambda 配合使用

## 分类速查

### 不修改序列的操作

| 算法 | 作用 | 迭代器要求 |
|------|------|-----------|
| `find` / `find_if` / `find_if_not` | 线性查找 | Input |
| `count` / `count_if` | 计数 | Input |
| `search` / `search_n` | 子序列查找 | Forward |
| `equal` | 区间相等比较 | Input |
| `mismatch` | 首个不匹配位置 | Input |
| `adjacent_find` | 连续相等元素 | Forward |
| `all_of` / `any_of` / `none_of` | 区间谓词判断 | Input |
| `for_each` | 对每个元素执行操作 | Input |

### 修改序列的操作

| 算法 | 作用 | 迭代器要求 |
|------|------|-----------|
| `copy` / `copy_if` / `copy_n` | 复制区间 | Input → Output |
| `move` | 移动区间 | Input → Output |
| `fill` / `fill_n` | 赋值 | Output |
| `generate` / `generate_n` | 生成赋值 | Output |
| `transform` | 一元/二元变换 | Input → Output |
| `replace` / `replace_if` | 替换元素 | Forward |
| `remove` / `remove_if` | 移除（**不是真的删除**）| Forward |
| `unique` | 去重（相邻相等）| Forward |
| `reverse` | 反转 | Bidirectional |
| `rotate` | 旋转区间 | Forward |
| `shuffle` | 随机打乱 | RandomAccess |
| `sample` | 随机采样 (C++17) | Forward |

> **重要**：`remove` 不删除元素，它把元素移到末尾返回新的 logical end，然后需要调用 `erase`（即 **erase-remove idiom**）。

### 排序与二分

| 算法 | 作用 | 迭代器要求 |
|------|------|-----------|
| `sort` | 排序（introsort：快排+堆排+插排）| RandomAccess |
| `stable_sort` | 稳定排序（归并）| RandomAccess |
| `partial_sort` | 只排前 N 个（heap select）| RandomAccess |
| `nth_element` | 找出第 N 大元素（quickselect）| RandomAccess |
| `lower_bound` | 第一个 >= val | Forward（有序区间）|
| `upper_bound` | 第一个 > val | Forward（有序区间）|
| `binary_search` | 二分查找是否存在 | Forward（有序区间）|
| `partition` | 分区（不稳定）| Forward |
| `stable_partition` | 稳定分区 | Bidirectional |

### 集合操作（有序区间）

| 算法 | 作用 |
|------|------|
| `merge` | 合并两个有序区间 |
| `set_union` | 并集 |
| `set_intersection` | 交集 |
| `set_difference` | 差集 |
| `includes` | 子集判断 |
| `inplace_merge` | 原地归并 |

### 堆操作

| 算法 | 作用 |
|------|------|
| `make_heap` | 将区间转为堆 (O(N)) |
| `push_heap` | 插入元素到堆 |
| `pop_heap` | 弹出堆顶 |
| `sort_heap` | 堆排序 |
| `is_heap` / `is_heap_until` | 检查是否为堆 |

### 最值与排列

| 算法 | 作用 |
|------|------|
| `min` / `max` / `minmax` | 两个/初始化列表的最值 |
| `min_element` / `max_element` | 区间最值 |
| `clamp` (C++17) | 限制值范围 |
| `next_permutation` / `prev_permutation` | 全排列迭代 |
| `lexicographical_compare` | 字典序比较 |

## Erase-Remove Idiom（核心模式）

```cpp
// remove 不会真正删除元素！
std::vector<int> v{1, 2, 3, 2, 5};

// remove 把不等于 2 的元素移到前面，返回新 logical end
auto new_end = std::remove(v.begin(), v.end(), 2);
// v 现在可能是 {1, 3, 5, ?, ?}，返回指向第一个 "?" 的迭代器

// 真正删除
v.erase(new_end, v.end());

// 或一句话：
v.erase(std::remove(v.begin(), v.end(), 2), v.end());
```

## 自定义比较与 Lambda

```cpp
struct Person { std::string name; int age; };

// sort with lambda
std::vector<Person> people{{"Alice",30}, {"Bob",25}};
std::sort(people.begin(), people.end(),
    [](const Person& a, const Person& b) {
        return a.age < b.age;
    });

// 多字段比较
std::sort(people.begin(), people.end(),
    [](const Person& a, const Person& b) {
        return std::tie(a.age, a.name) < std::tie(b.age, b.name);
    });

// 投影排序 (C++20 ranges 更优雅)
// 但普通 STL 需要用 lambda 手动包装
```

## 算法性能指南

| 算法 | 复杂度 | 注意 |
|------|--------|------|
| `sort` | O(N log N) | 平均性能最好 |
| `stable_sort` | O(N log N) | 比 sort 慢（需要额外内存）|
| `nth_element` | O(N) | 只排第 N 个，不是全排序 |
| `partial_sort` | O(N log K) | K=已排序前缀大小 |
| `lower_bound` | O(log N) | 必须有序区间 |
| `find` | O(N) | 无序区间只能用这个 |

> [!tip]- **工程要点**：
> - `std::sort` 在元素数量少时（<16）切换到插入排序（introsort 的优化）
> - `std::find` 不要用在已排序区间上——`lower_bound` 快得多
> - C++17 引入并行策略：`std::sort(std::execution::par, v.begin(), v.end())`

---

STL 容器与算法速查详见 → [STL Cheat Sheet (STL速查总览)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/06-STL%20Cheat%20Sheet%20(STL速查总览).md)

---

## STL Reference (STL 速查总览)

> [!note] 本节重点心考点：容器选择决策、复杂度一览、面试前快速复习用

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

容器适配器与算法详见 → [Container Adapters (容器适配器)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/03-Container%20Adapters%20(容器适配器).md) · [Algorithm Library (算法库速查)](/02-C++%20Backend%20(C++%20后端)/04-STL%20(标准模板库)/05-Algorithm%20Library%20(算法库速查).md)



## 零基础阅读路径

先阅读对象、内存或资源的“谁创建、谁拥有、何时销毁”部分；然后看语法和代码；最后才看性能、底层布局或面试延伸。任何代码先在编译器中跑最小版本。

## 常见误区

- 只背语言规则而不追问对象生命周期、所有权、异常路径或并发边界，容易在真实代码中误用。
- 不用编译器警告、单元测试、sanitizer 或小型实验验证，就把经验结论当作 C++ 规则。

## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **03-Iterators Algorithms and Adapters (迭代器算法与适配器)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
