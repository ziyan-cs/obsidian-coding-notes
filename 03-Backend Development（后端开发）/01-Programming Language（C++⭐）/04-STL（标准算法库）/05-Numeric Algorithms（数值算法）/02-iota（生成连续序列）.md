#cpp #stl #numeric #iota #sequence

## ⚡ TL;DR（快速决策）

- `iota` 的核心是：**用递增值顺序填充区间**
- 它特别适合生成编号数组、下标数组和初始排列

## 🧩 Core Idea（核心本质）

- `iota` 解决的是“连续序列初始化”问题
- 一句话理解：
    - **`iota` 是生成有规律初始序列的标准工具。**

## 🔧 典型写法

```cpp
vector<int> p(n);
iota(p.begin(), p.end(), 0);
```

## 🧪 常见场景

- 生成 `0..n-1`
- 配合 `sort` 做下标排序
- 配合 `next_permutation` 做排列初始化

## ⚠️ Pitfalls（高频错误）

- 忘记先给容器分配空间
- 把 `iota` 当成随机序列生成器

## 📌 One-liner Summary（一句话总结）

- **`iota` 的核心，是把区间快速初始化为连续递增序列。**