#cpp #stl #cheat-sheet #algorithms #containers

## ⚡ TL;DR（快速决策）

- **STL = 容器 + 迭代器 + 算法 + 函数对象 的可组合工具体系。**
- 三层结构：
    - 容器（存储）
    - 迭代器（访问）
    - 算法（处理）
- 目标：
    - 用 **统一迭代器接口** 连接不同容器
    - 用 **通用算法** 处理不同数据结构
    - 用 **抽象接口** 降低重复代码
- 如果只抓最重要的 20%，优先掌握：
    - `sort`
    - `lower_bound` / `upper_bound`
    - `binary_search`
    - `find`
    - `count`
    - `accumulate`
    - `min_element` / `max_element`
    - `reverse`
    - `unique`
    - `erase`
    - `next_permutation`
    - `copy`
    - `transform`
    - `push_back` / `emplace_back`
    - `map.find` / `unordered_map.find`
- 学习顺序建议：
    - 顺序容器高频接口
    - 算法库核心函数
    - 关联容器与组合套路


# 1. 容器高频通用操作

### 1.1 状态接口

- `size()`：返回元素个数
- `empty()`：判断是否为空
- `clear()`：清空容器
- `front()` / `back()`：访问头尾元素
- `begin()` / `end()`：返回迭代器区间

典型写法：

```cpp
if (!v.empty()) {
    cout << v.front() << ' ' << v.back() << '\n';
}
```

### 1.2 插入与删除

- `push_back()`：尾插元素
- `emplace_back()`：原地构造尾插
- `insert()`：在指定位置或按 key 插入
- `erase()`：删除位置 / 区间 / key
- `pop_back()` / `pop_front()`：弹出头尾元素（视容器而定）

典型写法：

```cpp
vector<pair<int,int>> edges;
edges.emplace_back(1, 2);
```

### 1.3 容器选择直觉

- `vector`：默认首选，顺序存储，随机访问快
- `deque`：头尾操作都方便
- `list`：链表，适合频繁中间插删，但随机访问差
- `set` / `map`：有序、自动维护排序，查找通常 $O(\log n)$
- `unordered_set` / `unordered_map`：哈希结构，平均查找通常 $O(1)$
- `queue` / `stack` / `priority_queue`：受限接口的容器适配器

### 1.4 高频提醒

- `vector` 没有 `pop_front()`
- `map[key]` 在 key 不存在时会插入默认值
- 需要“只查询不插入”时优先 `find()`
- 顺序容器 `erase()` 后迭代器可能失效


# 2. STL 算法总览

- 查找类：`find`、`count`、`binary_search`
- 二分边界类：`lower_bound`、`upper_bound`
- 排序类：`sort`、`stable_sort`、`nth_element`
- 最值类：`min`、`max`、`min_element`、`max_element`
- 变换类：`copy`、`transform`、`fill`
- 删除整理类：`remove`、`remove_if`、`unique`
- 反转重排类：`reverse`、`rotate`、`next_permutation`
- 数值类：`accumulate`、`iota`、`partial_sum`
- 集合类：`merge`、`set_union`、`set_intersection`
- 堆类：`make_heap`、`push_heap`、`pop_heap`

# 3. 最重要的算法函数 ⭐

### 3.1 查找与统计

#### `find`

- 作用：线性查找某个值
- 复杂度：通常 $O(n)$

```cpp
auto it = find(v.begin(), v.end(), x);
if (it != v.end()) {
    // 找到
}
```

#### `count`

- 作用：统计某个值出现次数

```cpp
int cnt = count(v.begin(), v.end(), x);
```

#### `count_if`

- 作用：统计满足条件的元素个数

```cpp
int cnt = count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; });
```

#### `all_of` / `any_of` / `none_of`

- 作用：整体判断区间是否全部 / 至少一个 / 一个都没有满足条件

```cpp
bool ok = all_of(v.begin(), v.end(), [](int x){ return x > 0; });
```

### 3.2 二分与有序区间

#### `lower_bound`

- 作用：返回第一个 `>= x` 的位置
- 前提：区间必须有序

```cpp
auto it = lower_bound(v.begin(), v.end(), x);
```

#### `upper_bound`

- 作用：返回第一个 `> x` 的位置

```cpp
auto it = upper_bound(v.begin(), v.end(), x);
```

#### `binary_search`

- 作用：判断有序区间中是否存在某值

```cpp
bool exists = binary_search(v.begin(), v.end(), x);
```

#### `equal_range`

- 作用：同时返回某值的左右边界区间

```cpp
auto [L, R] = equal_range(v.begin(), v.end(), x);
```

### 3.3 排序与选择

#### `sort`

- 作用：排序，默认升序

```cpp
sort(v.begin(), v.end());
sort(v.begin(), v.end(), greater<int>());
```

#### `stable_sort`

- 作用：稳定排序；相等元素的相对顺序保持不变
- 适合“先按次关键字排，再按主关键字稳定排序”的场景

#### `nth_element`

- 作用：把第 `n` 小元素放到正确位置，左右两边仅保证相对大小关系
- 典型用途：快速找第 k 小 / 第 k 大

```cpp
nth_element(v.begin(), v.begin() + k, v.end());
```

#### `partial_sort`

- 作用：只把前一部分排好
- 适合求前 k 小元素

#### `is_sorted`

- 作用：判断区间是否有序

### 3.4 最值与区间性质

#### `min` / `max`

```cpp
int a = min(x, y);
int b = max(x, y);
```

#### `min_element` / `max_element`

- 作用：返回最小 / 最大元素迭代器

