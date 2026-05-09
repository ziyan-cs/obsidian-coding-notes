
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