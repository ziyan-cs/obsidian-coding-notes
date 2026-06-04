---
tags:
  - algorithm
  - core-algorithm
---

> **核心考点**：归并排序的分治思想、merge 操作、逆序对计数、归并排序的稳定性

```cpp
// 对外接口
void mergeSort(vector<int>& arr, int L, int R) {
    if (L == R) return;
    int M = L + ((R - L) >> 1);
    mergeSort(arr, L, M);
    mergeSort(arr, M + 1, R);
    merge(arr, L, M, R);
}

// 子函数：合并
void merge(vector<int>& arr, int L, int M, int R) {
    vector<int> help(R - L + 1);
    int index = 0;
    int p1 = L;
    int p2 = M + 1;
    while (p1 <= M && p2 <= R)
	    help[index++] = arr[p1] < arr[p2] ? arr[p1++] : arr[p2++];
    while (p1 <= M) 
	    help[index++] = arr[p1++];
    while (p2 <= R) 
	    help[index++] = arr[p2++];
    for (int i = 0; i < (int)help.size(); ++i)
        arr[L + i] = help[i];
}
```

---

## 关联笔记

- [Quick Sort： Partition & Pivot (快排实现)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Heap Sort (堆排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Sorting Algorithms (排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
