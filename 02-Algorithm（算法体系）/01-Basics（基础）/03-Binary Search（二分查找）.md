#algorithm #binary-search #search #monotonicity

## 核心

- **在单调性上快速缩小答案区间**
- 关键词：
    - 有序数组
    - 单调性
    - 找边界
    - 二分答案

## 必会三类：

### 1. 经典查值

```cpp
int binarySearch(vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l <= r) {
        int mid = l + ((r - l) >> 1);
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
        int mid = l + ((r - l) >> 1);
        if (a[mid] >= target) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

### 3. 二分答案


```cpp
int binaryAnswer(int l, int r) {
    while (l < r) {
        int mid = l + ((r - l) >> 1);
        if (check(mid)) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

## 经典题型：

### 1. 搜索插入位置

```cpp
int searchInsert(vector<int>& nums, int target) {
    int l = 0, r = nums.size();
    while (l < r) {
        int mid = l + ((r - l) >> 1);
        if (nums[mid] >= target) r = mid;
        else l = mid + 1;
    }
    return l;
}
```

### 2. 在有序数组中统计出现次数

```cpp
int countTarget(vector<int>& a, int x) {
    auto L = lower_bound(a.begin(), a.end(), x);
    auto R = upper_bound(a.begin(), a.end(), x);
    return R - L;
}
```