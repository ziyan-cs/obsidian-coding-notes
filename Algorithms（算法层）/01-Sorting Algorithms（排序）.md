#algorithm #sorting #comparison-sort

## ⚡ TL;DR（快速决策）

- 数据规模不大、直接调用标准库 → 优先 `sort`
- 需要稳定排序 → 先确认是否必须稳定，再考虑 `stable_sort` 或特定稳定算法
- 需要 Top K / 第 k 小，不一定要全排序 → 先想堆、快速选择、二分答案
- 题目核心是“排完后更容易处理” → 排序通常是预处理，而不是最终目标
- 排序题真正关键通常不在“背算法流程”，而在“什么时候该排序、排什么字段、排序后怎么利用顺序性”

## 🧩 Core Idea（核心本质）

- **本质**：排序是在重排数据，使其满足某种全局顺序关系
- **关键机制**：一旦数据有序，很多问题会从暴力枚举转成双指针、二分、贪心或线性扫描
- **核心价值**：排序往往不是终点，而是把问题转化成“更容易处理的结构”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 标准升序排序

```cpp
sort(nums.begin(), nums.end());
```

### 2. 降序排序

```cpp
sort(nums.begin(), nums.end(), greater<int>());
```

### 3. 自定义排序规则

```cpp
sort(a.begin(), a.end(), [](const auto& x, const auto& y) {
	return x.second < y.second;
});
```

### 4. 稳定排序

```cpp
stable_sort(a.begin(), a.end(), [](const auto& x, const auto& y) {
	return x.first < y.first;
});
```

### 5. 排序后去重

```cpp
sort(nums.begin(), nums.end());
nums.erase(unique(nums.begin(), nums.end()), nums.end());
```

### 6. 排序后双指针

```cpp
sort(nums.begin(), nums.end());
int l = 0, r = nums.size() - 1;
while (l < r) {
	int sum = nums[l] + nums[r];
	if (sum == target) break;
	if (sum < target) ++l;
	else --r;
}
```

### 7. 结构体排序先有印象

```cpp
struct Node {
	int x;
	int y;
};

sort(a.begin(), a.end(), [](const Node& p, const Node& q) {
	if (p.x != q.x) return p.x < q.x;
	return p.y < q.y;
});
```

### 8. 常见排序算法骨架先有印象

```cpp
// 冒泡：相邻交换
// 选择：每轮选最小
// 插入：把当前元素插到前面有序区
// 归并：分治 + 合并
// 快排：分治 + 划分
// 堆排：反复取堆顶
```

## ⚠️ Pitfalls（高频错误）

- 排序后原下标通常会丢失，题目需要原位置时要先保留索引
- 自定义比较器必须满足严格弱序，否则排序结果可能异常
- 以为所有排序都稳定，其实 `sort` 默认不稳定
- 去重时只写 `unique()` 不配合 `erase()`，容器大小不会真的变
- 排序对象是 `pair` / 结构体时，别忽略次关键字
- 排序后再做二分、双指针前，要先确认排序方向和后续逻辑一致
- 很多题并不需要“完全排序”，全排可能是过度计算
- 把“排序算法题”和“用排序解决问题”混为一谈，会导致复习重点失焦

## 🚀 Performance / Tips（性能优化）

- 刷题默认优先想标准库 `sort`，不要先手写排序算法
- 只有在题目明确考察排序原理时，才把重心放到冒泡、选择、插入、归并、快排、堆排本身
- 需要稳定性时先确认是否必须保留相等元素原顺序
- 需要部分最优结果时，优先考虑堆 / `nth_element` / 二分，不一定全排
- 排序最常见的价值是制造：

```cpp
有序性
可二分性
可双指针性
可线性扫描性
```

## 🧪 Common Scenarios（常见使用场景）

- **数组排序**：升序 / 降序输出
- **去重**：排序后去重
- **区间题 / 贪心题**：按左端点或右端点排序
- **双指针题**：排序后夹逼
- **二分题**：排序后查边界
- **结构体 / pair 排序**：多关键字排序
- **面试基础题**：排序稳定性、时间复杂度、快排 / 归并 / 堆排特点

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {5, 2, 4, 1, 3};
	sort(nums.begin(), nums.end());

	for (int x : nums) {
		cout << x << ' ';
	}
	cout << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 排序的核心不是“把数据排好看”，而是先通过 **建立顺序** 把原问题转化成更容易做二分、双指针、贪心或线性扫描的问题。