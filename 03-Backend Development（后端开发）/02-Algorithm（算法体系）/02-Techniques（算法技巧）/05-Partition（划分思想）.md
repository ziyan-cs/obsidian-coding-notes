
## 核心

- **原地分区、时间复杂度O(n)**
- 关键词：
    - 快速排序
    - 单调性
    - 找边界
    - 二分答案

## 两路划分（基础）  
### 1. Lomuto 分区

```cpp
int lomutoPartition(vector<int>& arr, int l, int r) {
    int pivot = arr[r];
    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (arr[j] < pivot) {
            swap(arr[++i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[r]);
    return i + 1;
}
```
### 2. Hoare 分区

```cpp
int hoarePartition(vector<int>& arr, int l, int r) {
    int pivot = arr[l + ((r - l) >> 1)]; // 中间值做基准，避免最坏情况
    int i = l, j = r;
    while (i <= j) {
        while (arr[i] < pivot) ++i;
        while (arr[j] > pivot) --j;
        if (i <= j) swap(arr[i++], arr[j--]);
    }
    return j; // 返回分区边界
}
```


## 三路划分（重点）  

### 1. 荷兰国旗问题

```cpp
void dutchFlagSort(vector<int>& arr, int key) {
    int i = 0;
    int p1 = -1;
    int p2 = arr.size();
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
}
```

### 2. 三指针

```cpp
pair<int, int> netherlandsFlag(vector<int>& arr, int l, int r, int pivot) {
    int less = l - 1;
    int more = r + 1;
    int cur = l;
    while (cur < more) {
        if (arr[cur] < pivot) {
            swap(arr[cur++], arr[++less]);
        } else if (arr[cur] > pivot) {
            swap(arr[cur], arr[--more]);
        } else {
            cur++;
        }
    }
    return {less + 1, more - 1}; // 返回等于区域的左右边界
}
```

### 3. 三路快排

```cpp
void quickSortV2(vector<int>& arr, int L, int R) {
    if (L >= R) return;
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