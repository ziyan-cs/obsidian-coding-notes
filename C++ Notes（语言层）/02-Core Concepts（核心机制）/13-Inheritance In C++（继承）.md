#cpp #inheritance #class #oop

## ⚡ TL;DR（快速决策）

- **继承**：让一个类在保留已有成员的基础上继续扩展新能力
- 入门阶段最重要的是先分清：**基类、派生类、public/protected/private 继承、成员访问权限**
- 继承的核心目标不是“少写代码”，而是**表达类之间的层次关系**
- 最常见场景：**把共性放进基类，把个性写在派生类**
- 最容易错的不是语法，而是：**访问权限混乱、把继承和组合搞混、以为继承后什么都能直接访问**
- 如果题目涉及“学生是人”“狗是动物”“经理是员工”这种 is-a 关系，通常就可以想到继承

## 🧩 Core Idea（核心本质）

- **本质**：继承是在描述“派生类是一种特殊的基类”
- **关键机制**：派生类可以复用基类成员，并在此基础上新增自己的成员或行为
- **核心价值**：抽取共性、减少重复、让类层次更清晰
- **一句理解**：基类负责放“共同部分”，派生类负责补“特殊部分”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础继承

```cpp
class Animal {
public:
	void eat() {
		cout << "eat" << '\n';
	}
};

class Dog : public Animal {
public:
	void bark() {
		cout << "bark" << '\n';
	}
};
```

### 2. 派生类使用基类成员

```cpp
Dog d;
d.eat();
d.bark();
```

### 3. 基类 + 派生类新增成员

```cpp
class Person {
public:
	string name;
};

class Student : public Person {
public:
	int score;
};
```

### 4. public 继承

```cpp
class B {
public:
	int x;
};

class D : public B {};
```

- 基类 `public` 成员在派生类里仍是 `public`

### 5. protected 成员

```cpp
class Base {
protected:
	int id;
};

class Derived : public Base {
public:
	void setId(int x) {
		id = x;
	}
};
```

- `protected` 成员类外不能直接访问，但派生类内部可以访问

### 6. 调用基类构造函数

```cpp
class Person {
public:
	string name;
	Person(string n) : name(n) {}
};

class Student : public Person {
public:
	int score;
	Student(string n, int s) : Person(n), score(s) {}
};
```

### 7. 同名成员访问

```cpp
class Base {
public:
	int x = 1;
};

class Derived : public Base {
public:
	int x = 2;
};

Derived d;
cout << d.x << '\n';        // 2
cout << d.Base::x << '\n';  // 1
```

### 8. 多层继承

```cpp
class A {};
class B : public A {};
class C : public B {};
```

### 9. 禁止外部直接访问基类私有成员

```cpp
class Base {
private:
	int secret;
};

class Derived : public Base {
	// 不能直接访问 secret
};
```

## ⚠️ Pitfalls（高频错误）

- 继承不等于“把基类所有东西都随便拿来用”
- 基类 `private` 成员**不会**被派生类直接访问
- `protected` 不是“谁都能用”，它只是对派生类开放，对类外依然不可见
- `public` 继承表达的是最常见的 is-a 关系
- 不要把“继承”和“包含一个对象”混为一谈；不是所有复用都该用继承
- 派生类对象创建时，会先构造基类，再构造派生类
- 派生类对象销毁时，会先析构派生类，再析构基类
- 基类和派生类有同名成员时，可能发生隐藏，要知道作用域限定写法
- 默认情况下 `class Derived : Base` 是 **private 继承**，别忘了显式写 `public`
- 继承是面向对象关系设计工具，不是单纯语法技巧

## 🚀 Performance / Tips（性能优化）

- 写继承时先问自己一句：

```cpp
派生类是不是基类的一种？
```

如果答案不自然，可能就不该用继承

- 高频记忆规则：

```cpp
public 继承：最常用
private 成员：派生类不能直接访问
protected 成员：派生类能访问，类外不能
```

- 构造析构顺序要记牢：

```cpp
构造：先基类，后派生类
析构：先派生类，后基类
```

- 如果派生类需要初始化基类成员，优先在初始化列表里调用基类构造：

```cpp
Student(string n, int s) : Person(n), score(s) {}
```

- 工程 / 面试里继承经常和这些主题一起出现：

```cpp
多态
override
virtual
抽象类
访问控制
```

## 🧪 Common Scenarios（常见使用场景）

- **学生是人**：`Student : public Person`
- **狗是动物**：`Dog : public Animal`
- **经理是员工**：`Manager : public Employee`
- **抽取多个类的共同属性和行为**：放进基类
- **派生类扩展专属字段或方法**：在子类中继续补充
- **后续学习多态、虚函数、抽象类**：继承是基础前提

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Person {
public:
	string name;

	Person(string n) : name(n) {}

	void showName() {
		cout << name << '\n';
	}
};

class Student : public Person {
public:
	int score;

	Student(string n, int s) : Person(n), score(s) {}

	void showScore() {
		cout << score << '\n';
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Student s("YAN", 95);
	s.showName();
	s.showScore();

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 继承的核心不是“让一个类套一个类”，而是把 **共性放到基类、个性留给派生类、访问关系和层次关系设计清楚**，让类之间的组织更自然、更清晰、更可扩展。