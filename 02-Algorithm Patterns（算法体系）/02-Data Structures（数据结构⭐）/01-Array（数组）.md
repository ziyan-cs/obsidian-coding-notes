#data-structure #array #index #sequence #cpp

## ⚡ TL;DR（快速决策）

- 数组本质是：**一段连续内存中的同类型元素集合**
- 相关特征：
    - 按下标快速访问
    - 顺序遍历、统计、修改
    - 数据规模已知或上界明确
- 核心优势：
    - 连续存储
    - 随机访问快
    - 常数小、实现简单
- 核心代价：
    - 长度通常固定
    - 中间插入 / 删除不灵活
    - 越界风险高

## 🧩 Core Idea（核心本质）

- 数组中每个元素类型相同，位置连续
- 因为连续存储，所以可以通过首地址 + 偏移量快速定位任意元素
- 下标通常从 `0` 开始
- 一句话理解：
    - **数组就是“连续排好的一串盒子”，每个盒子有编号。**
- 数组常被当成很多结构的底层：
    - 堆
    - 哈希桶数组
    - 邻接表的存储容器
    - 前缀和 / 差分等基础技巧

## 🔧 Usage Patterns（可复用代码模板）

1. 一维数组定义与访问

```cpp
#include <iostream>
using namespace std;

int main() {
    int a[5] = {10, 20, 30, 40, 50};
    cout << a[0] << '\\n';
    cout << a[4] << '\\n';
    return 0;
}
```

1. 顺序遍历

```cpp
for (int i = 0; i < 5; ++i) {
    cout << a[i] << ' ';
}
```

1. 统计和 / 最值

```cpp
int sum = 0, mx = a[0];
for (int i = 0; i < 5; ++i) {
    sum += a[i];
    mx = max(mx, a[i]);
}
```

1. 二维数组

```cpp
int g[2][3] = {
    {1, 2, 3},
    {4, 5, 6}
};
cout << g[1][2] << '\\n';
```

1. 计数数组

```cpp
int cnt[10] = {0};
for (int x : nums) ++cnt[x];
```

## ⚠️ Pitfalls（高频错误）

- 下标越界
- 0-based 和 1-based 混用
- 局部数组未初始化就使用
- 数组长度和最后下标搞混
- 二维数组行列写反

## 🚀 Performance / Tips（性能优化）

- 随机访问：$O(1)$
- 顺序遍历：$O(n)$
- 连续内存通常缓存友好，常数表现很好
- 实战建议：
    - 长度固定、逻辑简单时直接用数组
    - 更灵活时常改用 `vector`

## 🧪 Common Scenarios（常见使用场景）

- 存一串数字
- 顺序扫描 / 枚举
- 前缀和、差分的底层存储
- 频率统计
- 二维网格 / 棋盘

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;
int main() {
    int a[4] = {2, 4, 6, 8};
    int sum = 0;
    for (int i = 0; i < 4; ++i) sum += a[i];
    cout << sum << '\\n';
    return 0;
}
```