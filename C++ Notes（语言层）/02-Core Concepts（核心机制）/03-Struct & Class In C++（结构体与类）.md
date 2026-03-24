#cpp #oop #struct-class

## ⚡ TL;DR（快速决策）

- 只是聚合数据、默认公开成员 → `struct`
- 需要封装行为、控制访问权限 → `class`
- 数据和操作天然属于同一个抽象 → 写成类
- 只是临时数据模型 / 题目节点结构 → `struct` 往往更直接
- `struct` 和 `class` 本质差异很小，核心区别主要是默认访问权限

## 🧩 Core Idea（核心本质）

- **本质**：`struct` 和 `class` 都是在定义自定义类型，把数据和行为组织到一起
- **关键机制**：成员变量描述状态，成员函数描述行为，访问控制决定外部能看到什么
- **核心区别**：`struct` 默认 `public`，`class` 默认 `private`

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础 `struct`

```cpp
struct Point {
	int x;
	int y;
};
```

### 2. `struct` 直接创建对象

```cpp
Point p{1, 2};
cout << p.x << ' ' << p.y << '\\n';
```

### 3. 最基础 `class`

```cpp
class Counter {
private:
	int value;

public:
	Counter() : value(0) {}

	void add_one() {
		++value;
	}

	int get() const {
		return value;
	}
};
```

### 4. 构造函数初始化成员

```cpp
struct User {
	string name;
	int age;

	User(string n, int a) : name(n), age(a) {}
};
```

### 5. 成员函数放在类里

```cpp
struct Rect {
	int w;
	int h;

	int area() const {
		return w * h;
	}
};
```

### 6. 只读成员函数 `const`

```cpp
class Box {
private:
	int value;

public:
	Box(int v) : value(v) {}

	int get() const {
		return value;
	}
};
```

### 7. 刷题里的节点结构

```cpp
struct ListNode {
	int val;
	ListNode* next;

	ListNode(int x) : val(x), next(nullptr) {}
};
```

### 8. 类外实现成员函数

```cpp
class Student {
private:
	string name;

public:
	Student(string n);
	string get_name() const;
};

Student::Student(string n) : name(n) {}

string Student::get_name() const {
	return name;
}
```

## ⚠️ Pitfalls（高频错误）

- 以为 `struct` 和 `class` 是两种完全不同的机制，其实核心能力几乎一样
- 忘了 `class` 默认是 `private`，导致外部不能直接访问成员
- 构造函数没初始化成员，状态可能不完整
- 成员函数不加 `const`，导致常对象不能调用
- 把本该公开的数据全塞进 `private`，又不提供接口，类型会很难用
- 把本该封装的内部状态全暴露成 `public`，后期维护会失控
- 在刷题里把简单节点结构也过度面向对象化，反而增加阅读负担
- 忘记 `this` 背后的语义，成员函数本质上是在操作当前对象

## 🚀 Performance / Tips（性能优化）

- 简单数据载体优先用 `struct`，更轻、更直观
- 有明显不变量或访问约束时再上 `class` 封装
- 构造函数优先用初始化列表：

```cpp
User(string n, int a) : name(n), age(a) {}
```

- 只读成员函数尽量加 `const`，接口语义更清晰
- 大对象传参到成员函数或普通函数时，优先 `const T&`

## 🧪 Common Scenarios（常见使用场景）

- **刷题节点定义**：链表 / 树节点用 `struct`
- **几何 / 坐标 / 区间模型**：简单聚合数据用 `struct`
- **封装状态与行为**：业务对象用 `class`
- **需要访问控制**：用 `private + public` 设计接口
- **需要构造规则**：写构造函数和成员函数

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

struct Point {
	int x;
	int y;

	int sum() const {
		return x + y;
	}
};

class Counter {
private:
	int value;

public:
	Counter() : value(0) {}

	void add_one() {
		++value;
	}

	int get() const {
		return value;
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Point p{1, 2};
	cout << p.sum() << '\\n';

	Counter c;
	c.add_one();
	cout << c.get() << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 `struct` 和 `class` 的核心不是二选一地背区别，而是先判断：**你是在定义一个简单数据模型，还是在定义一个需要封装状态和行为的抽象类型。**