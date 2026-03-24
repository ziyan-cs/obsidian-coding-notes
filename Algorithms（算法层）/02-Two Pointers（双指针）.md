#algorithm #two-pointers #sliding-window

## ⚡ TL;DR（快速决策）

- 题目涉及连续区间、左右收缩、相向移动 → 先想双指针
- 有序数组里找两数关系、去重、合并 → 双指针高频
- 链表中点、找环、倒数第 k 个节点 → 快慢指针
- 连续子串 / 子数组最值问题 → 往往是滑动窗口，本质也是双指针
- 双指针题真正关键通常不在“两个指针怎么写”，而在“为什么这边可以移动且不漏答案”

## 🧩 Core Idea（核心本质）

- **本质**：双指针是用两个位置协同移动来减少重复枚举的一类技巧
- **关键机制**：利用有序性、区间性质或速度差，让两个指针只朝特定方向推进
- **核心价值**：常把暴力的 `O(n^2)` 区间 / 配对问题优化到 `O(n)` 或 `O(n log n)`

## 🔧 Usage Patterns（可复用代码模板）

### 1. 左右夹逼

```cpp
int l = 0, r = n - 1;
while (l < r) {
	int sum = a[l] + a[r];
	if (sum == target) break;
	if (sum < target) ++l;
	else --r;
}
```

### 2. 有序数组两数之和

```cpp
sort(a.begin(), a.end());
int l = 0, r = a.size() - 1;
while (l < r) {
	int sum = a[l] + a[r];
	if (sum == target) {
		cout << l << ' ' << r << '\\n';
		break;
	}
	if (sum < target) ++l;
	else --r;
}
```

### 3. 快慢指针找中点

```cpp
ListNode* slow = head;
ListNode* fast = head;
while (fast != nullptr && fast->next != nullptr) {
	slow = slow->next;
	fast = fast->next->next;
}
```

### 4. 快慢指针判断有环

```cpp
ListNode* slow = head;
ListNode* fast = head;
while (fast != nullptr && fast->next != nullptr) {
	slow = slow->next;
	fast = fast->next->next;
	if (slow == fast) return true;
}
return false;
```

### 5. 滑动窗口基础模型

```cpp
int l = 0;
for (int r = 0; r < n; ++r) {
	while (窗口不合法) {
		++l;
	}
	// 更新答案
}
```

### 6. 去重基础模型

```cpp
sort(nums.begin(), nums.end());
int slow = 0;
for (int fast = 1; fast < nums.size(); ++fast) {
	if (nums[fast] != nums[slow]) {
		nums[++slow] = nums[fast];
	}
}
```

### 7. 合并两个有序数组先有印象

```cpp
int i = 0, j = 0;
while (i < a.size() && j < b.size()) {
	if (a[i] < b[j]) ++i;
	else ++j;
}
```

### 8. 回文判断

```cpp
int l = 0, r = s.size() - 1;
while (l < r) {
	if (s[l] != s[r]) return false;
	++l;
	--r;
}
return true;
```

## ⚠️ Pitfalls（高频错误）

- 双指针不是固定模板，关键是“指针各自表示什么”
- 移动左边还是右边必须有依据，不能凭感觉改
- 快慢指针循环条件最容易写错，尤其是 `fast` 判空
- 滑动窗口和左右夹逼虽然都叫双指针，但问题模型不同，不要混写
- 排序前提没确认就直接上左右指针，常会导致错误结论
- 去重题里 `slow` / `fast` 的含义不清，最容易覆盖错位置
- 更新答案的时机常常比指针移动本身更容易写错
- 很多题不是“能用两个指针”就叫双指针，真正关键是它是否减少了重复计算

## 🚀 Performance / Tips（性能优化）

- 一看到“连续区间”先想滑动窗口，一看到“有序配对”先想左右指针
- 双指针真正的优化来源通常是：

```cpp
有序性
单调性
区间可维护性
```

- 指针大多只前进不回退，所以很多双指针题是线性复杂度
- 写代码前，先用一句话定义：

```cpp
l 表示什么
r 表示什么
```

- 先确认题目需不需要排序，再决定是否能做左右夹逼

## 🧪 Common Scenarios（常见使用场景）

- **有序数组配对**：两数之和、最接近目标
- **回文串判断**：左右夹逼
- **数组去重**：快慢指针
- **链表问题**：中点、找环、倒数第 k 个节点
- **连续子串 / 子数组**：滑动窗口
- **合并 / 交集 / 比较两个序列**：双序列双指针

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {1, 2, 4, 7, 11, 15};
	int target = 15;
	int l = 0, r = nums.size() - 1;

	while (l < r) {
		int sum = nums[l] + nums[r];
		if (sum == target) {
			cout << l << ' ' << r << '\\n';
			break;
		}
		if (sum < target) ++l;
		else --r;
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 双指针的核心不是“放两个变量一起动”，而是利用 **顺序性、区间性质或速度差**，让问题从重复枚举转成有方向的线性推进。