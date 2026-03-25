#cpp #abstract-class #virtual #oop

## ⚡ TL;DR（快速决策）

- **抽象类**：包含至少一个**纯虚函数**的类，不能直接创建对象
- 入门阶段最重要的是先分清：**抽象类不是“不能写实现的类”，而是“不能直接实例化、主要用来定义接口的类”**
- 抽象类通常和 **继承 + 虚函数 + 多态** 一起出现
- 最常见场景：**基类规定统一接口，子类必须给出具体实现**
- 最容易错的不是定义，而是：**把抽象类当普通类实例化、忘记子类实现纯虚函数、搞不清纯虚函数和普通虚函数的区别**
- 如果你想表达“这类对象都该有某种能力，但具体做法交给子类”，通常就可以想到抽象类

## 🧩 Core Idea（核心本质）

- **本质**：抽象类是“只负责定义规范，不负责直接拿来创建对象”的类
- **关键机制**：只要类中有至少一个纯虚函数 `= 0`，这个类就是抽象类
- **核心价值**：强制子类遵守统一接口，让代码更适合面向抽象编程
- **一句理解**：抽象类像“模板规范”，子类才是“具体落地实现”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础抽象类

```cpp
class Animal {
public:
	virtual void speak() = 0;
};
```

### 2. 子类实现纯虚函数

```cpp
class Animal {
public:
	virtual void speak() = 0;
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "Dog bark" << '\n';
	}
};
```

### 3. 抽象类不能实例化

```cpp
class Animal {
public:
	virtual void speak() = 0;
};

// Animal a; // ❌ 错误
```

### 4. 基类指针指向具体子类

```cpp
Animal* p = new Dog();
p->speak();
```

### 5. 基类引用接收子类对象

```cpp
Dog d;
Animal& ref = d;
ref.speak();
```

### 6. 多个子类共享同一抽象接口

```cpp
class Cat : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\n';
	}
};
```

### 7. 抽象类里也可以有普通成员函数

```cpp
class Shape {
public:
	virtual void draw() = 0;

	void info() {
		cout << "This is a shape" << '\n';
	}
};
```

### 8. 抽象类里也可以有数据成员

```cpp
class Person {
public:
	string name;
	virtual void work() = 0;
};
```

### 9. 抽象类通常配虚析构函数

```cpp
class Animal {
public:
	virtual void speak() = 0;
	virtual ~Animal() = default;
};
```

### 10. 子类如果没实现纯虚函数，自己也还是抽象类

```cpp
class Base {
public:
	virtual void func() = 0;
};

class Derived : public Base {
	// 没有实现 func()
};

// Derived d; // ❌ 依然不能实例化
```

## ⚠️ Pitfalls（高频错误）

- 有纯虚函数的类就是抽象类，**不能直接创建对象**
- 纯虚函数写法是：

```cpp
virtual void func() = 0;
```

不要漏掉 `= 0`

- 子类如果没有实现所有继承来的纯虚函数，那它自己也还是抽象类
- 抽象类不是“完全不能写实现”，它**可以有普通成员函数、数据成员、构造函数、析构函数**
- 纯虚函数和普通虚函数不是一回事：
    - 普通虚函数：可以有默认实现
    - 纯虚函数：强调“必须由合适的具体类提供实现”
- 抽象类通常通过**基类指针 / 引用**来使用，而不是直接定义对象
- 如果抽象类要作为多态基类使用，析构函数通常应写成虚析构
- 不要把“抽象类”和“接口”完全等同理解，C++ 抽象类通常比纯接口更灵活
- 抽象类的重点是“定义规范”，不是“减少代码量”

## 🚀 Performance / Tips（性能优化）

- 抽象类最常用标准写法：

```cpp
class Base {
public:
	virtual void func() = 0;
	virtual ~Base() = default;
};
```

- 高频记忆规则：

```cpp
纯虚函数 = 0
含纯虚函数的类 = 抽象类
抽象类不能实例化
子类实现后才能变成具体类
```

- 如果某个基类的职责就是“规定接口”，那通常就可以考虑抽象类
- 如果你希望子类“必须实现某个函数”，那就把它设计成纯虚函数
- 工程 / 面试里抽象类经常和这些主题一起出现：

```cpp
继承
多态
virtual
override
虚析构
接口设计
```

- 设计思路常常是：

```cpp
抽象类负责定义规则
子类负责实现细节
外部通过基类指针 / 引用统一调用
```

## 🧪 Common Scenarios（常见使用场景）

- **动物都会叫，但叫法不同**：`Animal` 抽象类 + `Dog` / `Cat`
- **图形都能画，但具体画法不同**：`Shape` 抽象类 + `Circle` / `Rectangle`
- **员工都要工作，但职责不同**：`Employee` 抽象类 + `Manager` / `Engineer`
- **统一接口处理多种对象**：抽象类做父类规范
- **框架 / 项目里先定义能力，再让子类实现**：抽象类非常常见

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Animal {
public:
	virtual void speak() = 0;
	virtual ~Animal() = default;
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "Dog bark" << '\n';
	}
};

class Cat : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\n';
	}
};

void makeSpeak(Animal& a) {
	a.speak();
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Dog d;
	Cat c;
	makeSpeak(d);
	makeSpeak(c);

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 抽象类的核心不是“记住它不能实例化”，而是把 **统一接口、强制规范、子类实现、基类统一调用** 这几件事串起来，让继承体系更清楚、多态设计更自然。