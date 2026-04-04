#algorithm #two-pointers #array #linked-list

## 核心

- 双指针本质是：**两个位置指针按规则移动，降低枚举复杂度**
- 适用关键词：
    - 有序数组
    - 子数组 / 子序列
    - 去重
    - 左右夹逼
    - 快慢指针

## 什么时候用

- 需要同时维护 “ 左边界 + 右边界 ”
- 链表判环、找中点、删除重复元素
- 问题本来是双重枚举，但可以通过移动规则减少重复计算

## 常见类型

### 1. 左右夹逼

- 常用于有序数组求两数之和、回文判断

```cpp
int l = 0, r = n - 1;
while (l < r) {
    int s = a[l] + a[r];
    if (s == target) break;
    if (s < target) ++l;
    else --r;
}
```

### 2. 快慢指针

- 常用于链表中点、判环、原地去重

```cpp
ListNode* slow = head;
ListNode* fast = head;
while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
}
```

### 3. 同向双指针

- 常用于去重、维护合法区间

```cpp
int j = 0;
for (int i = 0; i < n; ++i) {
    while (j < n && ok(i, j)) ++j;
    // 处理区间 [i, j)
}
```

## 高频题型模板

### 1. 有序数组两数之和

```cpp
vector<int> twoSumSorted(vector<int>& a, int target) {
    int l = 0, r = (int)a.size() - 1;
    while (l < r) {
        int s = a[l] + a[r];
        if (s == target) return {l, r};
        if (s < target) ++l;
        else --r;
    }
    return {};
}
```

### 2. 原地去重（有序数组）

```cpp
int removeDuplicates(vector<int>& a) {
    if (a.empty()) return 0;
    int slow = 1;
    for (int fast = 1; fast < (int)a.size(); ++fast) {
        if (a[fast] != a[fast - 1]) {
            a[slow++] = a[fast];
        }
    }
    return slow;
}
```

## 高频坑点

- 没先确认数组是否有序，就直接左右夹逼
- 区间是 `[l, r]` 还是 `[l, r)` 写乱
- 快慢指针判环时空指针判断不完整
- 去重时 slow / fast 含义不清楚