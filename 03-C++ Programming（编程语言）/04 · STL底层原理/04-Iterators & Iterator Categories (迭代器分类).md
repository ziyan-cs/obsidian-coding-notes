# Iterators & Iterator Categories
#cpp #stl #iterator #categories #generic-programming

## ⚡ TL;DR（快速决策）

- 迭代器类型的核心是：**不同迭代器能力不同，因此算法可用范围也不同**
- 经典类别：
    - 输入迭代器
    - 输出迭代器
    - 前向迭代器
    - 双向迭代器
    - 随机访问迭代器

## 🧩 Core Idea（核心本质）

- STL 不是直接写死“只支持某容器”，而是按迭代器能力设计算法
- 一句话理解：
    - **迭代器类型决定算法能做什么。**

## 🔧 关键区别

- 前向迭代器：可单向前进
- 双向迭代器：可前后移动
- 随机访问迭代器：支持 `+`、`-`、下标式跳转

## 🧪 容器直觉

- `vector`：随机访问迭代器
- `list`：双向迭代器
- `forward_list`：前向迭代器

## ⚠️ Pitfalls（高频错误）

- 误以为所有容器都能直接配合 `sort`
- 不理解 `distance` / `advance` 在不同迭代器上复杂度差异

## 📌 One-liner Summary（一句话总结）

- **迭代器类型的核心，是用“能力分级”决定 STL 算法的可用性与复杂度。**
#cpp #stl #iterator #advance #next #prev

## ⚡ TL;DR（快速决策）

- `advance` / `next` / `prev` 的核心是：**在不同迭代器能力下进行通用位置移动**
- 它们让代码不必依赖某种具体容器的下标访问能力

## 🧩 Core Idea（核心本质）

- 并不是所有迭代器都支持 `it + k`
- 一句话理解：
    - **这组函数是 STL 中通用的迭代器移动工具。**

## 🔧 高频函数

### `advance(it, n)`

- 直接修改原迭代器位置

### `next(it, n)`

- 返回向后移动后的新迭代器，不改原对象

### `prev(it, n)`

- 返回向前移动后的新迭代器

## 🔧 典型写法

```cpp
auto it2 = next(it, 2);
auto it1 = prev(it2);
advance(it, 3);
```

## ⚠️ Pitfalls（高频错误）

- 忘记 `advance` 会修改原迭代器
- 对不支持后退的迭代器使用 `prev`
- 忽略在线性迭代器上的移动成本

## 📌 One-liner Summary（一句话总结）

- **`advance`、`next`、`prev` 的核心，是用统一接口处理不同类型迭代器的位置移动。**
#cpp #stl #iterator #back_inserter #inserter

## ⚡ TL;DR（快速决策）

- 插入迭代器的核心是：**把算法输出自动转换成容器的插入操作**
- 它解决的是“目标容器没有预先分配好空间，算法怎么写结果”这一问题
- 高频工具：
    - `back_inserter`
    - `front_inserter`
    - `inserter`

## 🧩 Core Idea（核心本质）

- 很多算法需要“输出迭代器”作为结果位置
- 插入迭代器可以把“赋值”动作转成 `push_back`、`push_front` 或 `insert`
- 一句话理解：
    - **插入迭代器是算法输出与容器插入接口之间的适配器。**

## 🔧 典型写法

```cpp
vector<int> ans;
copy(v.begin(), v.end(), back_inserter(ans));
```

## 🧪 常见搭配

- `copy + back_inserter`
- `transform + back_inserter`
- 集合算法 + `inserter`

## ⚠️ Pitfalls（高频错误）

- 输出到空容器时直接写 `ans.begin()`
- 对不支持对应插入方式的容器使用错误插入迭代器

## 📌 One-liner Summary（一句话总结）

- **插入迭代器的核心，是把 STL 算法输出无缝接到容器的插入行为上。**