#algorithm #divide-and-conquer #recursion #merge-sort #quick-sort

## ⚡ TL;DR（快速决策）

- 分治 = **分解（Divide）→ 解决（Conquer）→ 合并（Combine）**
- 适合分治的问题，通常都有这几个特征：
    - 原问题可以拆成若干个**同类型子问题**
    - 子问题之间**相对独立**
    - 子问题的答案可以**组合回原问题**
- 分治常和**递归**一起出现，但“递归”是写法，“分治”是思路
- 经典题型要立刻联想到分治：**归并排序、快速排序、二分查找、快速幂、线段树建树 / 查询、逆序对统计**
- 如果题目核心是“把大问题拆小，分别解决后再拼起来”，优先考虑分治
- 如果子问题之间有大量重复，更可能是 **DP（动态规划）**，不一定是纯分治

## 🧩 Core Idea（核心本质）

- 本质：把一个难处理的大问题，拆成若干个规模更小、结构相同的小问题
- 分治的关键不是“拆”，而是：
    - **怎么拆才合理**
    - **子问题怎么解**
    - **结果怎么合并**
- 标准分治三步：
    1. **Divide**：把规模为 $n$ 的问题拆成若干个规模更小的问题
    2. **Conquer**：递归解决这些子问题
    3. **Combine**：把子问题答案合并成原问题答案
- 一句话理解：**先把大事拆小，各个击破，再把结果拼回来**
- 分治和递归关系：
    - 递归：函数自己调用自己
    - 分治：一种常用递归设计思想
    - 所以：**不是所有递归都是分治，但分治通常会写成递归**

## 🔧 Usage Patterns（可复用代码模板）

1. 通用分治模板

```cpp
int solve(int l, int r) {
    if (l == r) {
        return base_case_value;  // 递归边界
    }

    int mid = l + (r - l) / 2;

    int leftAns = solve(l, mid);
    int rightAns = solve(mid + 1, r);

    int ans = combine(leftAns, rightAns);
    return ans;
}
```

适用理解：

- `base case`：问题已经小到可以直接解
- `mid`：决定怎么拆
- `combine(...)`：决定分治是否成立的核心

1. 二分查找（最典型的“不断二分”）

```cpp
int binarySearch(const vector<int>& nums, int target) {
    int l = 0, r = (int)nums.size() - 1;

    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        if (nums[mid] < target) l = mid + 1;
        else r = mid - 1;
    }

    return -1;
}
```

要点：

- 每次把搜索区间砍成一半
- 时间复杂度：$O(log n)$
- 虽然常写成循环，但思想仍然是分治

1. 归并排序（分治经典模板）

```cpp
void merge(vector<int>& nums, int l, int mid, int r) {
    vector<int> tmp;
    int i = l, j = mid + 1;

    while (i <= mid && j <= r) {
        if (nums[i] <= nums[j]) tmp.push_back(nums[i++]);
        else tmp.push_back(nums[j++]);
    }

    while (i <= mid) tmp.push_back(nums[i++]);
    while (j <= r) tmp.push_back(nums[j++]);

    for (int k = 0; k < (int)tmp.size(); ++k) {
        nums[l + k] = tmp[k];
    }
}

void mergeSort(vector<int>& nums, int l, int r) {
    if (l >= r) return;

    int mid = l + (r - l) / 2;
    mergeSort(nums, l, mid);
    mergeSort(nums, mid + 1, r);
    merge(nums, l, mid, r);
}
```

要点：

- 拆：左右两半
- 解：分别排序
- 合：把两个有序段合并
- 时间复杂度：$O(n log n)$
- 稳定排序

1. 快速排序（分治 + 划分）

```cpp
int partition(vector<int>& nums, int l, int r) {
    int pivot = nums[r];
    int i = l;

    for (int j = l; j < r; ++j) {
        if (nums[j] < pivot) {
            swap(nums[i], nums[j]);
            ++i;
        }
    }

    swap(nums[i], nums[r]);
    return i;
}

void quickSort(vector<int>& nums, int l, int r) {
    if (l >= r) return;

    int p = partition(nums, l, r);
    quickSort(nums, l, p - 1);
    quickSort(nums, p + 1, r);
}
```

要点：

- 拆分依据不是中点，而是 `pivot`
- 左边都比 `pivot` 小，右边都比 `pivot` 大或不小
- 平均时间复杂度：$O(n log n)$
- 最坏可能退化到 $O(n^2)$
- 一般是不稳定排序

1. 快速幂（按指数分治）

```cpp
long long fastPow(long long a, long long n) {
    if (n == 0) return 1;

    long long half = fastPow(a, n / 2);
    if (n % 2 == 0) return half * half;
    return half * half * a;
}
```

要点：

- 把 $a^n$ 转化成更小规模问题
- 当 `n` 很大时比连乘快得多
- 时间复杂度：$O(log n)$

1. 逆序对统计（归并排序变形）

