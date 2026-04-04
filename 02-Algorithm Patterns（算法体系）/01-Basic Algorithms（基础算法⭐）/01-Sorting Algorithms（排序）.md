#algorithm #sorting #sort #comparison-sort #non-comparison-sort

## ⚡ TL;DR

- 排序先记 4 件事：
    - 时间复杂度
    - 空间复杂度
    - 是否稳定
    - 是否原地
- 必会核心：
    - 插入排序
    - 归并排序
    - 快速排序
    - 堆排序
    - 计数排序
    - 基数排序

## 1. 总分类

### 比较排序

- 冒泡排序
- 选择排序
- 插入排序
- 希尔排序
- 归并排序
- 快速排序
- 堆排序
### 非比较排序

- 计数排序
- 桶排序
- 基数排序

---

## 2. 核心对比速记

- 冒泡：稳定，原地，$O(n^2)$
- 选择：不稳定，原地，$O(n^2)$
- 插入：稳定，原地，平均 $O(n^2)$，近乎有序时很强
- 希尔：不稳定，原地，依赖 gap 序列
- 归并：稳定，非原地，$O(n log n)$
- 快排：不稳定，通常原地，平均 $O(n log n)$，最坏 $O(n^2)$
- 堆排：不稳定，原地，$O(n log n)$
- 计数：可稳定，非原地，$O(n+k)$
- 桶排：依赖数据分布
- 基数：稳定，非原地，适合整数 / 定长串

---

## 3. 每种排序都给代码

## 3.1 冒泡排序（Bubble Sort）

- 思想：相邻交换，大的往后冒
- 稳定：是
- 复杂度：平均 / 最坏 $O(n^2)$

```cpp
void bubbleSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}
```

## 3.2 选择排序（Selection Sort）

- 思想：每轮选最小值放到前面
- 稳定：否
- 复杂度：$O(n^2)$

```cpp
void selectionSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n; ++i) {
        int mn = i;
        for (int j = i + 1; j < n; ++j) {
            if (a[j] < a[mn]) mn = j;
        }
        swap(a[i], a[mn]);
    }
}
```

## 3.3 插入排序（Insertion Sort）⭐

- 思想：维护前缀有序区，把当前值插进去
- 稳定：是
- 复杂度：平均 / 最坏 $O(n^2)$，最好 $O(n)$

```cpp
void insertionSort(vector<int>& a) {
    int n = a.size();
    for (int i = 1; i < n; ++i) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}
```

## 3.4 希尔排序（Shell Sort）

- 思想：按 gap 分组做插入排序
- 稳定：否

```cpp
void shellSort(vector<int>& a) {
    int n = a.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            int temp = a[i], j = i;
            while (j >= gap && a[j - gap] > temp) {
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}
```

## 3.5 归并排序（Merge Sort）⭐

- 思想：分治 + 合并两个有序区间
- 稳定：是
- 复杂度：$O(n log n)$

```cpp
void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int mid = (l + r) >> 1;
    mergeSort(a, l, mid);
    mergeSort(a, mid + 1, r);

    vector<int> tmp;
    int i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (a[i] <= a[j]) tmp.push_back(a[i++]);
        else tmp.push_back(a[j++]);
    }
    while (i <= mid) tmp.push_back(a[i++]);
    while (j <= r) tmp.push_back(a[j++]);

    for (int k = 0; k < (int)tmp.size(); ++k) {
        a[l + k] = tmp[k];
    }
}
```

## 3.6 快速排序（Quick Sort）⭐

- 思想：partition 后递归左右区间
- 稳定：否
- 复杂度：平均 $O(n log n)$，最坏 $O(n^2)$

```cpp
void quickSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int i = l, j = r;
    int pivot = a[(l + r) >> 1];
    while (i <= j) {
        while (a[i] < pivot) ++i;
        while (a[j] > pivot) --j;
        if (i <= j) swap(a[i++], a[j--]);
    }
    if (l < j) quickSort(a, l, j);
    if (i < r) quickSort(a, i, r);
}
```

## 3.7 堆排序（Heap Sort）⭐

- 思想：建大根堆，不断把堆顶放到末尾
- 稳定：否
- 复杂度：$O(n log n)$

```cpp
void heapify(vector<int>& a, int n, int i) {
    int largest = i;
    int l = i * 2 + 1, r = i * 2 + 2;
    if (l < n && a[l] > a[largest]) largest = l;
    if (r < n && a[r] > a[largest]) largest = r;
    if (largest != i) {
        swap(a[i], a[largest]);
        heapify(a, n, largest);
    }
}

void heapSort(vector<int>& a) {
    int n = a.size();
    for (int i = n / 2 - 1; i >= 0; --i) heapify(a, n, i);
    for (int i = n - 1; i > 0; --i) {
        swap(a[0], a[i]);
        heapify(a, i, 0);
    }
}
```

## 3.8 计数排序（Counting Sort）

- 思想：统计每个值出现次数
- 适用：值域小、整数
- 复杂度：$O(n+k)$

```cpp
void countingSort(vector<int>& a) {
    int mx = *max_element(a.begin(), a.end());
    vector<int> cnt(mx + 1, 0);
    for (int x : a) cnt[x]++;
    int idx = 0;
    for (int v = 0; v <= mx; ++v) {
        while (cnt[v]--) a[idx++] = v;
    }
}
```

## 3.9 桶排序（Bucket Sort）

- 思想：先分桶，再对桶内排序
- 适用：数据分布较均匀

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

## 3.10 基数排序（Radix Sort）

- 思想：按个位、十位、百位...稳定排序
- 适用：非负整数、位数有限

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

---

## 4. 最后只记这个选型

- 近乎有序、小规模：插入排序
- 稳定排序：归并排序
- 平均性能强：快速排序
- 最坏情况稳、原地：堆排序
- 值域小：计数排序
- 整数按位：基数排序
- 分布均匀：桶排序

---

## 5. 高频坑点

- 快排最坏会退化到 $O(n^2)$
- `nth_element` 不是排序，它只是选第 k 小
- 计数 / 基数 / 桶排序不是通用排序
- 稳定性在多关键字排序里很重要
- 归并排序需要额外空间