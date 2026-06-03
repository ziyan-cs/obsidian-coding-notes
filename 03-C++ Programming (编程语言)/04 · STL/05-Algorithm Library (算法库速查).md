> **核心考点**：STL 算法的分类、迭代器要求、Lambda 配合使用

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

> **工程要点**：
> - `std::sort` 在元素数量少时（<16）切换到插入排序（introsort 的优化）
> - `std::find` 不要用在已排序区间上——`lower_bound` 快得多
> - C++17 引入并行策略：`std::sort(std::execution::par, v.begin(), v.end())`
