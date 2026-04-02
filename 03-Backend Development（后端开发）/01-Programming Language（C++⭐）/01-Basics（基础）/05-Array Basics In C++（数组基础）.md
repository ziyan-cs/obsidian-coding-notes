#cpp #array #basics #index #sequence

## ⚡ TL;DR（快速决策）

- 数组本质是：**一组连续存储、可用下标访问的同类型数据**
- 一看到这些需求，要优先想到数组：
    - 数据个数固定或已知
    - 需要频繁按下标访问
    - 要顺序遍历、统计、修改
- 数组的核心优势：
    - 访问快
    - 结构简单
- 核心风险：
    - 越界

## 🧩 Core Idea（核心本质）

- 数组中元素连续存放在内存里
- 下标通常从 `0` 开始
- 一句话理解：
    - **数组就是用编号管理的一串同类型元素。**
- 常见写法：
    - `int a[100];`
    - 多维数组：`int a[10][10];`

## 🔧 Usage Patterns（可复用代码模板）

1. 基础定义与访问

```cpp
#include <iostream>
using namespace std;

int main() {
    int a[5] = {1, 2, 3, 4, 5};
    cout << a[0] << '\n';
    cout << a[4] << '\n';
}
```

2. 遍历数组

```cpp
for (int i = 0; i < 5; ++i) {
    cout << a[i] << ' ';
}
```

3. 统计和

```cpp
int sum = 0;
for (int i = 0; i < n; ++i) sum += a[i];
```

4. 二维数组

```cpp
int g[3][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};
```

## ⚠️ Pitfalls（高频错误）

- 下标越界是数组最大风险
- 下标从 0 还是 1 要统一
- 未初始化的局部数组值不一定可用
- 多维数组行列别写反

## 🚀 Performance / Tips（性能优化）

- 数组随机访问是 $O(1)$
- 数据量已知时，数组非常直接高效
- 题目更灵活时，常会用 `vector` 替代普通数组
- 写数组题时先把范围和下标想清楚

## 🧪 Common Scenarios（常见使用场景）

- 存一串数字
- 前缀和
- 统计频率
- 排序前存储
- 二维网格

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

int main() {
    int a[3] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) cout << a[i] << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **数组就是：一段连续存储、支持按下标快速访问的数据结构。**