---
tags:
  - cs/algorithm
status: 🌱
---

> **核心考点**：堆排序的下滤建堆 O(n)、堆顶与末尾交换、不稳定排序特性

```cpp
// 大顶堆（默认）
priority_queue<int> maxHeap;
// 小顶堆
priority_queue<int, vector<int>, greater<int>> minHeap;
```


```cpp
// 对外接口
void heapSort(vector<int>& arr) {
	if (arr.empty() || arr.size() < 2) return;
	int heapSize = arr.size();
    for (int i = (int)arr.size() - 1; i > 0; --i) {
        heapify(arr, i, arr.size());
    }
    swap(arr[0], arr[--heapSize]);
    while (heapSize > 0) {
        heapify(arr, 0, heapSize);
        swap(arr[0], arr[--heapSize]);
    }
}
// 子函数：堆化
void heapify(vector<int>& arr, int index, int heapSize) {
    int L = index * 2 + 1;
    while (L < heapSize) {
        int largest = L + 1 < heapSize && arr[L + 1] > arr[L] ? L + 1 : L;
        largest = arr[index] > arr[largest] ? index : largest;
        if (largest == index) return ;
        swap(arr[index] ,arr[largest]);
        index = largest;
        L = index *2 + 1;
    }
}
```

---

## 关联笔记

- [Quick Sort： Partition & Pivot (快排实现)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Merge Sort & Inversion Count (归并排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Sorting Algorithms (排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03%20·%20核心算法/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01%20·%20基础数据结构/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
