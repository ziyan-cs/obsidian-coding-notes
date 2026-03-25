#cpp #copy-control #copy-constructor #copy-assignment

## ⚡ TL;DR（快速决策）

- **拷贝控制**：决定对象在“拷贝、赋值、销毁”时怎么表现
- 入门阶段最重要的是先分清：**拷贝构造、拷贝赋值、析构函数**
- 如果类里没有自己管理资源，编译器默认生成的版本很多时候就够用
- 如果类里自己管理了堆内存 / 文件 / 句柄等资源，就必须认真考虑拷贝控制
- 拷贝控制最容易错的不是“会不会写函数”，而是：**浅拷贝导致重复释放、赋值和初始化混淆、忘记自赋值问题**
- 一旦类中出现裸指针资源，就要立刻警惕 Rule of Three

## 🧩 Core Idea（核心本质）

- **本质**：拷贝控制是在规定“一个对象被复制、被赋值、被销毁时，该怎么处理内部资源”
- **关键机制**：对象看起来只是“复制了一份”，但底层资源未必能直接共享
- **核心价值**：避免浅拷贝、内存泄漏、重复释放、悬空指针等问题
- **一句理解**：拷贝控制不是管“值怎么抄一份”，而是管“资源怎么安全地复制和释放”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 默认拷贝（成员逐个拷贝）

```cpp
class A {
public:
	int x;
	string s;
};
```

- 这种类通常默认拷贝就没问题

### 2. 拷贝构造函数

```cpp
class A {
public:
	int x;

	A(int v) : x(v) {}

	A(const A& other) {
		x = other.x;
	}
};
```

### 3. 拷贝赋值运算符

```cpp
class A {
public:
	int x;

	A& operator=(const A& other) {
		x = other.x;
		return *this;
	}
};
```

### 4. 析构函数

```cpp
class A {
public:
	~A() {
		cout << "destruct" << '\n';
	}
};
```

### 5. 带堆内存的深拷贝

```cpp
class Buffer {
public:
	int* data;

	Buffer(int v) {
		data = new int(v);
	}

	Buffer(const Buffer& other) {
		data = new int(*other.data);
	}

	~Buffer() {
		delete data;
	}
};
```

### 6. 拷贝赋值处理自赋值

```cpp
class Buffer {
public:
	int* data;

	Buffer(int v) {
		data = new int(v);
	}

	Buffer& operator=(const Buffer& other) {
		if (this == &other) return *this;
		delete data;
		data = new int(*other.data);
		return *this;
	}

	~Buffer() {
		delete data;
	}
};
```

### 7. 初始化 vs 赋值

```cpp
A a(10);
A b = a;   // 拷贝构造
A c(20);
c = a;     // 拷贝赋值
```

### 8. 禁止拷贝

```cpp
class A {
public:
	A() = default;
	A(const A&) = delete;
	A& operator=(const A&) = delete;
};
```

### 9. 显式使用默认实现

```cpp
class A {
public:
	A() = default;
	A(const A&) = default;
	A& operator=(const A&) = default;
	~A() = default;
};
```

## ⚠️ Pitfalls（高频错误）

- **拷贝构造** 和 **拷贝赋值** 不是一回事
- `A b = a;` 是初始化，走的是拷贝构造，不是赋值运算符
- 默认拷贝很多时候只是浅拷贝，遇到裸指针资源就容易出问题
- 浅拷贝后多个对象指向同一块堆内存，析构时可能重复释放
- 拷贝赋值运算符要记得返回 `*this`
- 拷贝赋值要考虑 **自赋值**：`a = a;`
- 如果类自己写了析构函数，往往就要开始检查是否也该考虑拷贝构造和拷贝赋值
- 不要把“对象拷贝”和“指针拷贝”混为一谈
- 如果只是普通成员（如 `int`、`string`、`vector`），默认拷贝通常已经够好
- 初学阶段一看到裸指针成员，就要提高警惕

## 🚀 Performance / Tips（性能优化）

- 如果类成员都是标准库类型：

```cpp
int
string
vector
map
```

很多时候直接用默认拷贝即可，不必手写复杂拷贝控制

- 如果类管理资源，优先先想：

```cpp
析构时要不要释放？
拷贝时要不要深拷贝？
赋值时旧资源要不要先清理？
```

- 高频记忆规则：

```cpp
拷贝构造：用已有对象初始化新对象
拷贝赋值：把已有对象赋值给另一个已存在对象
析构函数：对象销毁时释放资源
```

- Rule of Three 前置记忆：

```cpp
如果一个类需要自定义：
1. 析构函数
2. 拷贝构造函数
3. 拷贝赋值运算符
那这三个通常要一起考虑
```

- 现代 C++ 里，如果能直接用 `string`、`vector`、智能指针，往往比自己手管裸指针更稳

## 🧪 Common Scenarios（常见使用场景）

- **用已有对象初始化新对象**：拷贝构造
- **两个已存在对象之间赋值**：拷贝赋值
- **类中管理堆内存**：必须关注深浅拷贝
- **类中管理文件句柄 / 系统资源**：必须关注资源复制和释放
- **不希望对象被复制**：删除拷贝构造和拷贝赋值
- **普通值类型类**：通常默认拷贝就够用
- **面向对象题目 / 手写类题目**：这节是资源管理基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Buffer {
public:
	int* data;

	Buffer(int v) {
		data = new int(v);
	}

	Buffer(const Buffer& other) {
		data = new int(*other.data);
	}

	Buffer& operator=(const Buffer& other) {
		if (this == &other) return *this;
		delete data;
		data = new int(*other.data);
		return *this;
	}

	~Buffer() {
		delete data;
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Buffer a(10);
	Buffer b = a;
	Buffer c(20);
	c = a;

	cout << *a.data << ' ' << *b.data << ' ' << *c.data << '\n';
	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 拷贝控制的核心不是“多写几个特殊成员函数”，而是把 **对象复制时资源怎么处理、赋值时旧资源怎么清理、销毁时怎么安全释放** 这几件事想清楚，让类在复制和销毁时都稳定可靠。