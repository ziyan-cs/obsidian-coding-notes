#cpp #stl #numeric #adjacent_difference #difference

## ⚡ TL;DR（快速决策）

- `adjacent_difference` 的核心是：**把区间转成“当前值减前一个值”的差分形式**
- 它是前缀和的常见逆向对应工具之一

## 🧩 Core Idea（核心本质）

- 第一个输出通常直接保留原值，后续输出是相邻元素差
- 一句话理解：
    - **`adjacent_difference` 是把原序列改写为差分序列。**

## 🔧 典型写法

```cpp
vector<int> diff(v.size());
adjacent_difference(v.begin(), v.end(), diff.begin());
```

## 🧪 常见场景

- 差分数组理解
- 从前缀累积结果恢复变化量
- 数列变化趋势分析

## ⚠️ Pitfalls（高频错误）

- 忽略第一个元素的特殊含义
- 把它和“任意两项相减”混淆

## 📌 One-liner Summary（一句话总结）

- **`adjacent_difference` 的核心，是把原区间转换成相邻元素之间的变化量序列。**