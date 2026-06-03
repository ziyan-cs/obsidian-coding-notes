---
tags:
  - algorithm
  - core-algorithm
---

> **核心考点**：快排的 partition 函数实现（Lomuto/Hoare）、pivot 选择策略、递归与迭代栈实现
# 快排 1.0

```cpp
void quickSortV1(vector<int>& arr, int L, int R) {
    if (L >= R) return;
    // partition
    int p1 = L - 1;
    int key = arr[R];
    for (int i = L; i < R; ++i) {
        if (arr[i] < key) {
            swap(arr[++p1], arr[i]);
        }
    }
    swap(arr[++p1], arr[R]);
		
    quickSortV1(arr, L, p1 - 1);
    quickSortV1(arr, p1 + 1, R);
}
```

# 快排 2.0

```cpp
void quickSortV2(vector<int>& arr, int L, int R) {
    if (L >= R) return;
    // partition
    int p1 = L - 1;
    int p2 = R;
    int key = arr[R];
    int i = L;
    while (i < p2) {
        if (arr[i] == key) {
            ++i;
        } else if (arr[i] < key) {
            swap(arr[++p1], arr[i]);
            ++i;
        } else {
            swap(arr[--p2], arr[i]);
        }
    }
    swap(arr[p2], arr[R]);
	    
    quickSortV2(arr, L, p1);
    quickSortV2(arr, p2 + 1, R);
}
```

# 快排 3.0

```cpp
void quickSortV3(vector<int>& arr, int L, int R) {
    if (L >= R) return;
    swap(arr[L + rand() % (R - L + 1)], arr[R]);
    // partition
    int p1 = L - 1;
    int p2 = R;
    int key = arr[R];
    int i = L;
    while (i < p2) {
        if (arr[i] == key) {
            ++i;
        } else if (arr[i] < key) {
            swap(arr[++p1], arr[i]);
            ++i;
        } else {
            swap(arr[--p2], arr[i]);
        }
    }
    swap(arr[p2], arr[R]);
	    
    quickSortV3(arr, L, p1);
    quickSortV3(arr, p2 + 1, R);
}
```