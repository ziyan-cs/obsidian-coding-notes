#cpp #class #constructor #destructor

## ⚡ TL;DR（快速决策）

- **构造函数**：对象创建时自动调用，用来完成初始化
- **析构函数**：对象销毁时自动调用，用来完成清理
- 学这一节最重要的是分清：**什么时候自动调用、能不能重载、有没有返回类型、栈对象和堆对象析构时机是否一样**
- 构造函数最常见类型：**默认构造、带参构造、拷贝构造**
- 入门阶段最容易错的不是定义，而是：**把构造函数当普通函数、忘记初始化列表、堆对象 `new` 了却不 `delete`**
- 如果一个类自己管理资源，构造与析构往往必须成对考虑

## 🧩 Core Idea（核心本质）

- **本质**：构造函数负责“对象生下来时怎么初始化”，析构函数负责“对象离开时怎么收尾”
- **关键机制**：它们会在对象生命周期的开始和结束被自动调用
- **核心价值**：让对象从创建到销毁都保持“状态完整、资源成对管理”
- **一句理解**：构造是“把对象准备好”，析构是“把对象收拾干净”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础类 + 默认构造

```cpp
class Person {
public:
	string name;
	int age;

	Person() {
		name = "unknown";
		age = 0;
	}
};
```

### 2. 带参构造

```cpp
class Person {
public:
	string name;
	int age;

	Person(string n, int a) {
		name = n;
		age = a;
	}
};
```

### 3. 初始化列表写法（更推荐）

```cpp
class Person {
public:
	string name;
	int age;

	Person(string n, int a) : name(n), age(a) {}
};
```

### 4. 拷贝构造

```cpp
class Person {
public:
	string name;

	Person(string n) : name(n) {}

	Person(const Person& other) {
		name = other.name;
	}
};
```

### 5. 析构函数

```cpp
class Person {
public:
	Person() {
		cout << "构造函数调用" << '\n';
	}

	~Person() {
		cout << "析构函数调用" << '\n';
	}
};
```

### 6. 栈对象调用时机

```cpp
int main() {
	Person p;
	return 0;
}
```

- `p` 创建时自动调用构造
- `main` 结束时自动调用析构

### 7. 堆对象调用时机

```cpp
int main() {
	Person* p = new Person();
	delete p;
	return 0;
}
```

- `new Person()` 时调用构造
- `delete p` 时调用析构

### 8. 动态数组对应关系

```cpp
Person* arr = new Person[3];
delete[] arr;
```

- `new[]` 对 `delete[]`
- 不要写成 `delete arr`

## ⚠️ Pitfalls（高频错误）

- 构造函数**没有返回类型**，连 `void` 都不能写
- 构造函数名必须和类名完全一致
- 析构函数名是 `~类名`，也**没有返回类型**
- 一个类可以有多个构造函数，但**只能有一个析构函数**
- 构造函数可以重载，析构函数不能重载
- 析构函数不能带参数
- 定义了堆对象却忘记 `delete`，析构函数就不会按预期执行
- `new[]` 必须配 `delete[]`，不能和 `delete` 混用
- 不要把“对象销毁”和“指针变量销毁”混为一谈
- 如果类里有指针资源，默认拷贝可能只是浅拷贝，后面容易出问题

## 🚀 Performance / Tips（性能优化）

- 成员初始化优先考虑**初始化列表**：

```cpp
Person(string n, int a) : name(n), age(a) {}
```

它通常比“先默认构造、再赋值”更自然也更高效

- 对于 `const` 成员、引用成员，**必须**用初始化列表：

```cpp
class A {
public:
	const int x;
	A(int v) : x(v) {}
};
```

- 如果类不需要手动管理资源，很多时候直接用标准库类型（如 `string`、`vector`）会更省心
- 高频记忆规则：

```cpp
构造：对象创建时调用
析构：对象销毁时调用
栈对象：离开作用域自动析构
堆对象：delete 时析构
```

- 如果一个类自己申请了堆内存，至少要开始警惕：

```cpp
拷贝构造
拷贝赋值
析构函数
```

也就是常说的 Rule of Three 的前置理解

## 🧪 Common Scenarios（常见使用场景）

- **对象创建时给默认值**：默认构造函数
- **创建对象时直接传入数据**：带参构造函数
- **用已有对象初始化新对象**：拷贝构造函数
- **对象离开作用域时自动清理**：析构函数
- **类里管理文件 / 动态内存 / 句柄等资源**：构造负责获取，析构负责释放
- **刷题中自定义类 / 结构体时**：常见带参构造、初始化列表
- **面向对象题目里**：这部分是类对象生命周期的基础

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Person {
public:
	string name;
	int age;

	Person() : name("unknown"), age(0) {
		cout << "默认构造" << '\n';
	}

	Person(string n, int a) : name(n), age(a) {
		cout << "带参构造" << '\n';
	}

	~Person() {
		cout << "析构函数" << '\n';
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Person p1;
	Person p2("YAN", 18);

	cout << p2.name << ' ' << p2.age << '\n';
	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 构造与析构的核心不是“会写两个特殊函数”，而是把 **对象何时初始化、何时销毁、资源何时获取与释放** 这几件事想清楚，让对象生命周期完整、稳定、可控。