#cpp #stl #algorithm

## ⚡ TL;DR（快速决策）

- 需要排序 → `sort`
- 需要反转 / 去重 / 查找区间 → 优先先想 `<algorithm>`
- 需要二分边界 → `lower_bound` / `upper_bound`
- 需要最值、计数、排列操作 → 先查标准算法，不要手写重复逻辑
- 看到“区间 `[begin(), end())`” → 基本就是算法库的工作方式

## 🧩 Core Idea（核心本质）

- **本质**：算法库是 STL 提供的一组通用算法，直接作用在迭代器区间上
- **关键机制**：算法不依赖具体容器，而是依赖区间和比较规则工作
- **核心优势**：复用成熟实现，代码更短、更稳、更接近标准写法

## 🔧 Usage Patterns（可复用代码模板）

### 1. 排序

```cpp
sort(nums.begin(), nums.end());
```

### 2. 降序排序

```cpp
sort(nums.begin(), nums.end(), greater<int>());
```

### 3. 反转

```cpp
reverse(nums.begin(), nums.end());
```

### 4. 查找元素

```cpp
auto it = find(nums.begin(), nums.end(), 3);
if (it != nums.end()) {
	cout << *it << '\\n';
}
```

### 5. 二分查找存在性

```cpp
bool ok = binary_search(nums.begin(), nums.end(), 5);
```

### 6. 找左边界 / 右边界

```cpp
auto it1 = lower_bound(nums.begin(), nums.end(), x);
auto it2 = upper_bound(nums.begin(), nums.end(), x);
```

### 7. 去重（排序后）

```cpp
sort(nums.begin(), nums.end());
nums.erase(unique(nums.begin(), nums.end()), nums.end());
```

### 8. 最值

```cpp
int mn = *min_element(nums.begin(), nums.end());
int mx = *max_element(nums.begin(), nums.end());
```

### 9. 计数

```cpp
int cnt = count(nums.begin(), nums.end(), 2);
```

### 10. 全排列

```cpp
sort(nums.begin(), nums.end());
do {
	// use nums
} while (next_permutation(nums.begin(), nums.end()));
```

## ⚠️ Pitfalls（高频错误）

- 大多数算法操作的是区间，不是整个容器对象本身
- `binary_search`、`lower_bound`、`upper_bound` 通常要求区间有序
- `unique()` 只是把“重复元素挪到后面”，真正去重要配合 `erase`
- `find()` 找不到返回的是 `end()`，不是 `-1`
- `sort` 默认升序，想降序要显式传比较器
- 比较器写错会导致排序或二分行为异常
- `lower_bound` 找的是“第一个大于等于”，`upper_bound` 找的是“第一个大于”
- 对空区间解引用 `min_element` / `max_element` 的结果会出问题

## 🚀 Performance / Tips（性能优化）

- 优先用标准算法，通常比手写更稳、更短、更不容易出 bug
- 看到“排序、查找、统计、去重、排列”先想 `<algorithm>`
- 写算法库代码时优先统一成迭代器区间思维
- 二分相关算法前先确认“区间已排序”
- 配合 `auto` 使用，代码可读性通常更好

## 🧪 Common Scenarios（常见使用场景）

- **数组排序**：`sort`
- **去重**：`sort + unique + erase`
- **查找元素**：`find`
- **边界定位**：`lower_bound` / `upper_bound`
- **最值统计**：`min_element` / `max_element` / `count`
- **枚举排列**：`next_permutation`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {3, 1, 2, 2, 4};
	sort(nums.begin(), nums.end());
	nums.erase(unique(nums.begin(), nums.end()), nums.end());

	auto it = lower_bound(nums.begin(), nums.end(), 3);
	if (it != nums.end()) {
		cout << *it << '\\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 算法库的核心不是记住一长串函数名，而是先把问题翻译成 **一个迭代器区间上的排序、查找、统计或变换操作**。
