#cpp #stl #numeric #inner_product #dot-product

## ⚡ TL;DR（快速决策）

- `inner_product` 的核心是：**对两个区间做配对运算并累积结果**
- 默认含义是数学上的内积：对应项相乘后求和
- 它也支持自定义“配对规则 + 累积规则”

## 🧩 Core Idea（核心本质）

- `inner_product` 其实是“两个区间上的二重折叠”
- 一句话理解：
    - **`inner_product` 是对两个序列逐项组合后再聚合的算法。**

## 🔧 典型写法

```cpp
int ans = inner_product(a.begin(), a.end(), b.begin(), 0);
```

## 🧪 常见场景

- 向量点积
- 权重和计算
- 两序列相似度类计算

## ⚠️ Pitfalls（高频错误）

- 两个区间长度不匹配
- 初值类型不合适
- 不理解默认是“乘后加”，但其实可自定义

## 📌 One-liner Summary（一句话总结）

- **`inner_product` 的核心，是把两个区间逐项配对运算后累积成一个最终结果。**