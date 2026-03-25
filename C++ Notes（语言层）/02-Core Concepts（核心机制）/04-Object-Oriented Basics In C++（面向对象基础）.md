#cpp #oop #class-design

## ⚡ TL;DR（快速决策）

- 只是写几个函数能跑 → 不一定需要面向对象
- 数据和行为天然属于同一个抽象 → 考虑面向对象建模
- 需要封装状态、隐藏实现细节 → 用类
- 有明确对象边界、职责和交互关系 → 面向对象更自然
- 面向对象基础重点不在语法，而在“对象、职责、接口、封装”这四件事

## 🧩 Core Idea（核心本质）

- **本质**：面向对象是在用“对象”组织程序，把状态和行为绑定到一起
- **关键机制**：对象保存状态，成员函数定义行为，访问控制约束外部如何使用对象
- **核心目标**：让代码按职责拆分、接口稳定、内部实现可替换

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础对象建模

```cpp
class User {
private:
	string name;
	int age;

public:
	User(string n, int a) : name(n), age(a) {}

	string get_name() const {
		return name;
	}

	int get_age() const {
		return age;
	}
};
```

### 2. 封装“状态 + 行为”

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

### 3. 通过接口修改内部状态

```cpp
class BankAccount {
private:
	int balance;

public:
	BankAccount() : balance(0) {}

	void deposit(int money) {
		balance += money;
	}

	int get_balance() const {
		return balance;
	}
};
```

### 4. 构造函数保证对象初始合法

```cpp
class Point {
private:
	int x;
	int y;

public:
	Point(int a, int b) : x(a), y(b) {}
};
```

### 5. 只读接口 `const`

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

### 6. 类外实现成员函数

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

### 7. 用对象组织主流程

```cpp
class Task {
private:
	string title;
	bool done;

public:
	Task(string t) : title(t), done(false) {}

	void finish() {
		done = true;
	}

	bool is_done() const {
		return done;
	}
};
```

## ⚠️ Pitfalls（高频错误）

- 把“用了 class”误以为就是面向对象，真正关键是职责划分是否清晰
- 状态全设成 `public`，对象就失去封装意义
- 全部都设成 `private` 却不给接口，类型会非常难用
- 构造函数不保证初始状态合法，后续对象容易处于错误状态
- 成员函数不加 `const`，导致只读对象接口不完整
- 一个类承担太多职责，最后变成“大杂烩对象”
- 用面向对象解决本来更适合过程式的小题，代码反而更重
- 只会写 getter / setter，却没有真正建模出有意义的行为

## 🚀 Performance / Tips（性能优化）

- 对象设计先看职责，不要一开始就过度抽象
- 小而清晰的类，通常比“万能类”更容易维护
- 构造函数优先用初始化列表：

```cpp
User(string n, int a) : name(n), age(a) {}
```

- 只读成员函数尽量加 `const`
- 大对象在成员函数外传参时，优先 `const T&`
- 刷题时除非题目本身适合建模，否则不要为了“面向对象”而过度设计

## 🧪 Common Scenarios（常见使用场景）

- **业务对象建模**：用户、订单、账户、任务
- **封装状态与规则**：余额、计数器、配置对象
- **需要明确接口边界**：只允许通过成员函数修改状态
- **中大型代码组织**：把数据和行为按职责分组
- **面试基础问答**：对象、类、封装、构造函数、访问控制

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

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

	Counter c;
	c.add_one();
	cout << c.get() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 面向对象基础的核心不是“会写 class”，而是先判断：**哪些状态和行为应该属于同一个对象，以及这个对象该暴露什么接口、隐藏什么实现。**