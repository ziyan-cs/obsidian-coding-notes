#data-structure #monotonic-stack #stack #next-greater #optimization

## ⚡ TL;DR（快速决策）

- 单调栈本质是：**通过维持栈内单调性，在线性时间内处理最近更大 / 更小元素问题**
- 相关特征：
    - 下一个更大元素
    - 下一个更小元素
    - 柱状图最大矩形
    - 某元素左边 / 右边第一个满足条件的位置
- 它不是新容器，而是：**栈的一种高频技巧**

## 🧩 Core Idea（核心本质）

- 栈中元素保持单调递增或递减
- 当新元素到来时，会把不再可能有用的旧元素弹掉
- 一句话理解：
    - **让栈里只保留未来仍可能有价值的候选人。**
- 为什么常存下标：
    - 需要知道元素位置
    - 需要反查值并计算距离

## 🔧 Usage Patterns（可复用代码模板）

1. 下一个更大元素

```cpp
vector<int> ans(n, -1);
stack<int> st;
for (int i = 0; i < n; ++i) {
    while (!st.empty() && a[st.top()] < a[i]) {
        ans[st.top()] = i;
        st.pop();
    }
    st.push(i);
}
```

1. 维护递增栈

```cpp
while (!st.empty() && a[st.top()] >= a[i]) st.pop();
```

1. 左边第一个更小元素思路

```cpp
while (!st.empty() && a[st.top()] >= a[i]) st.pop();
left[i] = st.empty() ? -1 : st.top();
st.push(i);
```

## ⚠️ Pitfalls（高频错误）

- 搞不清维护递增还是递减
- 栈里该存值还是存下标没想清楚
- `<`、`<=`、`>`、`>=` 的选择会影响重复元素处理
- 忘记某些元素直到最后都没被弹出

## 🚀 Performance / Tips（性能优化）

- 许多题能从 $O(n^2)$ 优化到 $O(n)$
- 每个元素通常最多进栈一次、出栈一次
- 高频经验：
    - 单调栈几乎默认存下标
    - 题目问“最近第一个更大 / 更小”时高度敏感

## 🧪 Common Scenarios（常见使用场景）

- Next Greater Element
- 柱状图最大矩形
- 温度题 / 股票跨度题
- 左右边界查找
- 区间贡献法基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <stack>
#include <vector>
using namespace std;
int main() {
    vector<int> a = {2, 1, 3};
    stack<int> st;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!st.empty() && a[st.top()] < a[i]) st.pop();
        st.push(i);
    }
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **单调栈就是：通过维护栈内单调性，在线性时间内处理最近更大 / 更小元素问题。**