
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
void quickSort(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    // 随机选基准，避免最坏情况
    srand(time(0));
    int pivotIdx = l + rand() % (r - l + 1);
    swap(arr[pivotIdx], arr[r]);
    int pivot = arr[r];
    
    // 三路划分
    auto [eqL, eqR] = netherlandsFlag(arr, l, r, pivot);
    
    // 递归排序左右区域，等于区域无需排序
    quickSort(arr, l, eqL - 1);
    quickSort(arr, eqR + 1, r);
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
  void quickSort(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    // 随机选基准，避免最坏情况
    srand(time(0));
    int pivotIdx = l + rand() % (r - l + 1);
    swap(arr[pivotIdx], arr[r]);
    int pivot = arr[r];
    
    // 三路划分
    auto [eqL, eqR] = netherlandsFlag(arr, l, r, pivot);
    
    // 递归排序左右区域，等于区域无需排序
    quickSort(arr, l, eqL - 1);
    quickSort(arr, eqR + 1, r);
}
```

## 模板代码  
- 两路 partition  
- 三路 partition  
  
## 应用  
- 快速排序  
- 快速选择（第K大）