#cpp #stl #reduce #numeric #parallel #modern-stl

## ⚡ TL;DR（快速决策）

- `reduce` 的核心是：**把区间元素聚合成一个结果，但更强调可并行归约语义**
- 它和 `accumulate` 很像，但设计目标不完全相同
- 若只抓重点，应优先记住：
    - `accumulate` 偏顺序折叠
    - `reduce` 偏归约模型
    - 运算最好满足交换律 / 结合律
- 一句话理解：
    - **reduce 是更现代、为并行与泛化归约准备的累积算法。**

## 🧩 Core Idea（核心本质）

- `accumulate` 更像：
    - 从左到右一个一个加上去
- `reduce` 更像：
    - 允许分块、允许不同组合顺序地做归约
- 这意味着：
    - 对加法这类操作通常很好理解
    - 对字符串拼接、浮点误差敏感操作要更谨慎
- 一句话理解：
    - **reduce 关注“最终归约结果”，不强调固定的线性折叠顺序。**

## 🔧 典型写法

### 1. 默认求和

```cpp
int sum = reduce(v.begin(), v.end());
```

### 2. 指定初值

```cpp
long long sum = reduce(v.begin(), v.end(), 0LL);
```

### 3. 指定二元操作

```cpp
int ans = reduce(v.begin(), v.end(), 1, multiplies<int>());
```

## 🔍 `reduce` vs `accumulate`

### `accumulate`

- 更强调顺序
- 典型理解：左折叠
- 更适合顺序敏感运算

### `reduce`

- 更强调归约
- 更适合可并行、可重排的运算
- 对运算性质要求更高

## 🧠 为什么要学 `reduce`

- 它体现了现代 STL 对并行与泛化计算的支持方向
- 它帮助你真正理解：
    - “顺序累积”和“归约”不是同一个概念
    - 为什么算法语义会影响正确性与可并行性

## ⚠️ Pitfalls（高频错误）

- 把 `reduce` 完全当成 `accumulate` 替代品
- 在顺序敏感或非结合运算上乱用 `reduce`
- 忽略浮点加法在不同归约顺序下结果可能略有差异

## 🚀 Performance / Tips（理解深化）

- `reduce` 的学习重点不是 API 本身，而是“归约语义”
- 真正理解 `reduce`，会帮助你更好理解并行算法、执行策略、数值稳定性
- 刷题中多数基础场景仍常用 `accumulate`，但现代 STL 语义上 `reduce` 更值得理解

## 🧪 Common Scenarios（常见使用场景）

- 区间求和
- 区间求积
- 可交换可结合的归约操作
- 理解并行 STL 的基础概念

## 🧾 Minimal Template（最小理解模板）

```cpp
auto ans = reduce(v.begin(), v.end(), init, op);
```

## 📌 One-liner Summary（一句话总结）

- **reduce 的核心，是在更灵活、可归约的语义下把整个区间聚合成一个结果。**