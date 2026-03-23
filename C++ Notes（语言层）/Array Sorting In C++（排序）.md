#cpp #arr
## 一、常见排序算法概览

- **基础排序**：冒泡排序、选择排序、插入排序
- **进阶排序**：希尔排序、快速排序、归并排序、堆排序
- **面试重点**：插入排序、快速排序、归并排序
- **工程实践**：优先使用 `std::sort`

---

## 二、各排序方法总结

### 1. 冒泡排序

- **思想**：不断比较相邻元素，把较大的元素交换到后面
- **时间复杂度**：最好 $O(n)$，平均/最坏 $O(n^2)$
- **空间复杂度**：$O(1)$
- **稳定性**：稳定
- **适用场景**：适合教学演示，实际开发中很少直接使用

```cpp
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}
```

### 2. 选择排序

- **思想**：每轮从未排序部分选出最小值，放到前面
- **时间复杂度**：$O(n^2)$
- **空间复杂度**：$O(1)$
- **稳定性**：不稳定
- **适用场景**：数据量小、交换次数要求少时可用

```cpp
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        swap(arr[i], arr[minIdx]);
    }
}
```

### 3. 插入排序

- **思想**：把当前元素插入到前面已经排好序的合适位置
- **时间复杂度**：最好 $O(n)$，平均/最坏 $O(n^2)$
- **空间复杂度**：$O(1)$
- **稳定性**：稳定
- **适用场景**：小规模数据、近乎有序的数据

```cpp
template <typename T>
void insertionSort(vector<T>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
```

### 4. 希尔排序

- **思想**：先按较大间隔分组进行插入排序，再逐步缩小间隔
- **时间复杂度**：与步长序列有关，常见约为 $O(n^{1.3})$
- **空间复杂度**：$O(1)$
- **稳定性**：不稳定
- **适用场景**：中等规模数据，性能优于简单排序

```cpp
void shellSort(vector<int>& arr) {
    int n = arr.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}
```

### 5. 快速排序

- **思想**：选基准元素，把数组划分为左右两部分后递归排序
- **时间复杂度**：平均 $O(nlog n)$，最坏 $O(n^2)$
- **空间复杂度**：$O(log n)$
- **稳定性**：不稳定
- **适用场景**：大多数通用排序场景，实际应用非常广泛

```cpp
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
```

### 6. 归并排序

- **思想**：不断二分数组，分别排序后再合并
- **时间复杂度**：始终为 $O(n\log n)$
- **空间复杂度**：$O(n)$
- **稳定性**：稳定
- **适用场景**：需要稳定排序，或者适合外部排序时使用

```cpp
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int p = 0; p < k; p++) {
        arr[left + p] = temp[p];
    }
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = (left + right) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}
```

### 7. 堆排序

- **思想**：先构建最大堆，再不断把堆顶最大元素放到数组末尾
- **时间复杂度**：$O(nlog n)$
- **空间复杂度**：$O(1)$
- **稳定性**：不稳定
- **适用场景**：空间受限，但希望复杂度稳定在 $O(n\log n)$

```cpp
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr) {
    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}
```

---

## 三、对比记忆

- **稳定排序**：冒泡、插入、归并
- **不稳定排序**：选择、希尔、快速、堆
- **平均效率高**：快速、归并、堆
- **最省空间**：冒泡、选择、插入、希尔、堆

---

## 四、实用建议

- **日常开发**：直接使用 C++ 标准库 `std::sort`
- **面试手写重点**：插入排序、快速排序、归并排序
- **需要稳定性**：选择归并排序
- **空间受限**：选择堆排序
- **数据量小或接近有序**：选择插入排序

---

## 五、最简记忆版

- **冒泡**：相邻交换，稳定，慢
- **选择**：每次选最小，不稳定，交换少
- **插入**：插到合适位置，稳定，适合小规模
- **希尔**：分组插入，不稳定，比插入快
- **快排**：分治，平均最快，不稳定
- **归并**：分治加合并，稳定，但耗空间
- **堆排**：建堆排序，不稳定，省空间