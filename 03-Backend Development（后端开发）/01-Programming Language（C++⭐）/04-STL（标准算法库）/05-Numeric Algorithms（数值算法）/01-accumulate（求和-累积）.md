#cpp #stl #numeric #accumulate #fold

## ⚡ TL;DR（快速决策）

- `accumulate` 的核心是：**按顺序把区间元素累积成一个结果**
- 它不只会求和，本质上是一个折叠（fold）操作
- 若只抓重点，应优先记住：
    - 初值类型会影响结果类型
    - 可以传入自定义二元操作

## 🧩 Core Idea（核心本质）

- `accumulate` 从左到右不断把“当前结果”和“新区间元素”合并
- 一句话理解：
    - **`accumulate` 是 STL 中最常用的区间聚合函数。**

## 🔧 典型写法

```cpp
long long sum = accumulate(v.begin(), v.end(), 0LL);
```

自定义操作：

```cpp
string s = accumulate(v.begin(), v.end(), string(),
    [](string a, int x) { return a + to_string(x); });
```

## ⚠️ Pitfalls（高频错误）

- 求大数和时初值写成 `0` 而不是 `0LL`
- 不理解第三个参数决定返回类型

## 📌 One-liner Summary（一句话总结）

- **`accumulate` 的核心，是通过初值和二元操作把整个区间聚合为一个结果。**