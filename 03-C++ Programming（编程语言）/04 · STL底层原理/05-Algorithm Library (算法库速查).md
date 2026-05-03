# Algorithm Library (算法库速查)

---
#cpp #stl #algorithm #traversal #for_each

## ⚡ TL;DR（快速决策）

- 遍历类算法的核心是：**按顺序访问区间中的每个元素，并执行某种检查、输出或操作**
- 这类算法通常不强调“重排区间”，而强调“逐个处理”
- 高频函数：
    - `for_each`
    - `for_each_n`

## 🧩 Core Idea（核心本质）

- STL 的遍历类算法本质上是“把循环抽象成算法接口”
- 一句话理解：
    - **遍历类算法用于表达“对区间中每个元素做点什么”。**

## 🔧 高频函数

### `for_each`

- 作用：对区间每个元素执行函数对象

```cpp
for_each(v.begin(), v.end(), [](int x){ cout << x << ' '; });
```

- 适合：打印、统计副作用、批量处理

### `for_each_n`

- 作用：对前 n 个元素执行操作

```cpp
for_each_n(v.begin(), 3, [](int x){ cout << x << ' '; });
```

## ⚠️ Pitfalls（高频错误）

- 把 `for_each` 当作主要修改容器值的首选工具，很多时候 `transform` 更清晰
- 在 Lambda 中随意捕获和修改外部状态，导致逻辑不透明

## 🚀 Performance / Tips（理解深化）

- 这类算法最重要的价值是“表达意图”，不是单纯替代 `for`
- 如果操作结果要写入新区间，优先考虑 `transform`

## 📌 One-liner Summary（一句话总结）

- **遍历类算法的核心，是按区间顺序访问每个元素并执行统一处理逻辑。**
---
#cpp #stl #algorithm #search #find

## ⚡ TL;DR（快速决策）

- 查找类算法的核心是：**在区间中定位某个值、某类元素或某段模式**
- 若只抓重点，应优先掌握：
    - `find`
    - `find_if`
    - `count`
    - `binary_search`
    - `lower_bound`
    - `upper_bound`

## 🧩 Core Idea（核心本质）

- 查找类算法的差别，主要在于：
    - 是否要求有序
    - 返回迭代器还是计数
    - 查找的是单值、条件还是边界
- 一句话理解：
    - **查找类算法是在区间中定位“目标”或“边界”。**

## 🔧 高频函数

### 线性查找

- `find`：找某个值
- `find_if`：按条件找
- `count` / `count_if`：统计出现次数

### 有序区间查找

- `binary_search`：判断是否存在
- `lower_bound`：第一个 `>= x`
- `upper_bound`：第一个 `> x`
- `equal_range`：返回某值对应区间

## 🔧 高频模板

```cpp
auto it = find(v.begin(), v.end(), x);
bool ok = binary_search(v.begin(), v.end(), x);
auto L = lower_bound(v.begin(), v.end(), x);
auto R = upper_bound(v.begin(), v.end(), x);
```

## ⚠️ Pitfalls（高频错误）

- 在无序区间上使用二分家族函数
- 混淆 `find` 和 `binary_search` 的适用条件
- 不理解 `lower_bound` / `upper_bound` 返回的是位置而不是值

## 📌 One-liner Summary（一句话总结）

- **查找类算法的核心，是在线性区间或有序区间中定位目标元素及其边界。**
---
#cpp #stl #algorithm #sort #ordering

## ⚡ TL;DR（快速决策）

- 排序类算法的核心是：**按照某种比较规则重排区间元素**
- 若只抓重点，应优先掌握：
    - `sort`
    - `stable_sort`
    - `partial_sort`
    - `nth_element`
    - `is_sorted`

## 🧩 Core Idea（核心本质）

- 排序不只是“从小到大排好”，还包括：
    - 稳定排序
    - 部分排序
    - 第 k 小选择
