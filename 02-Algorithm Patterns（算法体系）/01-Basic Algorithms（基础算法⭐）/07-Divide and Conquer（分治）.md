#algorithm #divide-and-conquer #recursion #merge-sort

## 核心

- 分治 = 分成子问题 + 分别解决 + 合并答案
- 关键词：
    - 归并排序
    - 快速排序
    - 线段树思想
    - 大问题拆小问题

## 通用模板

```cpp
Answer solve(int l, int r) {
    if (边界) return base;
    int mid = (l + r) >> 1;
    auto left = solve(l, mid);
    auto right = solve(mid + 1, r);
    return merge(left, right);
}
```

## 高频例子

### 归并排序

```cpp
void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mid = (l + r) >> 1;
    mergeSort(a, l, mid);
    mergeSort(a, mid + 1, r);
    vector<int> tmp;
    int i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (a[i] <= a[j]) tmp.push_back(a[i++]);
        else tmp.push_back(a[j++]);
    }
    while (i <= mid) tmp.push_back(a[i++]);
    while (j <= r) tmp.push_back(a[j++]);
    for (int k = 0; k < (int)tmp.size(); ++k) a[l + k] = tmp[k];
}
```

### 分治求数组最大值

```cpp
int solve(vector<int>& a, int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    return max(solve(a, l, mid), solve(a, mid + 1, r));
}
```

## 高频坑点

- 只会“分”，不会“合”
- 边界条件没收好
- 合并过程复杂度失控

## 只记这个

- 分治不是单纯递归，重点是“合并子问题结果”

## 一句话

- **分治的核心，是把大问题拆成同类小问题，再把小问题答案组合回来。**