#cpp #STL #容器 #set #map #集合 #映射
## 一页速览

- **`set`**：自动去重、自动有序的集合
- **`map`**：键值对映射，按键自动有序
- **核心优势**：查重、计数、映射关系处理很方便
- **常见操作**：插入、删除、查找、遍历、统计出现次数
- **高频接口**：`insert`、`erase`、`find`、`count`、`size`、`empty`
- **高频场景**：去重、频率统计、字典映射、排序后查找替代
- **最常错**：`set` 和 `map` 作用混淆、误以为下标访问适用于所有容器、`find` 返回值不会判断、`map[key]` 的副作用不清楚

## 一、先总后分：这一节到底学什么

### 1. `set` 和 `map` 解决什么问题

- 有些题目需要快速判断一个值是否出现过
- 有些题目需要统计每个值出现多少次
- 有些题目需要建立“键 -> 值”的对应关系
- 这时 `set` 和 `map` 会比数组、`vector` 更自然

### 2. `set` 和 `map` 最核心看什么

- `set` 是干什么的
- `map` 是干什么的
- 自动有序和自动去重怎么理解
- 什么题目该想到它们

### 3. 当前阶段先抓主线

- 会定义 `set` 和 `map`
- 会写最常见的插入、查找、删除
- 会用 `map` 做简单计数
- 会分清 `set`、`map`、`unordered_map` 的基本区别

---

## 二、`set` 是什么

### 1. 基本理解

- `set` 是集合容器
- 元素**自动去重**
- 元素**默认按从小到大有序存放**

```cpp
#include <set>
set<int> s;
```

### 2. 当前阶段怎么理解

- 往里面放重复元素，只会保留一份
- 不需要你手动排序，它会自己保持有序

### 3. 当前阶段先记一句话

- **`set` = 自动去重 + 自动有序**

---

## 三、`map` 是什么

### 1. 基本理解

- `map` 存的是**键值对**
- 每个键 `key` 对应一个值 `value`
- 键默认按从小到大有序存放

```cpp
#include <map>
map<string, int> mp;
```

### 2. 当前阶段怎么理解

- 可以把它想成“字典”
- 例如：姓名对应分数，单词对应次数

### 3. 当前阶段先记一句话

- **`map` = 有序的键值映射表**

---

## 四、`set` 的常见操作

### 1. 插入 `insert`

```cpp
s.insert(10);
s.insert(20);
s.insert(10);
```

- 重复插入 `10` 也只会保留一个

### 2. 删除 `erase`

```cpp
s.erase(10);
```

### 3. 查找 `find`

```cpp
if (s.find(20) != s.end()) {
	cout << "found" << '\\n';
}
```

### 4. 统计是否存在 `count`

```cpp
cout << s.count(20) << '\\n';
```

- 对 `set` 来说，结果通常只有 `0` 或 `1`

### 5. 当前阶段先记

- **`insert`、`erase`、`find`、`count` 是最高频**

---

## 五、`map` 的常见操作

### 1. 赋值

```cpp
mp["Tom"] = 95;
mp["Alice"] = 88;
```

### 2. 访问值

```cpp
cout << mp["Tom"] << '\\n';
```

### 3. 删除 `erase`

```cpp
mp.erase("Tom");
```

### 4. 查找 `find`

```cpp
if (mp.find("Alice") != mp.end()) {
	cout << "found" << '\\n';
}
```

### 5. 当前阶段先记

- **`map[key]` 很常用**
- **`find` 适合判断键是否存在**

---

## 六、`map[key]` 怎么理解

### 1. 最基础理解

- `mp[key]` 表示访问这个键对应的值
- 如果这个键不存在，`map` 会先创建它

```cpp
map<string, int> mp;
cout << mp["Tom"] << '\\n';
```

- 这行会自动创建键 `"Tom"`
- 默认值是 `0`

### 2. 为什么这是高频点

- 统计次数时特别方便
- 但也容易在“只想判断存在性”时误创建新键

### 3. 当前阶段先记

- **只判断是否存在时，更稳妥地优先用 `find`**

---

## 七、遍历 `set` 和 `map`

### 1. 遍历 `set`

