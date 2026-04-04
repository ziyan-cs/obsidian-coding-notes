#algorithm #binary-search #search #monotonicity

## 核心

- 本质是：**在单调性上快速缩小答案区间**
- 关键词：
    - 有序数组
    - 单调性
    - 找边界
    - 二分答案

## 必会三类

### 1. 经典查值

```cpp
int binarySearch(vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (a[mid] == target) return mid;
        if (a[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}
```

### 2. 找左边界

```cpp
int lowerBound(vector<int>& a, int target) {
    int l = 0, r = (int)a.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (a[mid] >= target) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

### 3. 二分答案

- 前提：答案具有单调可判定性

```cpp
int binaryAnswer(int l, int r) {
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (check(mid)) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

## 什么时候想到二分

- 数组有序
- 问“第一个满足条件的位置”
- 问“最小的 x 使得 check(x) 成立”
- 暴力枚举答案太慢，但答案区间明显且有单调性

## 高频题型模板

### 搜索插入位置

```cpp
int searchInsert(vector<int>& nums, int target) {
    int l = 0, r = nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] >= target) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

### 在有序数组中统计出现次数

```cpp
int countTarget(vector<int>& a, int x) {
    auto L = lower_bound(a.begin(), a.end(), x);
    auto R = upper_bound(a.begin(), a.end(), x);
    return R - L;
}
```

## 高频坑点

- 循环条件 `l <= r` 和 `l < r` 混用
- 边界模板和 mid 更新不统一
- 忘记二分答案的前提是“check 结果单调”
- `mid = (l + r) / 2` 在极端情况下可能溢出

## 只记这个

- 有序数组找位置 -> 二分
- 找边界 -> lower/upper bound 模板
- 不在数组上也能二分，只要答案满足单调性