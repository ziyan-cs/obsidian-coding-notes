#cpp #stl #numeric #partial_sum #prefix-sum

## ⚡ TL;DR（快速决策）

- `partial_sum` 的核心是：**生成区间的前缀累积结果**
- 它是前缀和思想的 STL 标准实现

## 🧩 Core Idea（核心本质）

- 第 i 个输出值，表示前 i 个元素的累积结果
- 一句话理解：
    - **`partial_sum` 是“把累积过程展开成整个序列”。**

## 🔧 典型写法

```cpp
vector<int> pre(v.size());
partial_sum(v.begin(), v.end(), pre.begin());
```

## 🧪 常见场景

- 前缀和预处理
- 前缀乘积 / 自定义累积结构
- 区间统计前置准备

## ⚠️ Pitfalls（高频错误）

- 输出区间空间没准备好
- 只知道前缀和概念，不知道 STL 已有现成实现

## 📌 One-liner Summary（一句话总结）

- **`partial_sum` 的核心，是把区间的逐步累积结果完整生成出来。**