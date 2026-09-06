---
tags:
  - cs/algorithm
status: stable
---

# Sorting Algorithms — 排序

> [!important] **核心考点**：比较排序与非比较排序的边界、时间/空间/稳定性对比，以及按数据特征选型。

## 核心

- 排序 4 要素：
    - 时间复杂度
    - 空间复杂度
    - 稳定性（相同元素间的次序不变）
    - 原地性（不使用额外数组）
- 必会：
    - 归并排序
    - 快速排序
- 目标：
	- 先看数据规模、是否近乎有序、值域和稳定性要求，再决定是否值得手写排序。

## 排序算法对比

---

- 冒泡：$O(N^2)$  $O(1)$                          稳定，原地 简单易实现，适合教学 /小数据
- 选择：$O(N^2)$  $O(1)$                      不稳定，原地 交换次数少，适合操作昂贵的场景
- 插入：$O(N^2)$  $O(1)$                          稳定，原地    近乎有序时很强，适合小数据
- 归并：$O(N·log N)$  $O(N)$                稳定，非原地   时间稳定，适合逆序对拓展
- 快排：平均 $O(N·log N)$、最坏 $O(N^2)$；通常原地且不稳定。标准库一般以 introsort 等混合策略规避最坏情况
- 堆排：$O(N·log N)$  $O(1)$              不稳定，原地   适合空间敏感的场景
- 计数：$O(N+k)$  $O(k)$                     稳定，非原地   适合值域小的整数，线性时间
- 希尔：复杂度取决于 gap 序列；原地、不稳定
- 桶排：期望复杂度依赖近似均匀的输入分布及桶内排序策略
- 基数：$O(d·(N+k))$，其中 `d` 为位数、`k` 为每位取值范围；适合固定长度键

---

## 经典排序实现

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
    if (arr.empty()) return;
    int minVal = *min_element(arr.begin(), arr.end());
    int maxVal = *max_element(arr.begin(), arr.end());
    vector<int> cnt(maxVal - minVal + 1, 0);  // 仅适用于值域可承受
    for (int x : arr) ++cnt[x - minVal];
    int pos = 0;
    for (int offset = 0; offset < (int)cnt.size(); ++offset) {
        while (cnt[offset]-- > 0) arr[pos++] = offset + minVal;
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
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
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
    if (a.empty()) return;
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

> [!warning] 计数排序的空间随“值域”而不是元素个数增长；上例的基数排序只覆盖非负整数，`exp *= 10` 也要注意溢出。题目没有明确数据范围时，优先使用 `std::sort` / `std::stable_sort`，而不是把线性排序当作默认选项。

## 30 秒回答

**排序如何选？** 通用场景优先标准库排序：`std::sort` 适合通常的就地不稳定排序需求，需保持等值元素相对顺序时用 `std::stable_sort`。近乎有序、小区间常用插入排序思想；值域小的整数才考虑计数排序；固定长度键且每位范围有限才考虑基数排序。

**自测：** 为什么计数排序不能只看 `N`？快速排序为什么不能只写“`O(N log N)`”？

---

## 关联笔记

- [Quick Sort： Partition & Pivot (快排实现)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Merge Sort & Inversion Count (归并排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Heap Sort (堆排序)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Array & Two Pointers (数组与双指针)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/02-Algorithms%20&%20Data%20Structures%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)
