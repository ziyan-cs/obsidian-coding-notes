#data-structure #array #index #sequence #cpp

## ⚡ TL;DR（快速决策）

- 数组本质是：**连续存储的一组同类型元素**
- 一看到这些需求，要优先想到数组：
    - 需要按下标随机访问
    - 需要顺序遍历、统计、修改
    - 数据规模固定或可提前开空间
- 核心优势：访问快、结构简单
- 核心风险：越界

## 🧩 Core Idea（核心本质）

- 数组元素在内存中连续排列
- 下标通常从 `0` 开始
- 一句话理解：
    - **数组就是用编号管理的一串连续元素。**

## 🔧 Usage Patterns（可复用代码模板）

1. 基础定义

```cpp
int a[5] = {1, 2, 3, 4, 5};
cout << a[0] << ' ' << a[4] << '\\n';
```

1. 遍历

```cpp
for (int i = 0; i < 5; ++i) {
    cout << a[i] << ' ';
}
```

1. 二维数组

```cpp
int g[2][3] = {{1, 2, 3}, {4, 5, 6}};
cout << g[1][2] << '\\n';
```

## ⚠️ Pitfalls（高频错误）

- 下标越界
- 下标从 0 / 1 混乱
- 局部数组未初始化就使用

## 🚀 Performance / Tips（性能优化）

- 随机访问：$O(1)$
- 顺序扫描：$O(n)$
- 数据更灵活时通常改用 `vector`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;
int main() {
    int a[3] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) cout << a[i] << ' ';
}
```

## 📌 One-liner Summary（一句话总结）

- **数组就是：一段连续存储、支持按下标快速访问的数据结构。**