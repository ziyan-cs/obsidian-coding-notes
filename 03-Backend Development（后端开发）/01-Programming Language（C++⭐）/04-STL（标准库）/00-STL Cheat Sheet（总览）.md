#cpp #stl #cheat-sheet #containers #algorithms

## ⚡ TL;DR（快速决策）

- 这是一篇 **函数汇总型笔记**
- 使用方式：
    - 需要快速确认容器的增删查改接口时查阅
    - 需要回顾 STL 常见写法时查阅
    - 需要确认函数名、返回值或适用边界时查阅
- 如果只记 20%，优先记住这些：
    - `sort`
    - `lower_bound`
    - `upper_bound`
    - `find`
    - `accumulate`
    - `push_back`
    - `emplace_back`
    - `erase`
- 一句话理解：
    - **这篇笔记集中整理常用 STL 函数及其使用边界。**

## 🧩 Core Idea（核心本质）

- STL 使用中的常见问题通常不在于完全陌生，而在于：
    - 已有思路，但无法快速定位对应函数
    - 知道函数名，但不清楚返回值或适用容器
    - 能写出代码，但容易忽略边界条件与迭代器规则
- 因此这篇 Cheat Sheet 重点整理四类信息：
    - 函数作用
    - 典型写法
    - 常见适用容器
    - 高频坑点
- 一句话理解：
    - **应将 STL 理解为可组合的工具体系，而不是彼此孤立的函数清单。**

## 🗂️ 使用方式（阅读路径）

1. 容器基础状态接口：阅读“容器通用操作”
2. 插入类接口：阅读“插入操作”
3. 删除类接口：阅读“删除操作”
4. 查找类接口：阅读“查找操作”
5. 优先掌握最常用函数：阅读“最重要的 20% 函数”
6. 直接迁移到题目实现：优先阅读“高频组合套路”

---

## 1. 容器通用操作

### 1.1 `size()`

- 作用：返回容器当前元素个数
- 常见容器：`vector`、`string`、`deque`、`list`、`set`、`map`、`unordered_map`
- 典型写法：

```cpp
cout << v.size() << '\n';
```

- 高频提醒：
    - 返回值通常是无符号类型，和 `int` 比较时注意类型问题

### 1.2 `empty()`

- 作用：判断容器是否为空
- 典型写法：

```cpp
if (v.empty()) {
    cout << "empty\n";
}
```

- 高频提醒：
    - 判断空容器时，优先用 `empty()` 而不是 `size() == 0`

### 1.3 `begin()` / `end()`

- 作用：返回起始迭代器和尾后迭代器
- 典型写法：

```cpp
for (auto it = v.begin(); it != v.end(); ++it) {
    cout << *it << ' ';
}
```

- 高频提醒：
    - `end()` 指向的是“最后一个元素后面”，不能直接解引用

### 1.4 `clear()`

- 作用：清空容器中的全部元素
- 典型写法：

```cpp
v.clear();
```

- 高频提醒：
    - 清空后 `size()` 变 0，但底层容量不一定立刻缩掉（如 `vector`）

### 1.5 `swap()`

- 作用：交换两个容器内容
- 典型写法：

```cpp
vector<int> a = {1, 2}, b = {3, 4};
a.swap(b);
```

- 高频提醒：
    - 比手动复制交换更高效，也常用于“快速清空技巧”

---

## 2. 插入操作

### 2.1 `push_back()`

- 作用：在尾部插入一个元素
- 典型容器：`vector`、`string`、`deque`, `list`
- 典型写法：

```cpp
v.push_back(5);
```

- 高频提醒：
    - `vector` 中最常用的尾插接口之一就是它

### 2.2 `insert()`

- 作用：在指定位置插入元素
- 典型写法：

```cpp
v.insert(v.begin() + 1, 99);
set<int> s;
s.insert(10);
map<int, int> mp;
mp.insert({1, 100});
```

- 高频提醒：
    - 顺序容器的 `insert` 常要传迭代器位置
    - 关联容器的 `insert` 更像“按键值插入”

### 2.3 `emplace()` / `emplace_back()` ⭐

- 作用：原地构造元素，减少中间临时对象
- 典型写法：

```cpp
vector<pair<int, int>> v;
v.emplace_back(1, 2);

map<int, string> mp;
mp.emplace(1, "hello");
```

- 高频提醒：
    - 对 `pair` 和自定义对象尤其高效
    - 在题目实现中，`emplace_back` 往往比 `push_back(make_pair(...))` 更直接

---

## 3. 删除操作

### 3.1 `erase()`

- 作用：删除指定位置或指定范围元素
- 典型写法：

```cpp
v.erase(v.begin() + 2);
v.erase(v.begin(), v.begin() + 3);
mp.erase(1);
s.erase(10);
```

- 高频提醒：
    - 顺序容器删除后，后面的迭代器可能失效
    - `map` / `set` 的 `erase(key)` 是按键删

### 3.2 `pop_back()`

- 作用：删除尾部元素
- 典型写法：

```cpp
v.pop_back();
```

- 高频提醒：
    - `pop_back()` 不返回被删元素
    - 删之前若要用值，先 `back()` 再删

### 3.3 `pop_front()`

- 作用：删除头部元素
- 常见容器：`deque`、`list`
- 典型写法：

```cpp
dq.pop_front();
```

- 高频提醒：
    - `vector` 没有 `pop_front()`
    - 头删高频需求时，`deque` 往往比 `vector` 更合适

---

## 4. 查找操作

### 4.1 `find()`（算法库）

