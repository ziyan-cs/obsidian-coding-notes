#cpp #stl #stack

## ⚡ TL;DR（快速决策）

- 只需要“后进先出” → `stack`
- 只关心栈顶元素 → `top()`
- 频繁入栈 / 出栈 → `push()` / `pop()`
- 需要遍历全部元素或随机访问 → `stack` 往往不合适
- 括号匹配、表达式处理、单调栈基础 → 第一反应先想栈

## 🧩 Core Idea（核心本质）

- **本质**：`stack` 是后进先出（LIFO）的容器适配器
- **关键机制**：最后放进去的元素，最先被取出来
- **核心优势**：接口简单，栈顶操作清晰，非常适合处理嵌套关系和回退过程

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础定义

```cpp
stack<int> st;
```

### 2. 入栈 / 出栈 / 取栈顶

```cpp
st.push(10);
st.push(20);
cout << st.top() << '\n';
st.pop();
```

### 3. 判空与大小

```cpp
cout << st.empty() << '\n';
cout << st.size() << '\n';
```

### 4. 循环弹栈

```cpp
while (!st.empty()) {
	cout << st.top() << '\n';
	st.pop();
}
```

### 5. 括号匹配基础

```cpp
stack<char> st;
for (char ch : s) {
	if (ch == '(') {
		st.push(ch);
	} else if (ch == ')') {
		if (st.empty()) {
			cout << "invalid" << '\n';
			break;
		}
		st.pop();
	}
}
```

### 6. 用 `vector` 模拟栈

```cpp
vector<int> st;
st.push_back(10);
st.push_back(20);
cout << st.back() << '\n';
st.pop_back();
```

### 7. 单调栈先有印象

```cpp
stack<int> st;
for (int x : nums) {
	while (!st.empty() && st.top() < x) {
		st.pop();
	}
	st.push(x);
}
```

## ⚠️ Pitfalls（高频错误）

- 空栈不能直接调用 `top()` 或 `pop()`
- `pop()` 没有返回值，不能写成 `x = st.pop()`
- 以为 `stack` 可以遍历，其实它不提供普通迭代接口
- 把“栈顶元素”和“最后处理结果”混在一起，逻辑容易错
- 括号匹配类题目里，忘记先判空再 `pop()`
- `stack` 是容器适配器，不是完整顺序容器
- 需要查看非栈顶元素时，`stack` 通常不是合适选择
- 单调栈题目里，比较条件写反会导致整题思路失效

## 🚀 Performance / Tips（性能优化）

- `stack` 接口很轻，适合纯 LIFO 场景
- 只要题目本质是“最近未处理元素”，就要想到栈
- 如果需要更多控制，有时直接用 `vector` 模拟栈更灵活
- 单调栈本质上仍然是栈，只是多了“维护顺序”的约束
- 刷题里常见的不是“普通栈难”，而是“什么时候该想到栈”

## 🧪 Common Scenarios（常见使用场景）

- **括号匹配**：左括号入栈，右括号匹配出栈
- **表达式求值基础**：操作符 / 操作数管理
- **函数调用栈理解**：后进先出模型
- **单调栈题型**：下一个更大元素、柱状图面积
- **撤销 / 回退逻辑**：最近操作优先撤销

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	stack<int> st;
	st.push(1);
	st.push(2);
	cout << st.top() << '\n';
	st.pop();
	cout << st.top() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `stack` 的核心不是“存数据”，而是它强制你按 **后进先出** 的方式处理最近进入的元素，这正好对应很多嵌套、匹配、回退和单调结构问题。