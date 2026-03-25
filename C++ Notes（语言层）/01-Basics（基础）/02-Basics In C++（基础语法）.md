#cpp #basics #syntax

## ⚡ TL;DR（快速决策）

- 变量、条件、循环、函数 → C++ 语法骨架
- 先掌握：类型、输入输出、分支、循环、函数、数组 / `vector`
- 刷题默认模板 → `int main()` + 快速 IO + 基础控制流
- 语法题大多数问题不在“不会写”，而在类型、作用域、边界和初始化

## 🧩 Core Idea（核心本质）

- **本质**：基础语法就是组织程序的最小表达单元
- **关键机制**：数据靠类型约束，逻辑靠控制流驱动，复用靠函数抽象
- **核心目标**：把“数据 + 判断 + 重复 + 封装”这四件事写稳定

## 🔧 Usage Patterns（可复用代码模板）

### 1. 变量与基础类型

```cpp
int a = 10;
long long b = 10000000000LL;
double x = 3.14;
char ch = 'A';
bool ok = true;
string s = "hello";
```

### 2. 输入输出骨架

```cpp
int n;
string s;
cin >> n >> s;
cout << n << ' ' << s << '\n';
```

### 3. 条件分支

```cpp
if (x > 0) {
	cout << "positive" << '\n';
} else if (x == 0) {
	cout << "zero" << '\n';
} else {
	cout << "negative" << '\n';
}
```

### 4. 循环

```cpp
for (int i = 0; i < n; ++i) {
	cout << i << '\n';
}

int i = 0;
while (i < n) {
	++i;
}
```

### 5. 函数

```cpp
int add(int a, int b) {
	return a + b;
}
```

### 6. 数组与 `vector`

```cpp
int a[5] = {1, 2, 3, 4, 5};
vector<int> nums = {1, 2, 3};
nums.push_back(4);
```

### 7. 字符串

```cpp
string s = "abc";
cout << s.size() << '\n';
cout << s[0] << '\n';
```

### 8. 标准刷题主函数

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	return 0;
}
```

## ⚠️ Pitfalls（高频错误）

- `int` 范围不够时要换 `long long`
- `=` 是赋值，`==` 才是比较
- 变量未初始化就使用，结果不可靠
- 数组 / 字符串下标从 `0` 开始
- `for (int i = 0; i <= n; ++i)` 很容易越界
- `cin >>` 读字符串时遇到空格会停
- 函数参数传值会拷贝，大对象优先考虑引用
- 作用域写乱时，变量重名会导致逻辑错误

## 🚀 Performance / Tips（性能优化）

- 刷题默认先写：

```cpp
ios::sync_with_stdio(false);
cin.tie(nullptr);
```

- 大整数优先先想 `long long`
- 输出换行优先用 `\n`，少用 `endl`
- 不确定变量初值时，显式初始化，不靠默认行为
- 能拆函数就拆函数，主逻辑会更稳定

## 🧪 Common Scenarios（常见使用场景）

- **刷题输入处理**：变量声明 + `cin/cout`
- **分类讨论**：`if / else if / else`
- **重复处理数据**：`for` / `while`
- **封装重复逻辑**：函数
- **存一组数据**：数组 / `vector`
- **处理文本**：`string`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n;
	cin >> n;

	vector<int> a(n);
	for (int i = 0; i < n; ++i) {
		cin >> a[i];
	}

	for (int i = 0; i < n; ++i) {
		cout << a[i] << '\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 基础语法的核心不是背关键字，而是把 **类型、控制流、函数和数据结构入口** 这几块写稳定，让程序能正确表达你的思路。