- 一句话理解：
    - **排序类算法是在“顺序关系”上组织区间。**

## 🔧 高频函数

### `sort`

- 默认升序排序

```cpp
sort(v.begin(), v.end());
```

### `stable_sort`

- 保持相等元素相对顺序

### `nth_element`

- 让第 k 个位置正确，不保证整体有序

```cpp
nth_element(v.begin(), v.begin() + k, v.end());
```

### `partial_sort`

- 只把前一段排好

### `is_sorted`

- 判断区间是否有序

## ⚠️ Pitfalls（高频错误）

- 把 `nth_element` 当成完全排序
- 在不支持随机访问迭代器的容器上直接使用 `sort`
- 自定义比较器不满足严格弱序

## 📌 One-liner Summary（一句话总结）

- **排序类算法的核心，是按比较规则重排区间，并支持稳定、部分和选择型排序需求。**
---
#cpp #stl #algorithm #count #statistics

## ⚡ TL;DR（快速决策）

- 统计类算法的核心是：**从区间中提取数量、最值、性质判断等信息**
- 高频函数：
    - `count`
    - `count_if`
    - `all_of` / `any_of` / `none_of`
    - `min_element` / `max_element`
    - `accumulate`

## 🧩 Core Idea（核心本质）

- 统计类算法通常不改变区间，而是从区间中提炼“信息”
- 一句话理解：
    - **统计类算法用于回答“这个区间有什么特征”。**

## 🔧 高频函数

- `count` / `count_if`：统计个数
- `all_of` / `any_of` / `none_of`：整体条件判断
- `min_element` / `max_element` / `minmax_element`：求最值位置
- `accumulate`：求和或折叠聚合

## 🔧 高频模板

```cpp
int cnt = count(v.begin(), v.end(), x);
bool ok = any_of(v.begin(), v.end(), [](int x){ return x < 0; });
auto mx = max_element(v.begin(), v.end());
long long sum = accumulate(v.begin(), v.end(), 0LL);
```

## ⚠️ Pitfalls（高频错误）

- `accumulate` 初值类型写错
- 把最值迭代器直接当整数使用
- 忽略空区间下 `min_element` / `max_element` 的边界问题

## 📌 One-liner Summary（一句话总结）

- **统计类算法的核心，是从区间中提炼数量、最值和整体性质等关键信息。**
---
#cpp #stl #algorithm #modify #transform

## ⚡ TL;DR（快速决策）

- 修改类算法的核心是：**直接改变区间中的元素值、顺序或逻辑内容**
- 高频函数：
    - `fill`
    - `copy`
    - `transform`
    - `replace`
    - `remove`
    - `unique`
    - `reverse`
    - `rotate`

## 🧩 Core Idea（核心本质）

- 修改类算法关注“把原区间变成想要的样子”
- 一句话理解：
    - **修改类算法用于重写、替换、整理和重排区间内容。**

## 🔧 高频函数

- `fill`：统一赋值
- `copy`：复制到目标区间
- `transform`：按规则变换
- `replace` / `replace_if`：替换值
- `remove` / `remove_if`：逻辑移除
- `unique`：压缩相邻重复
- `reverse`：反转
- `rotate`：旋转

## 🔧 高频模板

```cpp
fill(v.begin(), v.end(), 0);
transform(v.begin(), v.end(), v.begin(), [](int x){ return x * 2; });
v.erase(remove(v.begin(), v.end(), x), v.end());
reverse(v.begin(), v.end());
```

## ⚠️ Pitfalls（高频错误）

- `remove` 后忘记配合 `erase`
- `unique` 前没搞清楚它只去除相邻重复
- `copy` 目标区间空间不足

## 📌 One-liner Summary（一句话总结）

- **修改类算法的核心，是通过变换、替换、删除整理和重排来直接改变区间内容。**
---
#cpp #stl #algorithm #permutation #next_permutation

## ⚡ TL;DR（快速决策）

