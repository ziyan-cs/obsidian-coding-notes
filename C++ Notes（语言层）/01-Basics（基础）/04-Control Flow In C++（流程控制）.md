#cpp #basics #control-flow 

## ⚡ TL;DR（快速决策）

- 条件分支 → `if / else if / else`
- 多分支离散值判断 → `switch`
- 已知次数重复 → `for`
- 未知次数、按条件重复 → `while`
- 流程控制题真正关键通常不在语法，而在“条件是否准确、循环是否收敛、边界是否正确”

## 🧩 Core Idea（核心本质）

- **本质**：流程控制是在决定程序“按什么条件执行、重复执行、提前结束或跳过某一步”
- **关键机制**：通过分支和循环，把静态代码变成随输入变化而变化的执行路径
- **核心目标**：把判断、重复、终止和状态更新写清楚，让程序逻辑稳定可控

## 🔧 Usage Patterns（可复用代码模板）

### 1. 条件分支 `if / else`

```cpp
if (x > 0) {
	cout << "positive" << '\\n';
} else if (x == 0) {
	cout << "zero" << '\\n';
} else {
	cout << "negative" << '\\n';
}
```

### 2. `switch` 多分支

```cpp
switch (op) {
	case '+':
		cout << "add" << '\\n';
		break;
	case '-':
		cout << "sub" << '\\n';
		break;
	default:
		cout << "unknown" << '\\n';
}
```

### 3. `for` 循环

```cpp
for (int i = 0; i < n; ++i) {
	cout << i << '\\n';
}
```

### 4. `while` 循环

```cpp
int i = 0;
while (i < n) {
	cout << i << '\\n';
	++i;
}
```

### 5. `do-while` 先有印象

```cpp
int x = 0;
do {
	cout << x << '\\n';
	++x;
} while (x < 3);
```

### 6. `break` 提前退出

```cpp
for (int i = 0; i < n; ++i) {
	if (a[i] == target) {
		cout << i << '\\n';
		break;
	}
}
```

### 7. `continue` 跳过当前轮

```cpp
for (int i = 0; i < n; ++i) {
	if (a[i] < 0) continue;
	cout << a[i] << '\\n';
}
```

### 8. 常见刷题读入循环

```cpp
int x;
while (cin >> x) {
	cout << x << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- `=` 是赋值，`==` 才是比较，条件里最容易写错
- `if` / `else` 逻辑顺序写错，后面分支可能永远进不到
- 循环变量忘记更新，直接死循环
- `for (int i = 0; i <= n; ++i)` 很容易越界
- `switch` 忘记 `break`，会继续穿透到下一个分支
- `continue` 只跳过当前轮，不会结束整个循环
- `break` 只退出当前这一层循环，不会直接退出所有嵌套循环
- 条件写得太复杂时，逻辑 bug 往往比语法 bug 更多

## 🚀 Performance / Tips（性能优化）

- 条件判断先追求正确和清晰，再谈写短
- 多重判断如果是离散常量分支，可以优先考虑 `switch`
- 写循环前优先先想清：

```cpp
循环变量是谁
循环何时结束
每轮更新什么
```

- 嵌套循环里如果能提早 `break`，通常能少做很多无效工作
- 刷题里很多错误并不是“不会算法”，而是流程控制边界没写稳

## 🧪 Common Scenarios（常见使用场景）

- **分类讨论**：`if / else if / else`
- **菜单 / 操作符判断**：`switch`
- **遍历数组 / 字符串**：`for`
- **条件驱动处理**：`while`
- **读到 EOF**：`while (cin >> x)`
- **搜索到答案提前停止**：`break`
- **过滤无效数据**：`continue`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n;
	cin >> n;

	for (int i = 0; i < n; ++i) {
		if (i % 2 == 0) {
			cout << i << '\\n';
		}
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 流程控制的核心不是记住 `if`、`for`、`while` 这些关键字，而是把 **条件、循环、更新和终止时机** 写准确，让程序执行路径始终符合你的预期。