> **核心考点**：边界条件、三种模板的使用场景、二分答案

## 二分的本质

在**有序**（或具有单调性）的搜索空间中，每次排除一半，将 O(n) 降到 O(log n)。

---

## 三种模板

### 模板一：精确查找（找到即返回）

python

```python
def binary_search(nums, target):
    l, r = 0, len(nums) - 1
    while l <= r:
        mid = l + (r - l) // 2    # 防止溢出（C++ 中重要）
        if   nums[mid] == target: return mid
        elif nums[mid] < target:  l = mid + 1
        else:                     r = mid - 1
    return -1
```

循环条件 `l <= r`，退出时 `l > r`，搜索空间为空时停止。

---

### 模板二：查找左边界（第一个 ≥ target 的位置）

python

```python
def lower_bound(nums, target):
    l, r = 0, len(nums)           # r = len(nums)，搜索空间是 [l, r)
    while l < r:
        mid = l + (r - l) // 2
        if nums[mid] < target:    l = mid + 1
        else:                     r = mid      # 保留 mid，继续向左收缩
    return l    # 返回第一个 >= target 的下标；若所有数都 < target，返回 len(nums)
```

### 模板三：查找右边界（最后一个 ≤ target 的位置）

python

```python
def upper_bound(nums, target):
    l, r = 0, len(nums)
    while l < r:
        mid = l + (r - l) // 2
        if nums[mid] <= target:   l = mid + 1  # target 可以在 mid 右边
        else:                     r = mid
    return l - 1    # l-1 是最后一个 <= target 的下标
```

**Python 内置：**

python

```python
import bisect
bisect.bisect_left(nums, target)   # 等价于 lower_bound（第一个 >= target）
bisect.bisect_right(nums, target)  # 第一个 > target 的位置
```

---

## 二分答案（最重要的应用）

**适用场景：** 答案在某个范围内，且答案越大（或越小）越容易满足条件（单调性），可以二分答案，把"求最值"转化为"验证是否可行"。

**套路：**

1. 确定答案范围 `[lo, hi]`
2. 定义 `check(mid)` 判断 mid 是否可行
3. 二分找到满足/不满足条件的边界

python

```python
# 模板
lo, hi = 最小可能答案, 最大可能答案
while lo < hi:
    mid = (lo + hi) // 2
    if check(mid):
        hi = mid        # mid 可行，尝试更小（求最小值）
    else:
        lo = mid + 1    # mid 不可行，需要更大
return lo
```

### 经典例题

**木材切割（二分最大长度）：**

python

```python
# n 根木材，各长 lengths[i]，至少切出 k 段，求最大段长
def max_piece(lengths, k):
    def check(mid):
        return sum(l // mid for l in lengths) >= k

    lo, hi = 1, max(lengths)
    while lo < hi:
        mid = (lo + hi + 1) // 2    # 求最大值时 mid 上取整，防止死循环
        if check(mid): lo = mid
        else:          hi = mid - 1
    return lo
```

**爱吃香蕉的珂珂（最小速度）：**

python

```python
def min_eating_speed(piles, h):
    def check(speed):
        return sum((p + speed - 1) // speed for p in piles) <= h

    lo, hi = 1, max(piles)
    while lo < hi:
        mid = (lo + hi) // 2
        if check(mid): hi = mid    # 速度可以更小
        else:          lo = mid + 1
    return lo
```

**在 D 天内送达包裹的能力（最小运载能力）：**

python

```python
def ship_within_days(weights, days):
    def check(cap):
        d, cur = 1, 0
        for w in weights:
            if cur + w > cap: d += 1; cur = 0
            cur += w
        return d <= days

    lo, hi = max(weights), sum(weights)
    while lo < hi:
        mid = (lo + hi) // 2
        if check(mid): hi = mid
        else:          lo = mid + 1
    return lo
```

---

## 旋转数组中的二分

python

```python
# 搜索旋转排序数组（无重复元素）
def search_rotated(nums, target):
    l, r = 0, len(nums) - 1
    while l <= r:
        mid = (l + r) // 2
        if nums[mid] == target: return mid
        if nums[l] <= nums[mid]:         # 左半段有序
            if nums[l] <= target < nums[mid]: r = mid - 1
            else:                             l = mid + 1
        else:                            # 右半段有序
            if nums[mid] < target <= nums[r]: l = mid + 1
            else:                             r = mid - 1
    return -1
```

---

## 二分常见陷阱

|陷阱|说明|解决|
|---|---|---|
|死循环|求最大值时 `mid=(lo+hi)//2`，`lo=mid` 可能不动|改为 `mid=(lo+hi+1)//2`|
|溢出（C++）|`(l+r)/2` 在 l、r 很大时溢出|用 `l + (r-l)/2`|
|边界判断|`<` 还是 `<=`，`+1` 还是不加|根据搜索空间开闭区间决定|