- 排列类算法的核心是：**在给定区间上生成、判断或操作排列顺序**
- 高频函数：
    - `next_permutation`
    - `prev_permutation`
- 它们在枚举排列、全排列搜索、字典序操作中非常高频

## 🧩 Core Idea（核心本质）

- 排列类算法本质上是在“当前顺序”的基础上寻找相邻字典序状态
- 一句话理解：
    - **排列类算法用于在区间顺序空间中做系统性移动。**

## 🔧 高频函数

### `next_permutation`

- 生成下一个字典序排列
- 若不存在更大排列，则返回 `false` 并重置为最小字典序

### `prev_permutation`

- 生成上一个字典序排列

## 🔧 高频模板

```cpp
sort(v.begin(), v.end());
do {
    // 当前排列
} while (next_permutation(v.begin(), v.end()));
```

## ⚠️ Pitfalls（高频错误）

- 不先排序就直接枚举，导致无法完整遍历全部排列
- 把它们误解成“随机打乱”函数

## 📌 One-liner Summary（一句话总结）

- **排列类算法的核心，是按字典序在不同排列状态之间前后推进。**
---
#cpp #stl #algorithm #set-operations #ordered-range

## ⚡ TL;DR（快速决策）

- 集合操作类算法的核心是：**在两个有序区间上进行并、交、差、合并等运算**
- 这类算法几乎都依赖一个关键前提：**输入区间有序**
- 高频函数：
    - `merge`
    - `set_union`
    - `set_intersection`
    - `set_difference`
    - `includes`

## 🧩 Core Idea（核心本质）

- 这些算法不是对 `set` 容器专属，而是对“有序区间”进行集合语义运算
- 一句话理解：
    - **集合操作算法是在有序序列上模拟集合运算。**

## 🔧 高频函数

- `merge`：合并两个有序区间
- `set_union`：并集
- `set_intersection`：交集
- `set_difference`：差集
- `set_symmetric_difference`：对称差
- `includes`：判断一个有序区间是否包含另一个

## 🔧 高频模板

```cpp
vector<int> c;
set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(c));
```

## ⚠️ Pitfalls（高频错误）

- 忘记先排序
- 误以为只能作用于 `set` 容器
- 输出区间未准备好或未使用插入迭代器

## 📌 One-liner Summary（一句话总结）

- **集合操作类算法的核心，是在两个有序区间上执行并、交、差等集合语义运算。**
---
#cpp #stl #algorithm #utility #helpers

## ⚡ TL;DR（快速决策）

- 工具类函数的核心是：**为 STL 算法与泛型编程提供通用辅助能力**
- 它们不一定直接属于 `<algorithm>`，但在 STL 使用中极高频
- 高频工具：
    - `swap`
    - `pair`
    - `make_pair`
    - `distance`
    - `advance`
    - `next` / `prev`
    - `back_inserter`

## 🧩 Core Idea（核心本质）

- 工具类函数的价值在于把“算法之间的衔接动作”标准化
- 一句话理解：
    - **工具类函数是 STL 体系里连接容器、算法和迭代器的胶水。**

## 🔧 高频工具

- `swap`：交换两个对象
- `pair` / `make_pair`：打包二元数据
- `distance`：计算迭代器距离
- `advance`：推进迭代器
- `next` / `prev`：返回前后移动后的迭代器
- `back_inserter` / `inserter`：把算法输出接到容器插入操作上

## 🔧 高频模板

```cpp
auto it2 = next(it);
copy(a.begin(), a.end(), back_inserter(ans));
```

## ⚠️ Pitfalls（高频错误）

- 不理解不同迭代器类型下 `advance` / `distance` 的复杂度差异
- 算法输出到空容器时忘记使用插入迭代器

## 📌 One-liner Summary（一句话总结）

- **工具类函数的核心，是为 STL 中容器、迭代器与算法的协同使用提供基础支撑。**
---
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
---
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
---
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
---
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
---
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