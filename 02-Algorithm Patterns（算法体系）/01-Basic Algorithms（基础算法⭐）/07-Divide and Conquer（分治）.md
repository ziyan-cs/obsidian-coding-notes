#algorithm #divide-and-conquer #recursion #merge-sort

## 核心

- 分治 = **拆分 + 合并的递归**
- 关键词：
    - 归并排序
    - 逆序对
    - 快速排序
    - 线段树思想
    - 大问题拆小问题

## 经典题型：

### 1. 归并排序

```cpp
void merge(vector<int>& arr, int L, int R) {
    if (L == R) return;
    int M = L + ((R - L) >> 1);
    merge(arr, L, M);
    merge(arr, M + 1, R);
    mergeSort(arr, L, M, R);
}

void mergeSort(vector<int>& arr, int L, int M, int R) {
    vector<int> help(R - L + 1);
    int index = 0;
    int p1 = L;
    int p2 = M + 1;
    while (p1 <= M && p2 <= R) {
        help[index++] = arr[p1] < arr[p2] ? arr[p1++] : arr[p2++];
    }
    while (p1 <= M) {
        help[index++] = arr[p1++];
    }
    while (p2 <= R) {
        help[index++] = arr[p2++];
    }
    for (int i = 0; i < (int)help.size(); ++i) {
        arr[L + i] = help[i];
    }
}
```
**（LeetCode 912）**

### 2. 逆序对

```cpp
int solve(vector<int>& a, int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    return max(solve(a, l, mid), solve(a, mid + 1, r));
}
```

### 3. 快速排序（分治经典，和归并排序并列）

```cpp
int solve(vector<int>& a, int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    return max(solve(a, l, mid), solve(a, mid + 1, r));
}
```

### 4. 最大子数组和（分治版）

```cpp
int solve(vector<int>& a, int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    return max(solve(a, l, mid), solve(a, mid + 1, r));
}
```
**（LeetCode 53）**

## 高频坑点

- 只会“分”，不会“合”
- 边界条件没收好
- 合并过程复杂度失控