#cpp #basics #functions 

## ⚡ TL;DR（快速决策）

- 重复逻辑 → 抽成函数
- 只读大对象参数 → `const T&`
- 需要修改外部变量 → 引用 `T&` 或指针
- 只返回一个结果 → `return`
- 函数题大多数问题不在“不会写”，而在参数传递、返回值设计和副作用控制

## 🧩 Core Idea（核心本质）

- **本质**：函数是对一段可复用逻辑的封装
- **关键机制**：输入靠参数传递，输出靠返回值或引用修改
- **核心目标**：把“逻辑拆分 + 接口清晰 + 副作用可控”这三件事做好

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础函数

```cpp
int add(int a, int b) {
	return a + b;
}
```

### 2. 无返回值函数

```cpp
void print_yes() {
	cout << "YES" << '\\n';
}
```

### 3. 只读大对象参数

```cpp
int get_length(const string& s) {
	return s.size();
}
```

### 4. 通过引用修改外部变量

```cpp
void add_one(int& x) {
	++x;
}
```

### 5. 数组 / `vector` 作为参数

```cpp
int sum_vector(const vector<int>& nums) {
	int sum = 0;
	for (int x : nums) sum += x;
	return sum;
}
```

### 6. 返回布尔值做判断

```cpp
bool is_even(int x) {
	return x % 2 == 0;
}
```

### 7. 刷题里拆函数

```cpp
bool check(int x) {
	return x >= 0;
}

void solve() {
	int x;
	cin >> x;
	cout << check(x) << '\\n';
}
```

## ⚠️ Pitfalls（高频错误）

- 大对象按值传递会拷贝，性能和语义都可能不合适
- 需要修改外部变量却写成值传递，函数内改了外面不变
- `void` 函数里误以为能返回结果
- 返回局部数组 / 局部变量地址会出问题
- 函数声明和定义参数类型不一致会报错
- 同名变量遮蔽外部变量时，逻辑容易写乱
- 递归函数漏掉出口，本质也是函数设计错误
- 函数做了太多事，后期难维护、难调试

## 🚀 Performance / Tips（性能优化）

- 大对象参数默认先想：

```cpp
const string& s
const vector<int>& nums
```

- 有修改需求才用非常量引用
- 函数名尽量体现语义，如 `is_valid`、`get_sum`、`build_graph`
- 刷题时优先把“判断逻辑”和“主流程”拆开，`main` / `solve` 会更干净
- 小函数通常更利于调试和复用

## 🧪 Common Scenarios（常见使用场景）

- **重复计算逻辑**：抽成函数
- **判定型问题**：写 `check()` / `is_xxx()`
- **刷题主流程**：写 `solve()`
- **处理容器**：参数传 `const vector<int>&`
- **需要回写结果**：引用参数 / 返回结构体或 `pair`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int add(int a, int b) {
	return a + b;
}

void solve() {
	int a, b;
	cin >> a >> b;
	cout << add(a, b) << '\\n';
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	solve();
	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 函数的核心不是“能封装代码”，而是把 **输入、输出、修改范围和职责边界** 设计清楚，让逻辑可复用、可读、可维护。