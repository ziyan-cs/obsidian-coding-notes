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