```cpp
for (int x : s) {
	cout << x << '\\n';
}
```

### 2. 遍历 `map`

```cpp
for (auto [key, value] : mp) {
	cout << key << " " << value << '\\n';
}
```

### 3. 当前阶段怎么理解

- `set` 遍历出来的是值
- `map` 遍历出来的是键值对

### 4. 结构化绑定先记

- `auto [key, value]` 在读 `map` 时非常方便

---

## 八、`set` 和 `map` 最常见应用

### 1. `set` 做去重

```cpp
vector<int> v = {1, 2, 2, 3, 3, 3};
set<int> s(v.begin(), v.end());
```

- `s` 中只会留下不同元素

### 2. `map` 做计数

```cpp
string s = "banana";
map<char, int> cnt;
for (char ch : s) {
	cnt[ch]++;
}
```

### 3. `map` 做映射关系

```cpp
map<string, int> score;
score["Tom"] = 100;
```

### 4. 当前阶段怎么理解

- **去重**先想 `set`
- **计数 / 映射**先想 `map`

---

## 九、`set` / `map` 和数组的区别

|对比项|数组 / `vector`|`set` / `map`|
|---|---|---|
|访问方式|下标|按值 / 按键|
|是否自动去重|否|`set` 是|
|是否自动有序|否|默认是|
|适合场景|顺序存储|查重、计数、映射|

### 当前阶段先记结论

- **顺序存数据**常想数组 / `vector`
- **查重和映射**常想 `set` / `map`

---

## 十、`unordered_set` 和 `unordered_map` 先有印象

### 1. 基本理解

- `unordered_set` 和 `unordered_map` 是哈希结构
- 一般查找更快
- 但默认**不保证有序**

### 2. 当前阶段怎么理解

- 要有序：优先想 `set` / `map`
- 不在乎顺序，想更快：可以考虑 `unordered_set` / `unordered_map`

### 3. 当前阶段先记

- **有序**和**无序**是这两组容器最关键的区别之一

---

## 十一、当前阶段最常见写法

### 1. `set` 去重模板

```cpp
set<int> s;
s.insert(1);
s.insert(2);
s.insert(2);
```

### 2. `map` 计数模板

```cpp
map<char, int> cnt;
for (char ch : s) {
	cnt[ch]++;
}
```

### 3. 查找模板

```cpp
if (mp.find(key) != mp.end()) {
	cout << "found" << '\\n';
}
```

### 4. 遍历模板

```cpp
for (auto [key, value] : mp) {
	cout << key << " " << value << '\\n';
}
```

---

## 十二、现代 C++ 的基础习惯

### 1. 看到这些关键词先想到 `set`

- 去重
- 判重
- 是否出现过
- 自动有序

### 2. 看到这些关键词先想到 `map`

- 计数
- 映射
- 频率统计
- 键值关系

### 3. 判断存在优先考虑 `find`

- 尤其是 `map` 中
- 避免无意间创建新键

### 4. 以后会继续补

- 自定义排序规则
- 迭代器细节
- 多重集合 `multiset`
- 多重映射 `multimap`
- 但当前阶段先把基础用法学稳

---

## 十三、高频易错点

1. `set` 自动去重，`map` 存键值对
2. `set` / `map` 默认都是有序的
3. `map[key]` 在键不存在时会创建新键
4. 只判断存在时优先用 `find`
5. `count` 对 `set` 常是 `0` 或 `1`
6. `set` 不能像数组那样下标访问
7. `map` 遍历出来的是键值对，不是单个值
8. `unordered_map` / `unordered_set` 默认不保证顺序

---

## 十四、最简模板

### `set` 模板

```cpp
#include <set>
set<int> s;
```

### `map` 模板

```cpp
#include <map>
map<string, int> mp;
```

### 计数模板

```cpp
map<char, int> cnt;
for (char ch : s) {
	cnt[ch]++;
}
```

### 查找模板

```cpp
if (mp.find(key) != mp.end()) {
	cout << "found" << '\\n';
}
```

<aside> 📌

这一节真正要掌握的，不只是“`set` 和 `map` 是什么”，而是看到题目时能立刻判断：**这里是在去重、判重、做计数，还是在建立键值映射关系。**

</aside>