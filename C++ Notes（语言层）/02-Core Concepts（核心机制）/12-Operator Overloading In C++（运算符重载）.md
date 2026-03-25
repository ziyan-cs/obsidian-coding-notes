#cpp #operator-overloading #class #oop

## ⚡ TL;DR（快速决策）

- **运算符重载**：让自定义类型也能像内置类型一样使用 `+`、`==`、`<<` 这类运算符
- 入门阶段最重要的是先分清：**哪些运算符能重载、成员函数和全局函数怎么选、重载后语义是否自然**
- 运算符重载的核心目标不是“炫技巧”，而是**让对象使用起来更直观**
- 最常见场景：**自定义类的加法、比较、赋值、下标、输入输出**
- 最容易错的不是语法，而是：**返回值写错、`const` 漏掉、把不该重载的行为硬重载、成员 / 非成员选错**
- 如果一个运算符重载完以后读起来比普通函数还绕，那大概率就不该这么写

## 🧩 Core Idea（核心本质）

- **本质**：运算符重载是给已有运算符赋予“处理自定义类型”的能力
- **关键机制**：表面上还是写 `a + b`，本质上编译器会把它转换成对应的函数调用
- **核心价值**：提升类的可用性和表达力，让代码更贴近自然语义
- **一句理解**：不是在发明新运算符，而是在教已有运算符“如何处理你的类”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础的 `+` 重载

```cpp
class Point {
public:
	int x, y;

	Point(int a = 0, int b = 0) : x(a), y(b) {}

	Point operator+(const Point& other) const {
		return Point(x + other.x, y + other.y);
	}
};
```

### 2. `==` 重载

```cpp
class Point {
public:
	int x, y;

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
};
```

### 3. `!=` 重载

```cpp
class Point {
public:
	int x, y;

	bool operator!=(const Point& other) const {
		return !(*this == other);
	}
};
```

### 4. 前置 `++`

```cpp
class Counter {
public:
	int x;

	Counter(int v = 0) : x(v) {}

	Counter& operator++() {
		++x;
		return *this;
	}
};
```

### 5. 后置 `++`

```cpp
class Counter {
public:
	int x;

	Counter(int v = 0) : x(v) {}

	Counter operator++(int) {
		Counter temp = *this;
		++x;
		return temp;
	}
};
```

### 6. 下标运算符 `[]`

```cpp
class Array {
public:
	int a[10];

	int& operator[](int index) {
		return a[index];
	}
};
```

### 7. 赋值运算符 `=`

```cpp
class A {
public:
	int x;

	A& operator=(const A& other) {
		if (this == &other) return *this;
		x = other.x;
		return *this;
	}
};
```

### 8. 输入输出运算符

```cpp
class Point {
public:
	int x, y;

	Point(int a = 0, int b = 0) : x(a), y(b) {}

	friend ostream& operator<<(ostream& out, const Point& p) {
		out << p.x << ' ' << p.y;
		return out;
	}

	friend istream& operator>>(istream& in, Point& p) {
		in >> p.x >> p.y;
		return in;
	}
};
```

### 9. 关系运算符 `<`

```cpp
class Point {
public:
	int x, y;

	bool operator<(const Point& other) const {
		if (x != other.x) return x < other.x;
		return y < other.y;
	}
};
```

### 10. 复合赋值 `+=`

```cpp
class Point {
public:
	int x, y;

	Point(int a = 0, int b = 0) : x(a), y(b) {}

	Point& operator+=(const Point& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
};
```

## ⚠️ Pitfalls（高频错误）

- 不是所有运算符都能重载，像 `.`、`::`、`?:`、`sizeof` 就不能重载
- 运算符重载**不能改变运算符的优先级、结合性、参数个数**
- 重载出来的行为要符合直觉，不要让 `+` 做减法这种反常操作
- `operator+` 通常返回新对象，`operator+=` 通常返回当前对象引用
- 比较运算符、加法运算符这类只读操作，通常要加 `const`
- 输入输出运算符一般写成**非成员 / friend**，因为左侧是 `ostream` / `istream`
- 前置 `++` 和后置 `++` 不是一回事，后置版本多一个占位参数 `int`
- `operator=` 要考虑**自赋值**，并且通常返回 `*this`
- 下标运算符如果要支持修改元素，返回值通常要写成引用
- 运算符能重载，不代表一定值得重载；语义不自然时普通函数更好

## 🚀 Performance / Tips（性能优化）

- 二元运算符常见推荐写法：

```cpp
Type operator+(const Type& other) const
bool operator==(const Type& other) const
```

- 修改自身并返回自身时，常见写法：

```cpp
Type& operator+=(const Type& other)
Type& operator=(const Type& other)
```

- 只读参数优先传 `const 引用`：

```cpp
const Type& other
```

避免不必要拷贝

- 高频记忆规则：

```cpp
operator+   // 返回新对象
operator+=  // 修改自身
operator==  // 判断是否相等
operator<<  // 输出
operator>>  // 输入
operator[]  // 下标访问
```

- 如果重载后能让代码像这样自然：

```cpp
a + b
a == b
cout << obj
arr[i]
```

通常就说明方向是对的

- 工程里最常见、最实用的重载往往是：

```cpp
=
==
<
<<
>>
[]
+=
```

## 🧪 Common Scenarios（常见使用场景）

- **数学对象相加**：如点、向量、复数的 `+`
- **对象比较大小 / 相等**：`<`、`==`
- **对象直接输出到屏幕**：`<<`
- **对象直接从输入流读取**：`>>`
- **自定义容器支持下标访问**：`[]`
- **资源类的赋值行为定义**：`=`
- **面向对象题目里让类更像内置类型**：常见运算符重载

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Point {
public:
	int x, y;

	Point(int a = 0, int b = 0) : x(a), y(b) {}

	Point operator+(const Point& other) const {
		return Point(x + other.x, y + other.y);
	}

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	friend ostream& operator<<(ostream& out, const Point& p) {
		out << '(' << p.x << ", " << p.y << ')';
		return out;
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Point a(1, 2), b(3, 4);
	Point c = a + b;

	cout << c << '\n';
	cout << (a == b) << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 运算符重载的核心不是“把符号都重载一遍”，而是把 **自定义类型的操作语义写自然、返回类型写正确、读写体验做顺手** 这几件事处理好，让类真正像一个好用的类型。