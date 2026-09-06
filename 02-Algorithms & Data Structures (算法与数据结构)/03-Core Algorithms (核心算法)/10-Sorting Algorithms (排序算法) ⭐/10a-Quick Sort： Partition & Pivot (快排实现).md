---
tags:
  - cs/algorithm
status: 🌱
---

> [!important] **核心考点**：快排的 partition 函数实现（Lomuto/Hoare）、pivot 选择策略、递归与迭代栈实现
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

## 快排 2.0

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

## 快排 3.0

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

---

## 关联笔记

- [Merge Sort & Inversion Count (归并排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Heap Sort (堆排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Sorting Algorithms (排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
