---
tags:
---
 #heap #priority-queue #complete-binary-tree #cpp


## 核心

- **Heap = 支持快速维护、取出当前最值的完全二叉树结构**
- **priority_queue = 系统自带的 Heap**
- 相关特征：
    - 动态维护最大值 / 最小值
    - 每次都取当前最优元素
    - Top K
    - 贪心过程中的“反复取最好”
	

- 大根堆：父节点 >= 子节点
- 小根堆：父节点 <= 子节点

- 父节点下标 → 左孩子：`index * 2 + 1`
- 父节点下标 → 右孩子：`index * 2 + 2`
- 孩子节点下标 → 父节点：`(index - 1) / 2`


## 高频模板（C++）

## 1. 大根堆核心操作

```cpp
	priority_queue<int> pq;
	priority_queue<int, vector<int>, greater<int>> pq1;
	pq.push(3);
	while (!pq.empty()) {
		cout << pq.top() << " ";
		pq.pop();
	}
```

## 2. 大根堆核心操作

**heapInsert ( )（向上调整 - 建堆 / 插入）**

```cpp
void heapInsert(vector<int>& arr, int index) {
    while (arr[index] > arr[(index - 1) / 2]) {
        swap(arr[index], arr[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}
```

**heapify ( )（向下调整 - 堆化 / 排序）**

```cpp
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

## 3. 建堆（两种）

**方式 1：heapInsert ( ) 正序建堆（O (n log n)）** 

```cpp
for (int i = 0; i < (int)arr.size(); ++i) {
    heapInsert(arr, i);
}
```

**方式 2：heapify ( ) 逆序建堆（O (n)，最优）** 

```cpp
for (int i = (int)arr.size() / 2 - 1; i >= 0; --i) {
    heapify(arr, i, arr.size());
}
```

### 4. 堆排序（大根堆 → 升序）

```cpp
void heapSort(vector<int>& arr){
    if (arr.empty() || (int)arr.size() < 2) return ;
    for (int i = (int)arr.size() - 1; i > 0; --i) {
        heapify(arr, i, arr.size());
    }
    // for (int i = 0; i < (int)arr.size(); ++i){
    //     heapInsert(arr, i);
    // }
    int heapSize = arr.size();
    swap(arr[0], arr[--heapSize]);
    while (heapSize > 0) {
        heapify(arr, 0, heapSize);
        swap(arr[0], arr[--heapSize]);
    }
}
```

### 5. 小根堆核心操作（仅改比较符号）

### 6. STL 优先队列（实战常用）

```cpp
#include <queue>
using namespace std;

priority_queue<int> maxHeap;

priority_queue<int, vector<int>, greater<int>> minHeap;
```

## 7. Top K

**「前 K 大」的元素（小根堆实现）**

```cpp
vector<int> getTopKMax(const vector<int>& arr, int k) {
	
    priority_queue<int, vector<int>, greater<int>> minHeap;
    
    for (int num : arr) {
        if (minHeap.size() < k) {
            minHeap.push(num);
        } else if (num > minHeap.top()) {
            minHeap.pop();
            minHeap.push(num);
        }
    }
    
    vector<int> res;
    while (!minHeap.empty()) {
        res.push_back(minHeap.top());
        minHeap.pop();
    }
    reverse(res.begin(), res.end())
    return res;
}
```

**「前 K 小」的元素（大根堆实现）**

```cpp
vector<int> getTopKMin(const vector<int>& arr, int k) {
	
    priority_queue<int> maxHeap;
	
    for (int num : arr) {
        if (maxHeap.size() < k) {
            maxHeap.push(num);
        } else if (num < maxHeap.top()) {
            maxHeap.pop();
            maxHeap.push(num);
        }
    }

    vector<int> res;
    while (!maxHeap.empty()) {
        res.push_back(maxHeap.top());
        maxHeap.pop();
    }
    reverse(res.begin(), res.end());
    return res;
}
```