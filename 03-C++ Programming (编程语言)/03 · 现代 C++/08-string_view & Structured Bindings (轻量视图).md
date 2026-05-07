# Modern STL Additions (C++17+ 补充)

---
01-ranges.md
#cpp #stl #ranges #cxx20 #modern-stl

## ⚡ TL;DR（快速决策）

- `ranges` 的核心是：**把“算法 + 区间 + 视图变换”统一进更现代、更可组合的接口体系**
- 它是 C++20 之后 STL 使用方式的一次明显升级
- 若只抓重点，应优先记住：
    - `ranges::sort`
    - `ranges::find`
    - `views::filter`
    - `views::transform`
    - `views::take`
- 一句话理解：
    - **ranges 让 STL 从“传 begin/end”走向“直接操作区间与视图流水线”。**

## 🧩 Core Idea（核心本质）

- 传统 STL 算法常写成：

```cpp
sort(v.begin(), v.end());
```

- ranges 风格常写成：

```cpp
ranges::sort(v);
```

- 它不只是“少写一点代码”，更重要的是：
    - 表达更直接
    - 可组合性更强
    - 与懒计算视图配合更自然
- 一句话理解：
    - **ranges = 面向区间对象与视图组合的现代泛型编程接口。**

## 🏗️ ranges 的三层结构

### 1. ranges algorithms

- 如：
    - `ranges::sort`
    - `ranges::find`
    - `ranges::count`
    - `ranges::binary_search`
- 作用：直接对 range 进行算法操作

### 2. views

- 如：
    - `views::filter`
    - `views::transform`
    - `views::take`
    - `views::drop`
    - `views::reverse`
- 作用：生成“视图”，通常不立即拷贝底层数据

### 3. range concepts

- 如：
    - `range`
    - `input_range`
    - `random_access_range`
- 作用：约束算法能接受什么样的区间类型

## 🔧 高频写法

### 1. 直接对容器排序

```cpp
ranges::sort(v);
```

### 2. 查找元素

```cpp
auto it = ranges::find(v, x);
```

### 3. 过滤后再变换

```cpp
auto vw = v | views::filter([](int x){ return x % 2 == 0; })
            | views::transform([](int x){ return x * x; });
```

### 4. 截取前 k 个

```cpp
auto firstK = v | views::take(k);
```

## 🧠 为什么 ranges 很重要

- 更贴近“数据流处理”思维
- 与函数式风格更接近
- 写法更统一
- 减少手写 `begin()` / `end()` 的噪音
- 更适合现代 C++ 风格的组合表达

## ⚠️ Pitfalls（高频错误）

- 以为 view 一定会立刻生成新容器，实际上很多 view 是惰性的
- 把 `ranges` 和“所有场景都更快”画等号，它首先优化的是表达方式
- 不理解 view 生命周期，导致悬垂引用风险
- 以为学了 `ranges` 就不用理解传统 STL，实际上底层思想仍是连贯的

## 🚀 Performance / Tips（理解深化）

- `ranges` 的重点首先是可读性与组合性，不是无脑追求性能神话
- 很多 `views` 是惰性求值，避免不必要中间容器构造
- 真正高质量的现代 C++ STL 使用，往往是：
    - 传统算法思想扎实
    - 再用 ranges 提升表达层次

## 🧪 Common Scenarios（常见使用场景）

- 过滤 + 映射 + 截取的流水线式处理
- 直接对容器使用算法
- 写更现代的工具函数和泛型代码
- C++20 风格刷题与工程代码整理

## 🧾 Minimal Template（最小理解模板）

```cpp
auto vw = v | views::filter(pred) | views::transform(func);
ranges::sort(v);
```

## 📌 One-liner Summary（一句话总结）

- **ranges 的核心，是把算法、区间和视图组合统一进更现代、更直接、更可组合的 STL 使用方式。**
---
02-reduce.md
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
---
03-新增算法.md
#cpp #stl #modern-stl #algorithms #cxx17 #cxx20

## ⚡ TL;DR（快速决策）

- “新增算法”这一节的核心是：**理解 C++17 / C++20 之后 STL 补进了哪些更现代、更语义化的算法能力**
- 重点不在“把所有新函数背一遍”，而在于抓住最有学习价值的一批：
- 一句话理解：

## 🧩 Core Idea（核心本质）

- 现代 STL 的新增算法有几个明显方向：
- 一句话理解：

## 🗂️ 最值得掌握的一批新增算法

### 1. 归约与扫描类

#### `reduce`

- 更现代的归约接口

#### `transform_reduce`

- 先变换，再归约
- 可以理解为：`transform + reduce` 的组合

```cpp
auto ans = transform_reduce(a.begin(), a.end(), b.begin(), 0);
```

#### `inclusive_scan`

- 包含当前元素的前缀扫描

#### `exclusive_scan`

- 不包含当前元素的前缀扫描

### 2. 更语义化的小工具

#### `clamp`

- 把值夹在某个区间内

```cpp
x = clamp(x, 0, 100);
```

#### `sample`

- 从区间中抽样若干元素

#### `shift_left` / `shift_right`

- 区间整体左移 / 右移
- 更清楚地表达“区间位移”意图

### 3. ranges 体系下的新增风格

- `ranges::sort`
- `ranges::find`
- `views::filter`
- `views::transform`
- `views::take`
- 这些不只是新名字，而是新的 STL 使用范式

## 🔧 为什么这些新增算法值得学

- 它们把高频操作语义化了
- 它们减少手写循环和模板噪音
- 它们体现了现代 C++ STL 的设计方向
- 它们对理解并行、区间和泛型表达很重要

## ⚠️ Pitfalls（高频错误）

- 把“新增算法”理解成“只是 API 变多了”
- 不区分 `accumulate` / `reduce`、`partial_sum` / `scan` 的语义差别
- 只记名字，不理解它们替代了什么老写法

## 🚀 Performance / Tips（理解深化）

- 学 Modern STL 时，重点应是“语义升级”和“表达升级”
- 真正高质量的学习方式是：

## 🧪 Common Scenarios（常见使用场景）

- 前缀扫描类问题
- 区间归约与统计
- 更现代的容器处理写法
- C++20 风格代码组织

## 🧾 Minimal Template（最小理解模板）

```
现代新增算法 = 更强的归约/扫描 + 更清楚的语义工具 + ranges 风格升级
```

## 📌 One-liner Summary（一句话总结）

- **Modern STL 新增算法的核心，是把常见高频操作进一步标准化、语义化，并向区间与并行时代升级。**