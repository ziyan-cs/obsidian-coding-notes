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