---
tags:
  - cpp/stl
status: 🌱
---

# Iterators & Iterator Categories — 迭代器分类

> [!important] **核心考点**：迭代器分类是 STL 算法与容器的桥梁，决定了算法可以用于哪些容器

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

算法库与迭代器分类紧密相关，详见 → [Algorithm Library (算法库速查)](/03-C++%20Programming%20(编程语言)/04-STL%20(标准模板库)/05-Algorithm%20Library%20(算法库速查).md)
