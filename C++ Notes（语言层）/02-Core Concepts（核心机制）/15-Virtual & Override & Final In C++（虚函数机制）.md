#cpp #virtual #override #final #oop

## ⚡ TL;DR（快速决策）

- `virtual`：告诉编译器“这个函数支持动态绑定”
- `override`：告诉编译器“我就是在重写基类虚函数”
- `final`：告诉编译器“这里到此为止，别再重写 / 别再继承了”
- 入门阶段最重要的是先分清：**谁负责开启虚函数机制，谁负责检查重写，谁负责禁止继续扩展**
- 这三个关键字经常一起出现，是 C++ 动态多态的核心语法组合
- 最容易错的不是背定义，而是：**漏写 `virtual`、误以为重名就是重写、没写 `override` 导致签名写错、搞不清 `final` 限制的是函数还是类**

## 🧩 Core Idea（核心本质）

- **本质**：`virtual`、`override`、`final` 这套机制是在控制“继承体系中的函数到底怎么绑定和扩展”
- **关键机制**：
    - `virtual`：开启动态绑定
    - `override`：明确声明重写并让编译器帮你检查
    - `final`：阻止继续继承或继续重写
- **核心价值**：让多态更安全、更清晰、更可控
- **一句理解**：`virtual` 负责“打开门”，`override` 负责“确认你走的是这扇门”，`final` 负责“把门关上”

## 🔧 Usage Patterns（可复用代码模板）

### 1. `virtual` 基础写法

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "Animal speak" << '\\n';
	}
};
```

### 2. `override` 基础写法

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

### 3. `final` 修饰虚函数

```cpp
class Base {
public:
	virtual void show() final {
		cout << "Base show" << '\\n';
	}
};

class Derived : public Base {
	// void show() override {} // ❌ 不能再重写
};
```

### 4. `final` 修饰类

```cpp
class Base final {
};

// class Derived : public Base {}; // ❌ 不能继承 final 类
```

### 5. 基类指针触发虚函数机制

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "Animal speak" << '\\n';
	}
};

class Cat : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\\n';
	}
};

Animal* p = new Cat();
p->speak();
```

### 6. 基类引用触发虚函数机制

```cpp
Cat c;
Animal& ref = c;
ref.speak();
```

### 7. 漏掉 `virtual` 的对比

```cpp
class Base {
public:
	void show() {
		cout << "Base" << '\\n';
	}
};

class Derived : public Base {
public:
	void show() {
		cout << "Derived" << '\\n';
	}
};

Base* p = new Derived();
p->show();   // 输出 Base
```

### 8. `override` 帮你检查签名

```cpp
class Base {
public:
	virtual void func(int x) {}
};

class Derived : public Base {
public:
	void func(int x) override {}
};
```

### 9. 虚析构函数

```cpp
class Base {
public:
	virtual ~Base() = default;
};

class Derived : public Base {
public:
	~Derived() override = default;
};
```

### 10. 抽象类中的虚函数

```cpp
class Shape {
public:
	virtual void draw() = 0;
	virtual ~Shape() = default;
};
```

## ⚠️ Pitfalls（高频错误）

- 有继承不代表自动多态，**必须有 `virtual`** 才会动态绑定
- 函数名相同不一定就是重写，参数列表不同可能只是重载 / 隐藏
- `override` 不是开启虚函数机制的关键字，它是“检查你是否真的重写成功”
- 如果签名写错，`override` 会直接帮你报错，这正是它的重要价值
- `final` 可以修饰**函数**，也可以修饰**类**，两者限制对象不同
- 基类析构函数如果不是虚函数，用基类指针删除派生类对象可能出问题
- 值传递不会触发你想要的动态多态，常见问题是**对象切片**
- 不要把 `virtual` 理解成“所有地方都会动态绑定”，通常要通过**基类指针 / 引用**调用才体现出来
- `override` 强烈建议写，但 `virtual` 在派生类重写处不是必须重复写
- `final` 是限制扩展工具，不是性能优化魔法按钮

## 🚀 Performance / Tips（性能优化）

- 最常用标准写法：

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
override  // 检查重写
final     // 禁止继续扩展
```

- 如果一个函数设计上就是给子类改写的，基类里优先考虑 `virtual`
- 如果派生类明确在重写，强烈建议总是写上 `override`
- 如果某个类 / 某个虚函数不希望再被扩展，可以考虑 `final`
- 工程 / 面试里这三个词经常和这些主题一起出现：

```cpp
继承
多态
抽象类
虚析构
对象切片
动态绑定
```

## 🧪 Common Scenarios（常见使用场景）

- **统一接口，不同子类不同实现**：`virtual`
- **防止重写签名写错**：`override`
- **禁止某个关键函数继续被改写**：函数级 `final`
- **禁止某个类继续派生**：类级 `final`
- **面向对象层次设计**：基类给接口，子类做实现
- **资源类安全销毁**：虚析构函数
- **抽象类定义规范**：纯虚函数 + `override`

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

class Cat final : public Animal {
public:
	void speak() override {
		cout << "Cat meow" << '\\n';
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Animal* p1 = new Dog();
	Animal* p2 = new Cat();

	p1->speak();
	p2->speak();

	delete p1;
	delete p2;
	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 虚函数机制的核心不是“背 3 个关键字”，而是把 **谁开启动态绑定、谁负责安全重写、谁负责限制扩展** 这几件事理顺，让继承体系既灵活又可控。