---
tags:
  - cs/algorithm
status: 🌱
---

# Monotonic Stack — 单调栈

> [!abstract] 核心考点：单调递增/递减栈维护、下一个更大/更小元素 O(n) 模板、每日温度/接雨水等经典问题


单调栈维护一个**严格单调递增或递减**的栈，用于解决"下一个更大/更小元素"类问题，时间复杂度 O(n)（每个元素最多入栈出栈一次）。

## 模板：下一个更大元素

```cpp
vector<int> nextGreaterElement(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;  // 存下标
    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[i] > nums[st.top()]) {
            res[st.top()] = nums[i];
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```

## 典型应用

### 接雨水

```cpp
int trap(vector<int>& height) {
    stack<int> st;
    int water = 0;
    for (int i = 0; i < (int)height.size(); i++) {
        while (!st.empty() && height[i] > height[st.top()]) {
            int top = st.top(); st.pop();
            if (st.empty()) break;
            int width   = i - st.top() - 1;
            int bounded = min(height[i], height[st.top()]) - height[top];
            water += width * bounded;
        }
        st.push(i);
    }
    return water;
}
```

### 柱状图中最大矩形（单调递增栈）

```cpp
int largestRectangleArea(vector<int>& heights) {
    vector<int> h = heights;
    h.push_back(0);             // 副本上的哨兵，避免修改调用者输入
    stack<int> st;
    st.push(-1);
    int res = 0;
    for (int i = 0; i < (int)h.size(); i++) {
        while (st.top() != -1 && h[i] < h[st.top()]) {
            int height = h[st.top()]; st.pop();
            int w = i - st.top() - 1;
            res = max(res, height * w);
        }
        st.push(i);
    }
    return res;
}
```

---

## 30 秒回答

**单调栈为什么是 O(n)？** 每个下标最多入栈一次、弹栈一次，虽然有嵌套 `while`，总弹栈次数仍不超过 `n`。先决定栈内维持递增还是递减，再明确“当前元素到来时，谁的答案被确定”。

**自测：** 下一个更大元素为何在弹栈时确定答案？柱状图题为什么要在末尾放哨兵？

## 关联笔记

- [BFS with Queue (队列BFS)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Hash Table (哈希表)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/04-Hash%20Table%20(哈希表)%20⭐.md)
