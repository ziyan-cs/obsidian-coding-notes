---
tags:
  - algorithm
  - basics
---

> **核心考点**：单调递增/递减栈维护、下一个更大/更小元素 O(n) 模板、每日温度/接雨水等经典问题


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
    heights.push_back(0);       // 哨兵，触发清空
    stack<int> st;
    st.push(-1);
    int res = 0;
    for (int i = 0; i < (int)heights.size(); i++) {
        while (st.top() != -1 && heights[i] < heights[st.top()]) {
            int h = heights[st.top()]; st.pop();
            int w = i - st.top() - 1;
            res = max(res, h * w);
        }
        st.push(i);
    }
    return res;
}
```

---

## 关联笔记

- [BFS with Queue (队列BFS)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/03-Stack%20&%20Queue%20(栈与队列)%20⭐/03b-BFS%20with%20Queue%20(队列BFS).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
- [Fast & Slow Pointers (快慢指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02b-Fast%20&%20Slow%20Pointers%20(快慢指针).md)
- [Hash Table (哈希表)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/04-Hash%20Table%20(哈希表)%20⭐.md)
