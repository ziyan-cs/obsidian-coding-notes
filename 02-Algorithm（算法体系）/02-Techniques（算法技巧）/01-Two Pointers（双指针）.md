#algorithm #two-pointers #array #linked-list

## 核心

- **两个位置指针按规则移动，降低枚举复杂度**
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

## 常见题型

### 1. 左右夹逼

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