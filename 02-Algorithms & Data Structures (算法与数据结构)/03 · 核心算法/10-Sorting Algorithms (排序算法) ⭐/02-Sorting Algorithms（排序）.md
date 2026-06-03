> **核心考点**：十大排序算法的时间/空间/稳定性对比、基于比较与基于计数的分类、排序选型决策
#algorithm #sorting #sort #comparison-sort #non-comparison-sort

# 核心

- 排序 4 要素：
    - 时间复杂度
    - 空间复杂度
    - 稳定性（相同元素间的次序不变）
    - 原地性（不使用额外数组）
- 必会：
    - 归并排序
    - 快速排序
- 目标：
	- 充分利用 $O(N·log N)$ 和 $O(N^2)$ 排序的各自优势
# 排序算法对比

---

- 冒泡：$O(N^2)$  $O(1)$                          稳定，原地 简单易实现，适合教学 /小数据
- 选择：$O(N^2)$  $O(1)$                      不稳定，原地 交换次数少，适合操作昂贵的场景
- 插入：$O(N^2)$  $O(1)$                          稳定，原地    近乎有序时很强，适合小数据
- 归并：$O(N·log N)$  $O(N)$                稳定，非原地   时间稳定，适合逆序对拓展
- 快排：$O(N·log N)$  $O(log N)$        不稳定，通常原地   工程最常用，平均性能最优
- 堆排：$O(N·log N)$  $O(1)$              不稳定，原地   适合空间敏感的场景
- 计数：$O(N+k)$  $O(k)$                     稳定，非原地   适合值域小的整数，线性时间
- 希尔：$O(N·log N)$  $O(1)$              不稳定，原地   依赖 gap 序列
- 桶排：$O(N)$  $O(N)$                       不稳定，非原地   依赖数据分布
- 基数：$O(N)$  $O(N)$                          稳定，非原地   适合整数 / 定长串

---

# 十大经典排序

### 2.1 冒泡排序（Bubble Sort）

```cpp
// 对外接口
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}
```

### 2.2 选择排序（Selection Sort）

```cpp
// 对外接口
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIdx]) minIdx = j;
        }
        swap(arr[i], arr[minIdx]);
    }
}
```

### 2.3 插入排序（Insertion Sort）⭐

```cpp
// 对外接口
void insertSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
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


### 2.5 快速排序（Quick Sort）⭐

### 2.6 堆排序（Heap Sort）⭐

### 2.7 计数排序（Counting Sort）

```cpp
// 对外接口
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


### 2.8 希尔排序（Shell Sort）

```cpp
// 对外接口
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

### 2.9 桶排序（Bucket Sort）

```cpp
// 对外接口
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

### 2.10 基数排序（Radix Sort）

```cpp
// 对外接口
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