---
status: stable
review_due: 2026-09-13
confidence: high
verified: 2026-09-06
---

# 08-Sorting Algorithms (排序算法)

> [!abstract] 学习定位：本专题把同类题型、数据结构与模板统一放在一个学习单元中，重点是识别模式、维护不变量与分析复杂度。

## Sorting Overview (排序总览)

> [!note] 本节重点：核心考点：比较排序与非比较排序的边界、时间/空间/稳定性对比，以及按数据特征选型。

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

- [Quick Sort： Partition & Pivot (快排实现)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Merge Sort & Inversion Count (归并排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Heap Sort (堆排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

## Quick Sort (快速排序)

> [!note] 本节重点：核心考点：快排的 partition 函数实现（Lomuto/Hoare）、pivot 选择策略、递归与迭代栈实现

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

## 关联笔记 · 延伸要点 2
- [Merge Sort & Inversion Count (归并排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Heap Sort (堆排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Sorting Algorithms (排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

## Merge Sort and Inversion Count (归并排序与逆序对)

> [!note] 本节重点：核心考点：归并排序的分治思想、merge 操作、逆序对计数、归并排序的稳定性

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

## 关联笔记 · 延伸要点 3
- [Quick Sort： Partition & Pivot (快排实现)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Heap Sort (堆排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10c-Heap%20Sort%20(堆排序).md)
- [Sorting Algorithms (排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)

---

## Heap Sort (堆排序)

> [!note] 本节重点：核心考点：堆排序的下滤建堆 O(n)、堆顶与末尾交换、不稳定排序特性

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

## 关联笔记 · 延伸要点 4
- [Quick Sort： Partition & Pivot (快排实现)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10a-Quick%20Sort：%20Partition%20&%20Pivot%20(快排实现).md)
- [Merge Sort & Inversion Count (归并排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10b-Merge%20Sort%20&%20Inversion%20Count%20(归并排序).md)
- [Sorting Algorithms (排序)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/03-Core%20Algorithms%20(核心算法)/10-Sorting%20Algorithms%20(排序算法)%20⭐/10z-Sorting%20Algorithms%20(排序).md)
- [Array & Two Pointers (数组与双指针)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/01-Array%20&%20Two%20Pointers%20(数组与双指针)%20⭐.md)
- [Reversal, Cycle Detection, Merge (反转⧸判环⧸合并)](/01-Foundations%20(基础能力)/02-Algorithms%20(算法与数据结构)/01-Fundamental%20Data%20Structures%20(基础数据结构)/02-Linked%20List%20(链表)%20⭐/02a-Reversal,%20Cycle%20Detection,%20Merge%20(反转⧸判环⧸合并).md)



## 零基础阅读路径

先从一个可手算的小输入读起，找出每一步不变的事实；再看代码模板；最后才背复杂度与题型变体。若代码看不懂，先画状态变化，不要直接记循环。

## 常见误区

- 把 **08-Sorting Algorithms (排序算法)** 只当作定义或模板背诵，遇到输入规模、边界条件或复杂度变化就不会选方案。 - 只在纸上推导而不写最小样例、反例和复杂度检查，容易把“会看”误当成会用。


## 学习闭环

### 从零复述

- 不看正文，用“问题 → 机制 → 边界”三句话讲清 **08-Sorting Algorithms (排序算法)**。

### 最小验证

- 写一个最小代码、命令、测试或项目观察，亲自验证本页的一条关键结论。

### 自测

1. 它解决的工程问题是什么？
2. 核心机制在哪个环节生效？
3. 什么时候应当换用另一种方案？
