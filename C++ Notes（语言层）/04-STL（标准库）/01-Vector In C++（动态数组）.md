#cpp #stl #vector

## ⚡ TL;DR（快速决策）

- 需要可变长度顺序容器 → `vector`
- 需要随机访问元素 → `vector[i]`
- 频繁尾插 → `push_back()` 很合适
- 预先知道大致容量 → `reserve()` 减少扩容开销
- 插入 / 删除中间元素很多 → `vector` 往往不是最优选择

## 🧩 Core Idea（核心本质）

- **本质**：`vector` 是连续内存上的可变长数组
- **关键机制**：元素连续存储，尾部扩容时可能整体搬迁到更大内存块
- **核心优势**：随机访问快，尾插高效，接口统一，和 STL 算法配合自然

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础定义

```cpp
vector<int> nums;
```

### 2. 初始化

```cpp
vector<int> a = {1, 2, 3};
vector<int> b(5);
vector<int> c(5, 7);
```

### 3. 尾部添加与访问

```cpp
nums.push_back(10);
nums.push_back(20);
cout << nums[0] << '\\n';
cout << nums.back() << '\\n';
```

### 4. 遍历

```cpp
for (int x : nums) {
	cout << x << '\\n';
}

for (int i = 0; i < nums.size(); ++i) {
	cout << nums[i] << '\\n';
}
```

### 5. 常用接口

```cpp
cout << nums.size() << '\\n';
cout << nums.empty() << '\\n';
nums.pop_back();
nums.clear();
```

### 6. 排序

```cpp
sort(nums.begin(), nums.end());
```

### 7. 预留容量

```cpp
vector<int> nums;
nums.reserve(1000);
```

### 8. 二维 `vector`

```cpp
vector<vector<int>> grid(3, vector<int>(4, 0));
```

## ⚠️ Pitfalls（高频错误）

- `nums[i]` 不做边界检查，越界就是未定义行为
- 扩容后旧指针、引用、迭代器可能失效
- `clear()` 清的是元素，不一定释放容量
- 误以为 `resize()` 和 `reserve()` 一样，它们语义完全不同
- 中间位置频繁插入 / 删除会导致大量元素搬移
- `pop_back()` 只能删最后一个元素，且空 `vector` 不能直接调
- `size()` 返回无符号类型，和负数比较时要小心
- 以为 `vector` 默认初始化后就一定有元素，空 `vector` 不能直接访问下标

## 🚀 Performance / Tips（性能优化）

- 预计数据量较大时优先 `reserve()`，减少重复扩容
- 频繁尾插是 `vector` 的强项，中间插删不是
- 需要高频头插 / 头删时，不要第一反应就用 `vector`
- 遍历时读多写少可以直接 range-based for
- 大对象放进 `vector` 时要关注拷贝 / 移动成本

## 🧪 Common Scenarios（常见使用场景）

- **刷题存输入数组**：`vector<int>`
- **动态维护一组元素**：尾插、排序、遍历
- **图的邻接表**：`vector<vector<int>>`
- **二维网格 / DP 表**：二维 `vector`
- **与 STL 算法联动**：`sort`、`reverse`、`lower_bound`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {3, 1, 2};
	nums.push_back(4);
	sort(nums.begin(), nums.end());

	for (int x : nums) {
		cout << x << ' ';
	}
	cout << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `vector` 的核心不是“能存很多元素”，而是它提供了 **连续内存 + 动态扩容 + 随机访问** 这组非常适合刷题和通用开发的能力。