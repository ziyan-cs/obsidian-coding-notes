#algorithm #binary-search #search

## ⚡ TL;DR（快速决策）

- 有序数组里查目标值 / 查边界 → 先想二分
- 题目不一定给有序数组，但只要答案具有单调性 → 也可能做二分
- 找“第一个满足”“最后一个满足” → 往往是边界二分
- 不要一上来背模板，先确认你是在“找值”还是“找边界”
- 二分题真正关键通常不在代码长度，而在区间定义、判定条件和收缩方向

## 🧩 Core Idea（核心本质）

- **本质**：二分查找是通过每次排除一半不可能区间来缩小搜索范围
- **关键机制**：依赖有序性或单调性，让一次比较就能确定答案只可能在某一侧
- **核心价值**：常把暴力的 `O(n)` 查找压到 `O(log n)`

## 🔧 Usage Patterns（可复用代码模板）

### 1. 找某个值

```cpp
int l = 0, r = n - 1;
while (l <= r) {
	int mid = l + (r - l) / 2;
	if (a[mid] == target) return mid;
	if (a[mid] < target) l = mid + 1;
	else r = mid - 1;
}
return -1;
```

### 2. 找左边界（第一个大于等于 target）

```cpp
int l = 0, r = n - 1;
while (l <= r) {
	int mid = l + (r - l) / 2;
	if (a[mid] >= target) r = mid - 1;
	else l = mid + 1;
}
```

### 3. 找右边界（最后一个小于等于 target）

```cpp
int l = 0, r = n - 1;
while (l <= r) {
	int mid = l + (r - l) / 2;
	if (a[mid] <= target) l = mid + 1;
	else r = mid - 1;
}
```

### 4. `lower_bound` / `upper_bound`

```cpp
auto it1 = lower_bound(a.begin(), a.end(), target);
auto it2 = upper_bound(a.begin(), a.end(), target);
```

### 5. 答案二分

```cpp
int l = min_ans, r = max_ans;
while (l <= r) {
	int mid = l + (r - l) / 2;
	if (check(mid)) r = mid - 1;
	else l = mid + 1;
}
```

### 6. 浮点数二分先有印象

```cpp
double l = 0, r = 1e6;
for (int i = 0; i < 100; ++i) {
	double mid = (l + r) / 2;
	if (check(mid)) r = mid;
	else l = mid;
}
```

### 7. 闭区间写法骨架

```cpp
while (l <= r) {
	int mid = l + (r - l) / 2;
	// 根据 mid 决定保留哪一边
}
```

### 8. 二分前先确认条件

```cpp
有序性
或
答案单调性
```

## ⚠️ Pitfalls（高频错误）

- 二分最常错的不是语法，而是“区间定义”和“边界收缩”配不起来
- `mid` 优先写 `l + (r - l) / 2`，不要默认写 `(l + r) / 2`
- 找值和找边界不是同一套停止逻辑，不能机械混写
- 二分前没确认有序性 / 单调性，整题方向可能就错了
- 循环结束后，`l` 和 `r` 的含义要重新判断，别直接乱返回
- `lower_bound` / `upper_bound` 的语义必须分清，不要只凭记忆硬写
- 答案二分里真正难的是 `check()`，不是二分框架本身
- 浮点二分不能照搬整数二分的终止条件

## 🚀 Performance / Tips（性能优化）

- 一看到“有序”“边界”“最小可行”“最大不可行”就要警觉二分信号
- 写二分前优先先想清：

```cpp
当前区间怎么定义
mid 表示什么
满足条件时该保留哪边
```

- 边界问题优先用语义来记，不要死背代码
- 能直接用 `lower_bound` / `upper_bound` 时，不一定要手写
- 二分真正高频的价值是：

```cpp
查值
查边界
查答案
```

## 🧪 Common Scenarios（常见使用场景）

- **有序数组查值**：判断是否存在、找下标
- **边界定位**：第一个大于等于、最后一个小于等于
- **重复元素区间**：值出现的左右边界
- **答案二分**：最小可行值、最大可行值
- **数学 / 几何近似**：浮点数二分
- **标准库搜索**：`lower_bound` / `upper_bound`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> a = {1, 3, 5, 7, 9};
	int target = 5;
	int l = 0, r = a.size() - 1;

	while (l <= r) {
		int mid = l + (r - l) / 2;
		if (a[mid] == target) {
			cout << mid << '\\n';
			break;
		}
		if (a[mid] < target) l = mid + 1;
		else r = mid - 1;
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 二分查找的核心不是“每次取中点”，而是利用 **有序性或单调性**，让一次判断就能安全排除一半搜索空间。