```cpp
long long mergeCount(vector<int>& nums, int l, int mid, int r) {
    vector<int> tmp;
    int i = l, j = mid + 1;
    long long cnt = 0;

    while (i <= mid && j <= r) {
        if (nums[i] <= nums[j]) {
            tmp.push_back(nums[i++]);
        } else {
            cnt += (mid - i + 1);  // 左边剩余元素都比 nums[j] 大
            tmp.push_back(nums[j++]);
        }
    }

    while (i <= mid) tmp.push_back(nums[i++]);
    while (j <= r) tmp.push_back(nums[j++]);

    for (int k = 0; k < (int)tmp.size(); ++k) {
        nums[l + k] = tmp[k];
    }

    return cnt;
}

long long countInversions(vector<int>& nums, int l, int r) {
    if (l >= r) return 0;

    int mid = l + (r - l) / 2;
    long long left = countInversions(nums, l, mid);
    long long right = countInversions(nums, mid + 1, r);
    long long cross = mergeCount(nums, l, mid, r);

    return left + right + cross;
}
```

要点：

- 不是只会“排序”，还能在合并阶段顺手统计信息
- 分治的威力常常就在 **combine 阶段顺便做事**

1. 最大子数组和（分治写法，了解）

```cpp
int maxCrossingSum(const vector<int>& nums, int l, int mid, int r) {
    int leftSum = INT_MIN, sum = 0;
    for (int i = mid; i >= l; --i) {
        sum += nums[i];
        leftSum = max(leftSum, sum);
    }

    int rightSum = INT_MIN;
    sum = 0;
    for (int i = mid + 1; i <= r; ++i) {
        sum += nums[i];
        rightSum = max(rightSum, sum);
    }

    return leftSum + rightSum;
}

int maxSubArray(vector<int>& nums, int l, int r) {
    if (l == r) return nums[l];

    int mid = l + (r - l) / 2;
    int leftAns = maxSubArray(nums, l, mid);
    int rightAns = maxSubArray(nums, mid + 1, r);
    int crossAns = maxCrossingSum(nums, l, mid, r);

    return max({leftAns, rightAns, crossAns});
}
```

要点：

- 跨越中点的答案需要单独处理
- 这是“合并逻辑比拆分更重要”的典型例子

## ⚠️ Pitfalls（高频错误）

- 不要把“递归”直接等同于“分治”
    - 递归只是实现方式
    - 分治强调的是：**拆分 + 子问题 + 合并**
- 分成子问题后，如果**没有有效合并方式**，那未必适合分治
- `mid = (l + r) / 2` 在极端情况下可能溢出，更稳妥写法是：
    - `int mid = l + (r - l) / 2;`
- 归并排序里最容易错的是：
    - 临时数组回写范围写错
    - `i / j` 边界判断写错
    - 少写了“剩余元素补进去”
- 快速排序最容易错的是：
    - `partition` 返回位置理解错
    - 递归区间写错成 `quickSort(nums, l, p)`
    - 在有序数组上选固定 `pivot` 导致退化
- 快速幂最容易错的是：
    - 忘记 `n == 0` 返回 1
    - 奇偶分支漏乘一次 `a`
- 分治不一定总更优：
    - 递归有函数调用开销
    - 某些题直接迭代更简单
- 子问题若大量重复，优先想 **记忆化 / DP**，不要硬套分治

## 🚀 Performance / Tips（性能优化）

- 分治复杂度分析常见形式：
    - $T(n) = aT(n / b) + f(n)$
- 常见理解：
    - `a`：拆成几个子问题
    - `n / b`：每个子问题规模
    - `f(n)`：拆分 / 合并本身的代价
- 高频记忆：
    - 二分查找：$O(log n)$
    - 归并排序：$O(n log n)$
    - 快速排序：平均 $O(n log n)$，最坏 $O(n^2)$
    - 快速幂：$O(log n)$
- 工程里写分治时要特别注意：
    - 递归深度是否会爆栈
    - 是否需要额外辅助数组（如归并排序）
    - 合并过程能否做到线性
- 想判断能不能用分治，可以先问自己 3 个问题：
    1. 能不能拆成更小的同类问题？
    2. 子问题能不能独立解决？
    3. 子问题结果能不能高效合并？

## 🧪 Common Scenarios（常见使用场景）

- **查找类**：二分查找、二分答案
- **排序类**：归并排序、快速排序
- **幂运算 / 乘法优化**：快速幂、矩阵快速幂
- **统计类**：逆序对、区间信息统计
- **树结构问题**：线段树、树的递归划分处理
- **经典面试理解题**：
    - 分治和递归什么关系？
    - 分治和动态规划有什么区别？
    - 为什么归并排序是分治、冒泡排序不是？

## 🆚 Divide and Conquer vs 其他常见思想

- **vs Recursion（递归）**
    - 递归：一种写函数的方法
    - 分治：一种解题思想
- **vs Dynamic Programming（动态规划）**
    - 分治：子问题通常相互独立
    - DP：子问题通常有重复，需要记忆化 / 状态转移
- **vs Greedy（贪心）**
    - 分治：先拆问题再合并答案
    - 贪心：每一步都做局部最优选择

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
#include <vector>
using namespace std;

void merge(vector<int>& a, int l, int mid, int r) {
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

void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;

    int mid = l + (r - l) / 2;
    mergeSort(a, l, mid);
    mergeSort(a, mid + 1, r);
    merge(a, l, mid, r);
}

int main() {
    vector<int> a = {5, 2, 8, 1, 3};
    mergeSort(a, 0, (int)a.size() - 1);

    for (int x : a) cout << x << ' ';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **分治就是：把大问题拆成若干个小的同类问题，分别解决后再把结果合并起来。**