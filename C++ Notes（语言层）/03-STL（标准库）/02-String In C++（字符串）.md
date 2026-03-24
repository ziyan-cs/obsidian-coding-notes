#cpp #stl #string

## ⚡ TL;DR（快速决策）

- 处理文本、字符序列 → `string`
- 需要随机访问字符 → `s[i]`
- 需要拼接文本 → `+` 或 `append()`
- 需要查找子串 / 字符 → `find()`
- 高频修改头部或中间内容很多时，`string` 不一定是最优选择

## 🧩 Core Idea（核心本质）

- **本质**：`string` 是管理字符序列的标准库容器
- **关键机制**：它像“专门处理字符的动态数组”，支持长度变化、随机访问和常见文本操作
- **核心优势**：接口统一、使用自然、与 STL 和算法题场景结合紧密

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础定义

```cpp
string s = "hello";
```

### 2. 输入字符串

```cpp
string s;
cin >> s;
```

### 3. 读取整行

```cpp
string line;
getline(cin, line);
```

### 4. 访问字符

```cpp
cout << s[0] << '\\n';
cout << s.back() << '\\n';
```

### 5. 遍历字符串

```cpp
for (char ch : s) {
	cout << ch << '\\n';
}

for (int i = 0; i < s.size(); ++i) {
	cout << s[i] << '\\n';
}
```

### 6. 拼接与追加

```cpp
string a = "hello";
string b = "world";
string c = a + " " + b;
a += "!";
```

### 7. 查找字符 / 子串

```cpp
size_t pos1 = s.find('a');
size_t pos2 = s.find("abc");
```

### 8. 截取子串

```cpp
string sub = s.substr(2, 3);
```

### 9. 排序字符串

```cpp
sort(s.begin(), s.end());
```

### 10. 字符串与数字转换

```cpp
int x = stoi("123");
string t = to_string(456);
```

## ⚠️ Pitfalls（高频错误）

- `cin >> s` 遇到空格就停止，整行输入要用 `getline`
- `cin >>` 后直接 `getline`，容易读到残留空行
- `s[i]` 不做边界检查，越界是未定义行为
- `find()` 找不到时返回的是 `string::npos`
- `substr(pos, len)` 的 `pos` 越界会出问题
- 以为 `string` 是 C 风格字符串，结果把 `char*` 和 `string` 混用写乱
- 修改字符串后，旧引用 / 指针 / 迭代器可能失效
- `size()` 返回无符号类型，和负数比较时要小心

## 🚀 Performance / Tips（性能优化）

- 只读传参优先用：

```cpp
const string& s
```

- 高频拼接大文本时，要关注重复扩容成本
- 单字符处理优先直接用 `char`
- 判断是否找到时优先写：

```cpp
if (s.find("abc") != string::npos) {
	// found
}
```

- 需要字符统计时，经常和数组 / 哈希表 / 滑动窗口一起使用

## 🧪 Common Scenarios（常见使用场景）

- **刷题读入字符串**：`cin >> s` / `getline`
- **回文 / 双指针**：按下标访问字符
- **字符频率统计**：遍历 `string`
- **子串判断**：`find()` / `substr()`
- **异位词 / 字典序问题**：`sort(s.begin(), s.end())`
- **数字与文本互转**：`stoi` / `to_string`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string s = "cba";
	sort(s.begin(), s.end());
	cout << s << '\\n';

	if (s.find("ab") != string::npos) {
		cout << "found" << '\\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `string` 的核心不是“能存文本”，而是它提供了 **动态长度 + 随机访问 + 常见文本操作接口**，让大多数字符串题和通用文本处理都能直接落在统一模型上。