#cpp #stl #iterator

## ⚡ TL;DR（快速决策）

- 需要统一遍历 STL 容器 → 用迭代器
- 需要和 STL 算法配合 → 传 `begin()` / `end()`
- 只想遍历容器元素 → `for (auto it = ... )`
- 只想拿值而不关心位置 → range-based for 往往更简洁
- 涉及删除、插入、失效问题时 → 必须先确认迭代器是否还有效

## 🧩 Core Idea（核心本质）

- **本质**：迭代器是访问容器元素位置的统一抽象
- **关键机制**：不同容器通过 `begin()` / `end()` 暴露可遍历区间，算法通过迭代器而不是直接通过容器工作
- **核心优势**：把“遍历、查找、区间操作”统一成一套接口，便于泛型编程和 STL 复用

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础迭代器

```cpp
vector<int> nums = {1, 2, 3};
auto it = nums.begin();
cout << *it << '\\n';
```

### 2. 遍历 `vector`

```cpp
for (auto it = nums.begin(); it != nums.end(); ++it) {
	cout << *it << '\\n';
}
```

### 3. 遍历 `set`

```cpp
set<int> s = {3, 1, 2};
for (auto it = s.begin(); it != s.end(); ++it) {
	cout << *it << '\\n';
}
```

### 4. 遍历 `map`

```cpp
map<string, int> mp;
mp["Tom"] = 95;

for (auto it = mp.begin(); it != mp.end(); ++it) {
	cout << it->first << ' ' << it->second << '\\n';
}
```

### 5. 配合 `find()`

```cpp
auto it = find(nums.begin(), nums.end(), 3);
if (it != nums.end()) {
	cout << *it << '\\n';
}
```

### 6. 配合排序和反转

```cpp
sort(nums.begin(), nums.end());
reverse(nums.begin(), nums.end());
```

### 7. 区间写法

```cpp
auto l = nums.begin();
auto r = nums.end();
```

### 8. 删除元素后接住返回迭代器

```cpp
auto it = nums.begin();
it = nums.erase(it);
```

## ⚠️ Pitfalls（高频错误）

- `end()` 不是最后一个元素，不能直接解引用
- `it` 是位置，`*it` 才是元素值
- `map` 迭代器访问键值对要用 `it->first` / `it->second`
- 删除元素后，原迭代器可能失效，不能继续直接使用
- 不是所有容器都支持下标访问，但大多数 STL 容器都支持迭代器
- 把迭代器和指针完全等同，会在不同容器上出理解偏差
- `find()` 找不到时返回的是对应容器的 `end()`
- `it++` 和 `++it` 都能用，但在迭代器语境里通常优先 `++it`

## 🚀 Performance / Tips（性能优化）

- 迭代器类型长时，优先用 `auto`
- STL 算法优先直接吃迭代器区间，不要手写重复逻辑
- 只读遍历时，优先考虑 `const auto&` 或常量迭代器语义
- 需要删除元素时，优先确认容器的失效规则
- “看到算法，先想到区间 `[begin(), end())`” 是很重要的思维习惯

## 🧪 Common Scenarios（常见使用场景）

- **遍历容器**：`vector` / `set` / `map`
- **配合 STL 算法**：`find`、`sort`、`reverse`
- **区间处理**：传一段范围给算法
- **删除后继续遍历**：接住返回的迭代器
- **泛型代码阅读**：理解模板和算法中的位置抽象

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	vector<int> nums = {3, 1, 2};
	for (auto it = nums.begin(); it != nums.end(); ++it) {
		cout << *it << ' ';
	}
	cout << '\\n';

	auto it = find(nums.begin(), nums.end(), 1);
	if (it != nums.end()) {
		cout << *it << '\\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 迭代器的核心不是“会几个写法”，而是理解它把 **容器元素位置** 抽象成统一接口，让遍历、查找和 STL 算法都能落在同一套区间模型上。