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