```cpp
auto mn = min_element(v.begin(), v.end());
auto mx = max_element(v.begin(), v.end());
```

#### `minmax_element`

- 作用：一次得到最小和最大元素位置

#### `clamp`

- 作用：把值限制在某个区间内

```cpp
x = clamp(x, 0, 100);
```

### 3.5 变换与复制

#### `copy`

- 作用：复制区间

```cpp
copy(v.begin(), v.end(), back_inserter(ans));
```

#### `transform`

- 作用：按规则变换每个元素

```cpp
transform(v.begin(), v.end(), v.begin(), [](int x){ return x * 2; });
```

#### `fill`

- 作用：把区间填成同一值

```cpp
fill(v.begin(), v.end(), 0);
```

#### `replace` / `replace_if`

- 作用：替换满足条件的元素

### 3.6 删除整理类

#### `remove`

- 作用：把“不想要的值”移到后面，返回新的逻辑结尾
- 重点：**它不真正缩容**

```cpp
v.erase(remove(v.begin(), v.end(), x), v.end());
```

#### `remove_if`

- 作用：按条件移除，常与 `erase` 配套

#### `unique`

- 作用：去除相邻重复元素，返回新的逻辑结尾
- 若想真正“全局去重”，通常先 `sort` 再 `unique`

```cpp
sort(v.begin(), v.end());
v.erase(unique(v.begin(), v.end()), v.end());
```

### 3.7 反转与排列

#### `reverse`

- 作用：反转区间

#### `rotate`

- 作用：循环旋转区间

#### `next_permutation`

- 作用：生成下一个字典序排列

```cpp
sort(v.begin(), v.end());
do {
    // 使用当前排列
} while (next_permutation(v.begin(), v.end()));
```

#### `prev_permutation`

- 作用：生成上一个字典序排列

### 3.8 数值类算法

#### `accumulate`

- 头文件：`<numeric>`
- 作用：累加 / 可扩展为自定义折叠

```cpp
long long sum = accumulate(v.begin(), v.end(), 0LL);
```

- 高频提醒：
    - 初值类型决定结果类型

#### `iota`

- 作用：顺序填充递增值

```cpp
iota(v.begin(), v.end(), 0);
```

#### `partial_sum`

- 作用：生成前缀和序列

#### `adjacent_difference`

- 作用：生成相邻差分序列

### 3.9 集合与有序区间算法

- 前提通常是：**参与运算的区间有序**

#### `merge`

- 合并两个有序区间

#### `set_union`

- 求并集

#### `set_intersection`

- 求交集

#### `set_difference`

- 求差集

### 3.10 堆相关算法

#### `make_heap`

- 把区间建成堆

#### `push_heap`

- 堆中插入新元素后维护堆性质

#### `pop_heap`

- 把堆顶移到末尾并维护剩余区间为堆

#### `sort_heap`

- 对堆排序

---

## 4. 关联容器最值得掌握的成员函数

### 4.1 `set` / `map`

- `find(key)`：查找 key
- `count(key)`：判断是否存在（或计数）
- `lower_bound(key)` / `upper_bound(key)`：有序边界查找
- `insert(...)` / `emplace(...)`：插入
- `erase(key)` / `erase(it)`：删除

典型写法：

```cpp
map<int,int> mp;
mp[3] = 10;
auto it = mp.find(3);
if (it != mp.end()) cout << it->second << '\n';
```

### 4.2 `unordered_map` / `unordered_set`

- 高频成员：
    - `find`
    - `count`
    - `insert`
    - `emplace`
    - `erase`
- 适合高频查找、无需维护有序性的场景

高频提醒：

- `unordered_map` 平均快，但最坏情况不一定优雅
- 需要有序遍历、范围查询、边界查找时还是 `map` 更自然

---

## 5. 高频组合套路 ⭐

### 5.1 排序 + 去重

```cpp
sort(v.begin(), v.end());
v.erase(unique(v.begin(), v.end()), v.end());
```

### 5.2 排序 + 二分查找

```cpp
sort(v.begin(), v.end());
auto it = lower_bound(v.begin(), v.end(), x);
```

### 5.3 统计某值出现次数

```cpp
int cnt = upper_bound(v.begin(), v.end(), x) - lower_bound(v.begin(), v.end(), x);
```

### 5.4 remove-erase 惯用法

```cpp
v.erase(remove(v.begin(), v.end(), x), v.end());
```

### 5.5 map 查询存在性

```cpp
if (mp.find(key) != mp.end()) {
    // 存在
}
```

### 5.6 accumulate 快速聚合

```cpp
long long sum = accumulate(v.begin(), v.end(), 0LL);
```

### 5.7 next_permutation 枚举排列

```cpp
sort(v.begin(), v.end());
do {
    // 当前排列
} while (next_permutation(v.begin(), v.end()));
```

### 5.8 iota + sort / shuffle 生成序列

```cpp
vector<int> p(n);
iota(p.begin(), p.end(), 0);
```




# 优先级建议

### 第一层：必须滚瓜烂熟

- `sort`
- `lower_bound` / `upper_bound`
- `find`
- `count`
- `accumulate`
- `reverse`
- `unique + erase`
- `map.find`
- `push_back` / `emplace_back`

### 第二层：刷题高频增强

- `binary_search`
- `min_element` / `max_element`
- `transform`
- `next_permutation`
- `iota`
- `remove_if`
- `nth_element`

### 第三层：理解拓展

- `partial_sort`
- `stable_sort`
- `partial_sum`
- `merge`
- `set_union` / `set_intersection`
- `make_heap` / `push_heap` / `pop_heap`
