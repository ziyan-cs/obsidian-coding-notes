#cpp #basics #string 

## ⚡ TL;DR（快速决策）

- 处理单个字符序列、文本输入、查找拼接 → 优先想到 `string`
- 现代 C++ 里做字符串题，默认先用 `string`，不要先上 C 风格字符数组
- 高频操作集中在：读取、访问、遍历、拼接、比较、截取、查找
- 字符串题最容易错的通常不是定义，而是：输入方式、下标越界、`string` 和字符数组混用
- 如果题目涉及子串、模式匹配、双指针、滑动窗口，`string` 往往就是基础载体

## 🧩 Core Idea（核心本质）

- **本质**：`string` 是 C++ 标准库里专门表示字符串的类型，本质上可以看作“可变长字符序列”
- **关键机制**：支持下标访问、遍历、拼接、比较、查找等常见文本操作
- **核心优势**：比 `char[]` 更安全、更直观、更适合刷题和日常开发
- **一句理解**：数组更像“存一串字符”，`string` 更像“专门处理文本的工具”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础定义

```cpp
string s = "hello";
```

### 2. 输入一个单词

```cpp
string s;
cin >> s;
```

### 3. 输入一整行

```cpp
string s;
getline(cin, s);
```

### 4. 下标访问字符

```cpp
cout << s[0] << '\\n';
s[1] = 'a';
```

### 5. 遍历字符串

```cpp
for (int i = 0; i < s.size(); ++i) {
	cout << s[i] << '\\n';
}
```

### 6. 范围 `for` 遍历

```cpp
for (char c : s) {
	cout << c << '\\n';
}
```

### 7. 拼接字符串

```cpp
string a = "hello";
string b = "world";
string c = a + " " + b;
```

### 8. 比较字符串

```cpp
if (a == b) {
	cout << "same";
}
if (a < b) {
	cout << "lexicographically smaller";
}
```

### 9. 常用成员函数

```cpp
string s = "abcdef";
cout << s.size() << '\\n';
cout << s.substr(2, 3) << '\\n';
cout << s.find("cd") << '\\n';
```

### 10. 反转字符串

```cpp
reverse(s.begin(), s.end());
```

## ⚠️ Pitfalls（高频错误）

- `cin >> s` 只能读到空白前，读句子要用 `getline`
- `cin >>` 后直接接 `getline`，常常会读到残留换行，记得先 `cin.ignore(...)`
- `s.size()` 返回的是无符号类型，和 `int` 混用时要注意比较问题
- 下标有效范围是 `0` 到 `s.size() - 1`，写成 `s[s.size()]` 就越界了
- 空字符串上直接访问 `s[0]` 会出问题
- `string` 比较是按字典序，不是按长度
- `find` 找不到时返回 `string::npos`，不要直接当普通下标用
- `char` 和 `string` 不是一回事：`'a'` 是字符，`"a"` 是字符串
- 修改字符可以写 `s[i] = 'x'`，但前提是这个位置存在
- 不要把 `string` 和 C 风格字符串函数硬混着写，初学阶段容易乱

## 🚀 Performance / Tips（性能优化）

- 一般刷题直接用 `string`，不用过早切回 `char[]`
- 判断字符串长度优先 `s.size()`，不要自己手数
- 需要频繁遍历时，先想清是按下标遍历还是按字符遍历
- 如果题目要改原字符串，范围 `for` 应写成：

```cpp
for (char& c : s) {
	c = toupper(c);
}
```

- 高频操作记住这几个就够用：

```cpp
size()
empty()
push_back()
pop_back()
substr()
find()
reverse()
```

- 处理英文字符串统计题时，常配合：

```cpp
数组计数
双指针
滑动窗口
哈希
```

## 🧪 Common Scenarios（常见使用场景）

- **输入姓名 / 句子**：`getline`
- **判断回文串**：双指针 + `string`
- **统计字符出现次数**：字符串遍历 + 计数数组
- **查找子串**：`find`
- **截取一段内容**：`substr`
- **翻转字符串**：`reverse`
- **字典序比较**：直接用比较运算符
- **后续算法载体**：KMP、滑动窗口、哈希字符串、Trie 前置理解

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string s;
	getline(cin, s);

	cout << s << '\n';
	cout << s.size() << '\n';

	for (char c : s) {
		cout << c << ' ';
	}
	cout << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 字符串基础的核心不是“会写 `string s`”，而是把 **输入方式、字符访问、常用操作和边界控制** 这几件事写稳，让 `string` 成为后续字符串算法的可靠载体。