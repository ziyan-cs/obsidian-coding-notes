#algorithm #sorting #comparison-sort #stable-sort #cpp

## ⚡ TL;DR（快速决策）

- 排序本质是：**把一组无序数据按某种规则重排成有序序列**
- 一看到这些需求，要优先想到排序：
    - 从小到大 / 从大到小输出
    - 去重前预处理
    - 区间合并、贪心、二分前置步骤
    - 需要把“乱序问题”变成“有序问题”
- 排序不只是最终目的，很多时候是**问题求解的前处理**
- 常见分类：
    - 比较排序：冒泡、选择、插入、归并、快速、堆排
    - 非比较排序：计数、桶、基数（有前提）

## 🧩 Core Idea（核心本质）

- 排序的目标是建立顺序关系
- 一旦序列有序，很多操作就会变简单：
    - 查找更快
    - 相邻关系更清晰
    - 去重更方便
    - 区间问题更自然
- 一句话理解：
    - **排序就是把“杂乱无章”变成“规则清晰”。**
- 做题时要先问自己：
    - 需要稳定性吗？
    - 需要原地吗？
    - 数据范围和复杂度要求是什么？

## 🔧 Usage Patterns（可复用代码模板）

1. `sort` 基础使用

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {5, 2, 4, 1, 3};
    sort(a.begin(), a.end());
    for (int x : a) cout << x << ' ';
}
```

1. 自定义降序

```cpp
sort(a.begin(), a.end(), greater<int>());
```

1. 自定义比较器

```cpp
sort(a.begin(), a.end(), [](const pair<int,int>& x, const pair<int,int>& y) {
    if (x.first != y.first) return x.first < y.first;
    return x.second < y.second;
});
```

1. 稳定排序

```cpp
stable_sort(a.begin(), a.end(), cmp);
```

1. 去重常见套路

```cpp
sort(a.begin(), a.end());
a.erase(unique(a.begin(), a.end()), a.end());
```

## ⚠️ Pitfalls（高频错误）

- 最常见错误 1：比较器不满足严格弱序，导致结果异常
- 最常见错误 2：以为 `sort` 是稳定排序，其实默认不是
- 最常见错误 3：排序后忘了原下标信息丢失
- 最常见错误 4：想去重却只用了 `unique`，没先排序
- 最常见错误 5：复杂对象排序时比较规则没写完整

## 🚀 Performance / Tips（性能优化）

- `sort` 平均复杂度通常按 $O(n \log n)$ 记
- `stable_sort` 常也可接受，但常数可能更大
- 数据范围很小且离散时，计数排序可能更优
- 高频经验：
    - 能用库排序就优先用库排序
    - 排序题很多本质是“排序 + 扫描”
    - 排序后通常要关注相邻元素关系

## 🧪 Common Scenarios（常见使用场景）

- 数组从小到大排序
- 区间按左端点排序
- 贪心前按某关键字排序
- 二分前预排序
- 去重、离散化、排名处理

## 🆚 Sorting vs Heap / Selection

- 排序：得到整体有序结果
- 堆：擅长动态维护当前最值，不保证整体有序
- 选择算法：只关心第 k 大 / 小，不一定要完整排序

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a = {4, 1, 3, 2};
    sort(a.begin(), a.end());
    for (int x : a) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **排序就是：按指定规则重排数据，让后续查找、比较和处理都更容易。**