- 作用：在线性区间中查找某个值
- 头文件：`<algorithm>`
- 典型写法：

```cpp
auto it = find(v.begin(), v.end(), 5);
if (it != v.end()) cout << "found\\n";
```

- 高频提醒：
    - 这是线性查找，复杂度通常是 $O(n)$
    - 对有序区间想找边界，常优先考虑二分相关函数

### 4.2 `lower_bound()` ⭐

- 作用：返回“第一个 >= target”的位置
- 前提：区间必须有序
- 典型写法：

```cpp
sort(v.begin(), v.end());
auto it = lower_bound(v.begin(), v.end(), 5);
```

- 高频提醒：
    - 没排序就用，结果不可靠
    - 它是二分查找家族里最常用的一个

### 4.3 `upper_bound()`

- 作用：返回“第一个 > target”的位置
- 典型写法：

```cpp
auto it = upper_bound(v.begin(), v.end(), 5);
```

- 高频提醒：
    - 结合 `lower_bound()`，可以快速求某个值的出现区间

### 4.4 `map.find()`

- 作用：在 `map` / `unordered_map` 中按 key 查找
- 典型写法：

```cpp
auto it = mp.find(3);
if (it != mp.end()) {
    cout << it->second << '\\n';
}
```

- 高频提醒：
    - `mp[key]` 会在 key 不存在时创建新元素
    - 只想查询是否存在时，优先 `find()`

---

## 5. 排序

### 5.1 `sort()` ⭐

- 作用：对区间排序
- 常见容器：`vector`、`string`、数组区间
- 头文件：`<algorithm>`
- 典型写法：

```cpp
sort(v.begin(), v.end());
sort(v.begin(), v.end(), greater<int>());
```

- 高频提醒：
    - `sort` 底层要求随机访问迭代器，所以不能直接排 `list`
    - 自定义比较器时要确保逻辑稳定且满足严格弱序

### 5.2 `list.sort()`

- 作用：链表自己的排序函数
- 典型写法：

```cpp
list<int> lst = {3, 1, 2};
lst.sort();
```

- 高频提醒：
    - `list` 要用成员函数 `sort()`，不能直接用算法库 `sort(lst.begin(), lst.end())`

---

## 6. 最重要的 20% 函数 ⭐

### 6.1 `sort`

- 用途：排序、贪心前处理、去重前准备、二分前准备

### 6.2 `lower_bound`

- 用途：第一个 `>= x`，找边界、离散化、二分定位

### 6.3 `upper_bound`

- 用途：第一个 `> x`，统计区间出现次数常用

### 6.4 `find`

- 用途：线性查找，适合小范围或无需有序前提时

### 6.5 `accumulate`

- 头文件：`<numeric>`
- 用途：快速求和
- 典型写法：

```cpp
int sum = accumulate(v.begin(), v.end(), 0);
```

- 高频提醒：
    - 初值类型会影响结果类型，求大和时常改成 `0LL`

### 6.6 `push_back`

- 用途：尾插基础操作，序列构造中的高频接口

### 6.7 `emplace_back`

- 用途：原地构造，插入 `pair` 和对象时尤其常用

### 6.8 `erase`

- 用途：删元素、配合 `unique` 去重、删区间

<aside> ⭐

如果希望优先掌握最常用的 STL 组合，建议先熟悉下面几组模式：

**`sort + lower_bound`**

**`sort + unique + erase`**

**`find + end` 进行存在性判断**

**`accumulate` 进行区间求和**

</aside>

---

## 7. 高频组合套路（实践价值最高的部分）

### 7.1 排序 + 去重

```cpp
sort(v.begin(), v.end());
v.erase(unique(v.begin(), v.end()), v.end());
```

- 场景：去重、离散化预处理

### 7.2 二分找位置

```cpp
auto it = lower_bound(v.begin(), v.end(), x);
```

- 场景：有序数组找边界、找插入点

### 7.3 查某 key 是否存在

```cpp
if (mp.find(key) != mp.end()) {
    // 存在
}
```

- 场景：不想误创建 map 元素时

### 7.4 快速求和

```cpp
long long sum = accumulate(v.begin(), v.end(), 0LL);
```

- 场景：数组和、前缀处理前的快速统计

### 7.5 原地构造 `pair`

```cpp
vector<pair<int,int>> edges;
edges.emplace_back(u, v);
```

- 场景：图论边集、坐标、二元组存储

---

## ⚠️ Pitfalls（高频错误）

- 对无序序列使用 `lower_bound` / `upper_bound`
- 误以为 `erase` 删除后迭代器还一定有效
- 误把 `mp[key]` 当纯查询接口，结果意外插入元素
- 忘记 `pop_back()` / `pop_front()` 不返回值
- `accumulate` 初值写成 `0`，结果在大数时仍按 `int` 计算
- 把 `sort` 直接用于 `list`

## 🚀 Performance / Tips（性能优化）

- `find`：通常是 $O(n)$
- `lower_bound` / `upper_bound`：有序区间上通常是 $O(\log n)$
- `sort`：通常是 $O(n \log n)$
- `map.find`：通常是 $O(\log n)$
- `unordered_map.find`：平均通常是 $O(1)$
- 使用建议：
    - 先判断容器类型
    - 再判断区间是否有序
    - 最后选择线性查找、二分还是哈希

## 📌 One-liner Summary（一句话总结）

- **这篇 STL Cheat Sheet 将最常用的 STL 函数按“增删查改 + 排序 + 组合模式”进行集中整理。**