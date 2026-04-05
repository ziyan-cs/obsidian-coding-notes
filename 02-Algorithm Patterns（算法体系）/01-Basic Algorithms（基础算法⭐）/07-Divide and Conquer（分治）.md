#algorithm #divide-and-conquer #recursion #merge-sort

## 核心

- 分治 = **拆分 + 合并的递归**
- 关键词：
    - 归并排序
    - 逆序对
    - 快速排序
    - 最大子数组和
    - 线段树思想
    - 大问题拆小问题

## 经典题型：

### 1. 归并排序

```cpp
// 主函数：归并分治
void merge(vector<int>& arr, int L, int R) {
    if (L >= R) return;
    int M = L + ((R - L) >> 1);
    merge(arr, L, M);
    merge(arr, M + 1, R);
    mergeSort(arr, L, M, R);
}

// 子函数：合并+两边排序
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
-->**（LeetCode 912 ）**

### 2. 统计逆序对

```cpp
// 主函数：归并分治
int merge(vector<int>& arr, int L, int R) {
    if (L >= R) return 0;
    int M = L + ((R - L) >> 1);
    int left = merge(arr, L, M);
    int right = merge(arr, M + 1, R);
    int cross = mergeAdd(arr, L, M, R);
    return left + right + cross;
}

// 子函数：合并+两边排序+统计逆序对
int mergeAdd(vector<int>& arr, int L, int M, int R) {
    vector<int> help(R - L + 1);
    int index = 0;
    int p1 = L;
    int p2 = M + 1;
    int count = 0;
    while (p1 <= M && p2 <= R) {
        if (arr[p1] > arr[p2]) {
            count += M - p1 + 1;
            help[index++] = arr[p2++];
        } else {
            help[index++] = arr[p1++];
        }
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
    return count;
}
```
-->**（LeetCode 315 ）**
-->**（LeetCode 493 ）**

### 3. 快速排序（分治经典，和归并排序并列）

```cpp
// 主函数：归并分治
void solve(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mid = partition(a, l, r);
    solve(a, l, mid - 1);
    solve(a, mid + 1, r);
}

// 子函数：合并+排序
int partition(vector<int>& a, int l, int r) {
    int pivot = a[r];
    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (a[j] < pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[r]);
    return i + 1;
}
```
-->**（LeetCode 912 ）**

### 4. 最大子数组和（分治版）

```cpp
#include <climits> // INT_MIN的头文件 
#include <algorithm> // max()的头文件

// 主函数：归并分治
int solve(vector<int>& a, int l, int r) {
    if (l == r) return a[l];
    int mid = (l + r) >> 1;
    int leftMax = solve(a, l, mid);
    int rightMax = solve(a, mid + 1, r);
    int crossMax = getCrossSum(a, l, mid, r);
    return max(max(leftMax, rightMax), crossMax);
}

// 子函数：合并+求和
int getCrossSum(vector<int>& a, int l, int mid, int r) {
    int leftSum = INT_MIN;
    int sum = 0;
    for (int i = mid; i >= l; --i) {
        sum += a[i];
        leftSum = max(leftSum, sum);
    }
    int rightSum = INT_MIN;
    sum = 0;
    for (int i = mid + 1; i <= r; ++i) {
        sum += a[i];
        rightSum = max(rightSum, sum);
    }
    return leftSum + rightSum;
}
```
 -->**（LeetCode 53 ）**