#data-structure #difference-array #range-update #prefix-sum #array

## 核心

- **把区间加减操作压缩成端点修改，最后再统一还原原数组**
- 相关特征：
    - 多次区间加 / 区间减
    - 最后统一问每个位置的值
    - 需要把很多区间修改高效合并
- 核心思路：
    - 在左端点加
    - 在右端点后一个位置减

## 🧩 Core Idea（核心本质）

- 差分数组 `diff` 记录相邻位置的变化量
- 对区间 `[l, r]` 加 `x`：
    - `diff[l] += x`
    - `diff[r + 1] -= x`
- 最后对 `diff` 做一次前缀和，就能还原最终数组
- 一句话理解：
    - **把“改整段”变成“改两个端点”。**
- 它和前缀和是反向关系：
    - 前缀和擅长查询
    - 差分擅长批量区间修改

## 🔧 Usage Patterns（可复用代码模板）

1. 一维差分模板

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<int> diff(n + 2, 0);

    int l = 2, r = 4, x = 3;
    diff[l] += x;
    diff[r + 1] -= x;

    vector<int> a(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        a[i] = a[i - 1] + diff[i];
        cout << a[i] << ' ';
    }
}
```

1. 基于原数组构造差分

```cpp
diff[1] = a[1];
for (int i = 2; i <= n; ++i) diff[i] = a[i] - a[i - 1];
```

1. 二维差分思路

```cpp
// 矩形区域更新时可扩展到二维差分
```

## ⚠️ Pitfalls（高频错误）

- 忘了给 `r + 1` 留空间
- 下标从 0 / 1 开始没统一
- 差分修改后忘记最后做前缀还原
- 只适合“最后统一看结果”的类型，边改边查未必适合

## 🚀 Performance / Tips（性能优化）

- 每次区间修改：$O(1)$
- 最终还原：$O(n)$
- 若有很多区间修改、最后只输出一次结果，差分非常强
- 若还要实时查询，则可能改用树状数组或线段树

## 🧪 Common Scenarios（常见使用场景）

- 航班预订统计
- 区间加法操作
- 多次覆盖 / 增减后统一输出
- 二维矩阵区间增量

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n = 5;
    vector<int> diff(n + 2, 0);
    diff[2] += 5;
    diff[4] -= 5;
    int cur = 0;
    for (int i = 1; i <= n; ++i) {
        cur += diff[i];
        cout << cur << ' ';
    }
}
```
