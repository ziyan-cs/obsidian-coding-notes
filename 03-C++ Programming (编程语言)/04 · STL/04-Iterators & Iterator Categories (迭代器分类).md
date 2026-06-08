---
tags:
  - cpp
  - stl
---

> **核心考点**：迭代器分类是 STL 算法与容器的桥梁，决定了算法可以用于哪些容器

## 迭代器分类体系

```text
输入迭代器 (Input)
    |
前向迭代器 (Forward)
    |
双向迭代器 (Bidirectional)
    |
随机访问迭代器 (Random Access)
```

| 类别 | 能力 | 示例容器 |
|------|------|---------|
| **输入迭代器** | 只读、单遍扫描、`++it`、`*it` | `istream_iterator` |
| **输出迭代器** | 只写、单遍扫描、`++it`、`*it=val` | `ostream_iterator` |
| **前向迭代器** | 读+写、**多遍扫描** | `forward_list`、`unordered_map` |
| **双向迭代器** | 前向 + `--it` | `list`、`map`、`set` |
| **随机访问** | 双向 + `it+n`、`it-n`、`it[n]`、`it1-it2`、`<`/`>` | `vector`、`deque`、`array` |

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
| `vector` | pos 后全失效 | pos 后全失效 | 全失效（扩容时）| 全失效 | — |
| `deque` | 全失效 | 全失效 | 仅尾端操作不影响 | — | — |
| `list` | ❌ 不影响 | ❌ 仅被删元素 | ❌ 不影响 | — | — |
| `map`/`set` | ❌ 不影响 | ❌ 仅被删元素 | — | — | — |
| `unordered_map` | ❌ 不影响 | ❌ 仅被删元素 | — | — | **全失效** |

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

---

算法库与迭代器分类紧密相关，详见 → [Algorithm Library (算法库速查)](/03-C++%20Programming%20(编程语言)/04%20·%20STL/05-Algorithm%20Library%20(算法库速查).md)
