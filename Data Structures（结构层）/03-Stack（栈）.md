#cpp #data-structure #stack

## ⚡ TL;DR（快速决策）

- 需要“后进先出”处理顺序 → 栈
- 只关心最近进入、最近未处理的元素 → 优先想栈
- 题目出现括号匹配、表达式求值、撤销、单调栈 → 先想到栈模型
- 需要随机访问或中间查找 → 栈通常不合适
- 栈题的关键通常不在结构，而在“压栈 / 出栈时机”是否正确

## 🧩 Core Idea（核心本质）

- **本质**：栈是后进先出（LIFO）的线性结构
- **关键机制**：最后进入栈的元素，最先被访问或移除
- **核心优势**：天然适合表达嵌套关系、回退过程和“最近相关元素”问题

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础栈操作

```cpp
stack<int> st;
st.push(1);
st.push(2);
cout << st.top() << '\n';
st.pop();
```

### 2. 判空后处理栈顶

```cpp
if (!st.empty()) {
	cout << st.top() << '\n';
}
```

### 3. 循环弹栈

```cpp
while (!st.empty()) {
	cout << st.top() << '\n';
	st.pop();
}
```

### 4. 括号匹配基础模型

```cpp
stack<char> st;
for (char ch : s) {
	if (ch == '(') {
		st.push(ch);
	} else if (ch == ')') {
		if (st.empty()) return false;
		st.pop();
	}
}
return st.empty();
```

### 5. 用 `vector` 模拟栈

```cpp
vector<int> st;
st.push_back(1);
st.push_back(2);
cout << st.back() << '\n';
st.pop_back();
```

### 6. 单调栈基础模型

```cpp
stack<int> st;
for (int x : nums) {
	while (!st.empty() && st.top() < x) {
		st.pop();
	}
	st.push(x);
}
```

### 7. 表达式 / 逆波兰求值先有印象

```cpp
stack<int> st;
// 遇到数字入栈，遇到运算符弹出两个数计算后再压回
```

### 8. DFS 非递归先有印象

```cpp
stack<int> st;
st.push(start);
while (!st.empty()) {
	int u = st.top();
	st.pop();
}
```

## ⚠️ Pitfalls（高频错误）

- 空栈不能直接 `top()` 或 `pop()`
- `pop()` 没有返回值，不能写成 `x = st.pop()`
- 栈顶元素和“当前答案”不要混，很多题只是借助栈维护过程
- 括号匹配题最容易忘记先判空再出栈
- 单调栈题里比较条件写反，整题会直接失效
- 以为栈适合遍历全部历史元素，其实它只擅长处理最近元素
- `stack` 不能随机访问，也不提供普通迭代接口
- 很多栈题真正难点是“什么时候压栈，什么时候弹栈”

## 🚀 Performance / Tips（性能优化）

- 只要问题本质是“最近未匹配 / 最近未处理”，就该先想到栈
- 纯刷题场景里，用 `vector` 模拟栈有时更灵活
- 单调栈本质仍然是栈，只是额外维护顺序约束
- 栈强项是维护过程，不是保存最终有序结果
- 写栈题时，优先先画出 3 到 5 步入栈 / 出栈过程

## 🧪 Common Scenarios（常见使用场景）

- **括号匹配**：左括号入栈，右括号匹配出栈
- **表达式求值**：操作数 / 运算符管理
- **单调栈题型**：下一个更大元素、柱状图最大矩形
- **撤销 / 回退逻辑**：最近操作优先撤销
- **非递归 DFS**：手动维护搜索过程
- **函数调用栈理解**：后进先出执行模型

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

	if (!st.empty()) {
		cout << st.top() << '\n';
	}

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 栈的核心不是“存一批元素”，而是利用 **后进先出** 的顺序持续处理“最近进入、最近相关”的状态，这正好对应匹配、回退、单调维护和非递归搜索。