#algorithm #sorting #sort #comparison-sort #non-comparison-sort

## 核心：

- 排序 4 要素：
    - 时间复杂度
    - 空间复杂度
    - 稳定性
    - 原地性
- 必会：
    - 归并排序
    - 快速排序

##  核心排序算法对比：

- 插入：$O(n^2)$  $O(1)$                          稳定，原地    近乎有序时很强，适合小数据
- 归并：$O(n log n)$  $O(n)$                     稳定，非原地   时间稳定，适合逆序对拓展
- 快排：$O(n log n)$  $O(log n)$             不稳定，通常原地   工程最常用，平均性能最优
- 堆排：$O(n log n)$  $O(1)$                  不稳定，原地   适合空间敏感场景
- 计数：$O(n+k)$  $O(k)$                  可稳定，非原地   适合值域小的整数，线性时间
- 希尔：$O(n log n)$  $O(1)$                  不稳定，原地   依赖 gap 序列
- 桶排：$O(n)$  $O(n)$                        不稳定，非原地   依赖数据分布
- 基数：$O(n)$  $O(n)$                           稳定，非原地   适合整数 / 定长串


---


## 1. 冒泡排序（Bubble Sort）

```cpp
void bubbleSort(vector<int>& arr) {
    int len = arr.size();
    for (int i = 0; i < len - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < len - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        // 无交换则提前终止
        if (!swapped) break;
    }
}
```

## 2. 选择排序（Selection Sort）

```cpp
void selectionSort(vector<int>& arr) {
    int len = arr.size();
    for (int i = 0; i < len; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < len; ++j) {
            if (arr[j] < arr[minIdx]) minIdx = j;
        }
        swap(arr[i], arr[minIdx]);
    }
}
```

## 3. 插入排序（Insertion Sort）⭐

```cpp
void insertSort(vector<int>& arr) {
    int len = arr.size();
    for (int i = 1; i < len; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}
```

## 4. 归并排序（Merge Sort）⭐

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
    int index = 0, p1 = L, p2 = M + 1;
    while (p1 <= M && p2 <= R)
        help[index++] = arr[p1] < arr[p2] ? arr[p1++] : arr[p2++];
    while (p1 <= M) help[index++] = arr[p1++];
    while (p2 <= R) help[index++] = arr[p2++];
    for (int i = 0; i < (int)help.size(); ++i)
        arr[L + i] = help[i];
}
```

## 5. 快速排序（Quick Sort）⭐

```cpp
void quickSort(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int i = l, j = r;
    int pivot = a[(l + r) >> 1];
    while (i <= j) {
        while (arr[i] < pivot) ++i;
        while (arr[j] > pivot) --j;
        if (i <= j) swap(arr[i++], arr[j--]);
    }
    if (l < j) quickSort(arr, l, j);
    if (i < r) quickSort(arr, i, r);
}
```

## 6. 堆排序（Heap Sort）⭐

```cpp
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int l = i * 2 + 1, r = i * 2 + 2;
    if (l < n && a[l] > arr[largest]) largest = l;
    if (r < n && a[r] > arr[largest]) largest = r;
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr) {
    int len = arr.size();
    for (int i = n / 2 - 1; i >= 0; --i) heapify(arr, n, i);
    for (int i = n - 1; i > 0; --i) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}
```

## 7. 计数排序（Counting Sort）

```cpp
void countingSort(vector<int>& arr) {
    int minIdx = *max_element(arr.begin(), arr.end());
    vector<int> cnt(minIdx + 1, 0);
    for (int x : a) cnt[x]++;
    int idx = 0;
    for (int v = 0; v <= minIdx; ++v) {
        while (cnt[v]--) a[idx++] = v;
    }
}
```


## 8. 希尔排序（Shell Sort）

```cpp
void shellSort(vector<int>& arr) {
    int n = arr.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            int temp = arr[i], j = i;
            while (j >= gap && a[j - gap] > temp) {
                arr[j] = a[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}
```

## 9. 桶排序（Bucket Sort）

```cpp
void bucketSort(vector<float>& a) {
    int n = a.size();
    vector<vector<float>> buckets(n);
    for (float x : a) {
        int idx = x * n;
        if (idx == n) idx = n - 1;
        buckets[idx].push_back(x);
    }
    for (auto& bucket : buckets) sort(bucket.begin(), bucket.end());
    int k = 0;
    for (auto& bucket : buckets) {
        for (float x : bucket) a[k++] = x;
    }
}
```

## 10. 基数排序（Radix Sort）

```cpp
void radixSort(vector<int>& a) {
    int mx = *max_element(a.begin(), a.end());
    for (int exp = 1; mx / exp > 0; exp *= 10) {
        vector<int> output(a.size());
        vector<int> cnt(10, 0);
        for (int x : a) cnt[(x / exp) % 10]++;
        for (int i = 1; i < 10; ++i) cnt[i] += cnt[i - 1];
        for (int i = (int)a.size() - 1; i >= 0; --i) {
            int d = (a[i] / exp) % 10;
            output[--cnt[d]] = a[i];
        }
        a = output;
    }
}
```