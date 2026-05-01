



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
