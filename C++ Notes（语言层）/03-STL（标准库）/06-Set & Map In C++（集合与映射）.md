#cpp #stl #set-map

## ⚡ TL;DR（快速决策）

- 需要自动去重 + 有序存储 → `set`
- 需要键值映射 + 按键有序 → `map`
- 只关心“是否存在” → 优先想 `set`
- 需要“计数 / 频率统计” → 优先想 `map`
- 需要更快平均性能且不关心顺序 → 再考虑 `unordered_set` / `unordered_map`

## 🧩 Core Idea（核心本质）

- **本质**：`set` 管“唯一元素集合”，`map` 管“键到值的映射关系”
- **关键机制**：底层通常基于平衡树，自动维护有序性
- **核心优势**：插入、删除、查找统一，适合动态维护去重、映射和频率信息

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础 `set`

```cpp
set<int> s;
s.insert(3);
s.insert(1);
s.insert(3);
```

### 2. 最基础 `map`

```cpp
map<string, int> mp;
mp["Tom"] = 95;
mp["Jerry"] = 88;
```

### 3. 判断是否存在

```cpp
if (s.count(3)) {
	cout << "found" << '\\n';
}

if (mp.count("Tom")) {
	cout << "found" << '\\n';
}
```

### 4. 查找元素

```cpp
auto it = s.find(3);
if (it != s.end()) {
	cout << *it << '\\n';
}
```

### 5. 遍历 `set`

```cpp
for (int x : s) {
	cout << x << '\\n';
}
```

### 6. 遍历 `map`

```cpp
for (auto& [key, value] : mp) {
	cout << key << ' ' << value << '\\n';
}
```

### 7. 频率统计

```cpp
map<int, int> cnt;
for (int x : nums) {
	cnt[x]++;
}
```

### 8. 删除元素

```cpp
s.erase(3);
mp.erase("Tom");
```

### 9. 安全读取 `map`

```cpp
auto it = mp.find("Tom");
if (it != mp.end()) {
	cout << it->second << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- `set` 里元素唯一，重复插入不会报错，只是不会重复存
- `map[key]` 在 key 不存在时会自动创建默认值，这是高频坑
- 只想查询时直接用 `mp[key]`，可能意外修改容器状态
- `find()` 找不到时返回的是 `end()`，不是 `nullptr`
- `set` / `map` 是有序容器，但不是按插入顺序保存
- 误以为它们和 `unordered_*` 行为完全一样，顺序与复杂度都不同
- 删除元素后，相关迭代器可能失效，不能继续乱用
- 频率统计时忘记值默认从 `0` 开始，逻辑容易重复初始化

## 🚀 Performance / Tips（性能优化）

- 需要顺序时用 `set` / `map`，只求平均更快查找时考虑 `unordered_*`
- `count()` 适合做存在性判断，`find()` 适合拿到迭代器继续操作
- 频率统计、离散映射、去重，是 `set/map` 的高频强项
- 只读查询 `map` 时优先 `find()`，避免 `mp[key]` 的副作用
- 大多数刷题里，先把“要不要有序”这件事想清楚，再选 `set/map` 还是 `unordered_*`

## 🧪 Common Scenarios（常见使用场景）

- **数组去重**：`set`
- **判重 / 是否出现过**：`set`
- **频率统计**：`map`
- **字符串或编号映射**：`map`
- **按顺序输出去重结果**：`set`
- **按 key 顺序遍历统计结果**：`map`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	set<int> s;
	s.insert(3);
	s.insert(1);
	s.insert(3);

	map<int, int> cnt;
	for (int x : {1, 2, 2, 3}) {
		cnt[x]++;
	}

	for (int x : s) {
		cout << x << ' ';
	}
	cout << '\\n';

	for (auto& [key, value] : cnt) {
		cout << key << ':' << value << ' ';
	}
	cout << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `set` 和 `map` 的核心不是“会几个接口”，而是先判断你要解决的是 **去重、判重、计数，还是键值映射**，然后再决定是否需要“自动有序”这个特性。