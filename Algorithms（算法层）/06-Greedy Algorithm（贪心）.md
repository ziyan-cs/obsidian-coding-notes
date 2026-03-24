#algorithm #greedy #optimization

## ⚡ TL;DR（快速决策）

- 每一步都有明显“当前最优选择” → 先怀疑能不能贪心
- 题目目标是“最少 / 最多 / 尽量大 / 尽量小” → 贪心高频
- 排序后规律特别明显 → 很可能是贪心信号
- 贪心题真正关键通常不在代码，而在“为什么这样选不会后悔”
- 局部最优不一定推出全局最优，不会证明就别急着套贪心

## 🧩 Core Idea（核心本质）

- **本质**：贪心是在每一步都直接做当前最优选择，并且不回头修改
- **关键机制**：依赖某种局部最优策略，使得一路选择下去仍能得到全局最优或正确答案
- **核心价值**：一旦策略成立，代码通常短、复杂度低、实现直接

## 🔧 Usage Patterns（可复用代码模板）

### 1. 排序后扫描

```cpp
sort(a.begin(), a.end());
for (int i = 0; i < a.size(); ++i) {
	// 按贪心规则做选择
}
```

### 2. 区间调度（按结束时间排序）

```cpp
sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
	return a[1] < b[1];
});
int ans = 0;
int end = INT_MIN;
for (auto& seg : intervals) {
	if (seg[0] >= end) {
		++ans;
		end = seg[1];
	}
}
```

### 3. 分发资源

```cpp
sort(a.begin(), a.end());
sort(b.begin(), b.end());
int i = 0, j = 0;
while (i < a.size() && j < b.size()) {
	if (b[j] >= a[i]) {
		++i;
		++j;
	} else {
		++j;
	}
}
```

### 4. 最远可达先有印象

```cpp
int farthest = 0;
for (int i = 0; i <= farthest; ++i) {
	farthest = max(farthest, i + nums[i]);
}
```

### 5. 维护当前最优选择

```cpp
for (每个候选项) {
	if (当前更优) {
		更新答案或状态;
	}
}
```

### 6. 排序 + 自定义比较器

```cpp
sort(a.begin(), a.end(), [](const auto& x, const auto& y) {
	return x.second < y.second;
});
```

### 7. 剪掉不必要选择先有印象

```cpp
if (当前选择已经比已有答案差) {
	continue;
}
```

### 8. “先想规则再写代码”

```cpp
先确定局部最优规则
再证明它不会后悔
最后再实现
```

## ⚠️ Pitfalls（高频错误）

- 不是所有最优化问题都能贪心
- 代码短不代表题目简单，贪心真正难点常在策略证明
- 排序依据选错，整个贪心策略就会错
- 只凭“感觉这样最好”通常不够，必须能解释为什么不后悔
- 把贪心和 DP 混掉：一个是当前直接选，一个是综合多个状态再选
- 局部最优未必能推出全局最优
- 区间题里最常见错误是按错端点排序
- 贪心往往是“选法正确”，不是“枚举更少”这么简单

## 🚀 Performance / Tips（性能优化）

- 一看到“最少 / 最多 / 尽可能多地完成”要先警觉贪心信号
- 贪心真正高频的价值是：

```cpp
一次扫描
少量状态维护
避免复杂状态枚举
```

- 排序是很多贪心题的前置步骤，不排序往往看不出规则
- 写贪心前优先先问：

```cpp
当前最优到底是什么
为什么这样选不会后悔
```

- 如果策略解释不清，先不要急着落代码

## 🧪 Common Scenarios（常见使用场景）

- **区间问题**：最多不重叠区间、区间覆盖
- **资源分配**：饼干分发、任务安排
- **跳跃问题**：是否可达、最少跳跃次数
- **排序后扫描**：按某字段排序再决策
- **Top / 最优选择维护**：每步选当前最优对象
- **面试高频题**：贪心策略证明、和 DP 的区别

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<vector<int>> intervals = {{1, 3}, {2, 4}, {3, 5}};
	sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
		return a[1] < b[1];
	});

	int ans = 0;
	int end = INT_MIN;
	for (auto& seg : intervals) {
		if (seg[0] >= end) {
			++ans;
			end = seg[1];
		}
	}

	cout << ans << '\\n';
	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 贪心的核心不是“每次随便挑一个看起来好的”，而是先找到 **可证明不会后悔的局部最优策略**，再用它一路推进到全局答案。