---
tags:
  - cpp/stl
status: 🌱
---

> [!important] **核心考点**：动态数组的连续内存布局、扩容策略、迭代器失效场景、与 `std::array`/原始数组的抉择

## 内存布局与核心特性

`vector` 管理一块连续的堆内存，由三个指针组成（typical impl）：

```cpp
// 伪代码：典型 vector 内部结构
struct vector_internals {
    T* _start;       // 数据起始
    T* _finish;      // 已构造元素末尾
    T* _end_of_storage; // 已申请内存末尾
};
// size() = _finish - _start
// capacity() = _end_of_storage - _start
```

**连续内存是核心优势**：Cache locality 极好，迭代、随机访问都是 O(1)。

## 扩容策略（Reallocation）

当 `size() == capacity()` 时 `push_back` 触发扩容：

```cpp
// 典型扩容流程
void push_back(const T& val) {
    if (_finish == _end_of_storage) {
        size_t new_cap = grow();  // 计算新容量
        T* new_buf = allocate(new_cap);
        move_or_copy(_start, _finish, new_buf);  // 转移已有元素
        deallocate(_start);
        _start = new_buf;
        _finish = new_buf + old_size;
        _end_of_storage = new_buf + new_cap;
    }
    construct(_finish, val);
    ++_finish;
}
```

**扩容倍数对比（工程选择）**：

| 策略 | 实现 | 均摊 O(1) | 内存浪费 | 典型使用者 |
|------|------|-----------|----------|-----------|
| 固定增量 | `cap += N` | ❌ O(N) | 低 | — |
| 1.5x | `cap += cap/2` | ✅ | ~33% | **MSVC / GCC (prior)** |
| 2x | `cap *= 2` | ✅ | ~50% | **Visual Studio** |

> **工程选择**：1.5x 比 2x 更节省内存且可以复用旧内存块的碎片。GCC 5.0+ 改用 1.5x 策略。

## 关键操作与复杂度

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `operator[]` / `at()` | O(1) | `at()` 带边界检查（抛异常） |
| `push_back` | **均摊** O(1) | 扩容时 O(N)，极少数次 |
| `pop_back` | O(1) | 不释放内存，只析构 |
| `insert(pos)` | O(N) | 后续元素全部后移 |
| `erase(pos)` | O(N) | 后续元素全部前移 |
| `emplace_back` | **均摊** O(1) | 原地构造，避免拷贝 |

## 迭代器失效（面试重点）

哪些操作会让迭代器**失效**：

| 操作 | 失效范围 | 原因 |
|------|---------|------|
| `push_back` / `emplace_back` | **全部**迭代器 | 可能触发重分配 |
| `insert` / `erase` | **插入/删除位置之后**全部失效 | 元素移动 |
| `reserve` | **全部**迭代器 | 可能触发重分配 |
| `resize(n)` (n > capacity) | **全部** | 触发重分配 |
| `resize(n)` (n <= capacity) | 仅超出元素失效 | 仅析构超出部分 |
| `pop_back` | 仅被删除元素及 `end()` | — |

```cpp
// ❌ 错误：扩容导致迭代器失效
std::vector<int> v{1, 2, 3};
auto it = v.begin();
v.push_back(4);  // 可能扩容，it 悬空
*it = 10;        // 未定义行为！

// ✅ 正确：预留足够容量
v.reserve(100);
auto it2 = v.begin();
v.push_back(42);  // 不会扩容
*it2 = 10;        // ✅ 安全
```

## 工程最佳实践

```cpp
// ✅ 预分配避免频繁扩容
std::vector<int> v;
v.reserve(1000);  // 已知大约数量时提前预留

// ✅ emplace_back 优于 push_back（避免临时对象）
v.emplace_back(args...);

// ✅ shrink_to_fit 释放多余内存（O(N) 拷贝, 谨慎使用）
v.shrink_to_fit();

// ❌ 不要把 vector<bool> 当普通 vector 用
// vector<bool> 是位压缩特化，operator[] 返回代理对象而非引用
```

> **面试常见题**：为什么 `vector` 扩容不能原地 realloc？因为底层是 C++ 对象构造/析构，类型非平凡时不能像 `realloc` 一样直接拷字节。T 有 non-trivial 构造函数时，必须移动构造。

## vector vs 其他容器

| 场景 | 推荐 |
|------|------|
| 随机访问为主 | `vector` |
| 频繁头插/头删 | `deque` 或 `list` |
| 频繁中间插入 | `list` 或 `deque` |
| 大小固定且编译期已知 | `std::array` |
| 需要稳定迭代器 | `list`（插入删除不影响已有迭代器） |

---

其他序列容器对比详见 → [deque Block-based Storage (分块存储)](/03-C++%20Programming%20(编程语言)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01b-deque%20Block-based%20Storage%20(分块存储).md) · [list Doubly Linked List (双向链表)](/03-C++%20Programming%20(编程语言)/04-STL%20(标准模板库)/01-Sequence%20Containers%20Internals%20(序列容器底层)/01c-list%20Doubly%20Linked%20List%20(双向链表).md)
