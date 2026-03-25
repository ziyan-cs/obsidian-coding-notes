#cpp #polymorphism #virtual #oop

## ⚡ TL;DR（快速决策）

- **多态**：同样的调用方式，在不同对象上表现出不同结果
- 入门阶段最重要的是先分清：**静态多态** 和 **动态多态**
- C++ 面向对象里常说的“多态”，通常重点指的是：**虚函数 + 继承 + 基类指针/引用**
- 最常见场景：**基类统一接口，子类各自实现**
- 最容易错的不是概念，而是：**忘了写 `virtual`、以为有继承就自动多态、值传递导致对象切片、该写 `override` 却没写**
- 如果题目涉及“同一个函数名，面对不同子类对象表现不同”，就要立刻想到多态

## 🧩 Core Idea（核心本质）

- **本质**：多态是在统一接口下，让不同类型对象展现不同行为
- **关键机制**：通过基类指针 / 引用调用虚函数时，程序会在运行时决定真正执行哪个子类版本
- **核心价值**：提升扩展性，让代码面向抽象编程，而不是写死某个具体类
- **一句理解**：你面对的是“同一套接口”，但背后对象可以“各演各的角色”

## 🔧 Usage Patterns（可复用代码模板）

### 1. 没有多态的普通同名函数

```cpp
class Animal {
public:
	void speak() {
		cout << "Animal speak" << '\\n';
	}
};

class Dog : public Animal {
public:
	void speak() {
		cout << "Dog bark" << '\\n';
	}
};
```

### 2. 虚函数基础

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "Animal speak" << '\\n';
	}
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "Dog bark" << '\\n';
	}
};
```

### 3. 基类指针触发动态多态

```cpp
Animal* p = new Dog();
p->speak();   // 输出 Dog bark
```

### 4. 基类引用触发动态多态

```cpp
Dog d;
Animal& ref = d;
ref.speak();
```

### 5. 多个子类共享同一接口

```cpp
class Cat : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\\n';
	}
};
```

### 6. 多态函数参数

```cpp
void makeSpeak(Animal& a) {
	a.speak();
}
```

### 7. 纯虚函数

```cpp
class Animal {
public:
	virtual void speak() = 0;
};
```

### 8. 抽象类

```cpp
class Animal {
public:
	virtual void speak() = 0;
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "Dog bark" << '\\n';
	}
};
```

- 含有纯虚函数的类叫**抽象类**，不能直接实例化

### 9. 虚析构函数

```cpp
class Base {
public:
	virtual ~Base() {
		cout << "Base destruct" << '\\n';
	}
};

class Derived : public Base {
public:
	~Derived() override {
		cout << "Derived destruct" << '\\n';
	}
};
```

### 10. 静态多态（了解）

```cpp
void print(int x) {}
void print(double x) {}
```

- 函数重载、模板通常归到静态多态

## ⚠️ Pitfalls（高频错误）

- 只有“继承”还不够，想要运行时多态，必须有 **虚函数**
- 只有“虚函数”还不够，通常还需要通过**基类指针或基类引用**调用
- `override` 不是必须语法，但强烈建议写，能帮你防止误写签名
- 如果基类析构函数不是虚函数，用基类指针删除派生类对象可能出问题
- 多态常见触发条件要记住：

```cpp
继承
虚函数
基类指针 / 引用
```

- 值传递会导致**对象切片**，从而丢失多态行为
- 纯虚函数类不能直接创建对象
- 不要把“函数重载”误当成这里讲的面向对象动态多态
- 基类指针能指向派生类对象，但不是所有成员都能直接通过基类指针访问
- 多态的重点是“接口统一，行为可变”，不是“到处用指针”

## 🚀 Performance / Tips（性能优化）

- 动态多态最常用标准写法：

```cpp
class Base {
public:
	virtual void func() {}
	virtual ~Base() = default;
};

class Derived : public Base {
public:
	void func() override {}
};
```

- 高频记忆规则：

```cpp
virtual   // 开启动态绑定
override  // 明确重写
= 0       // 纯虚函数
```

- 如果你只是想“同名函数处理不同参数”，那通常是：

```cpp
函数重载
```

不是这里强调的运行时多态

- 构造面向对象层次时，经常把“公共接口”放基类，把“具体实现”交给派生类
- 工程 / 面试里多态经常和这些词一起出现：

```cpp
继承
虚函数
抽象类
override
对象切片
虚析构
```

## 🧪 Common Scenarios（常见使用场景）

- **动物叫声不同，但统一叫 `speak()`**：典型多态
- **图形都能 `draw()`，但绘制方式不同**：Shape / Circle / Rectangle
- **员工都能 `work()`，但实现不同**：Employee / Manager / Engineer
- **统一接口处理不同子类对象**：减少 `if-else` 分支
- **抽象类定义规范，子类负责实现细节**：面向抽象编程

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

class Animal {
public:
	virtual void speak() {
		cout << "Animal speak" << '\\n';
	}

	virtual ~Animal() = default;
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "Dog bark" << '\\n';
	}
};

class Cat : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\\n';
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

👉 C++ 多态的核心不是“记住几个关键字”，而是把 **统一接口、动态绑定、不同子类各自实现** 这几件事串起来，让程序既能面向抽象写，又能对不同对象做出不同响应。