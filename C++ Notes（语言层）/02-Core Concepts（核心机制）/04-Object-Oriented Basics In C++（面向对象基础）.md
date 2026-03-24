#cpp #核心机制 #面对对象
## 一页速览

- **面向对象 3 个核心词**：封装、继承、多态
- **类**：对象的模板
- **对象**：类创建出来的实例
- **封装**：数据和方法放在一起
- **继承**：子类复用父类能力
- **多态**：同一调用，不同表现
- **最常错**：类和对象混淆、继承写法不熟、忘记 `virtual`、不会区分重写和重载

## 一、先总后分：这一节到底学什么

### 1. 面向对象解决什么问题

- 不再只把程序看成一堆零散函数
- 而是把现实中的“对象”抽象成程序中的类和对象
- 让代码更清晰、更容易复用和扩展

### 2. 面向对象最核心看什么

- **封装**：把数据和方法放一起
- **继承**：在已有类基础上扩展
- **多态**：不同对象对同一行为有不同实现

### 3. 当前阶段先抓主线

- 分清类和对象
- 理解封装为什么重要
- 看懂最基础的继承写法
- 对多态先建立正确印象

---

## 二、类和对象

### 1. 类是什么

- 类是一个“模板”
- 规定对象应该有哪些数据和行为

```cpp
class Person {
public:
	string name;
	void sayHello() {
		cout << "Hello" << '\\n';
	}
};
```

### 2. 对象是什么

- 对象是类创建出来的实例

```cpp
Person p;
p.name = "Tom";
p.sayHello();
```

### 3. 速记

- `Person`：类
- `p`：对象
- `p.sayHello()`：对象调用成员函数

---

## 三、封装

### 1. 什么是封装

- 把数据和操作数据的方法放在一起
- 对外隐藏不该直接改的数据

### 2. 为什么要封装

- 防止外部乱改数据
- 让类自己保证数据合法
- 让代码职责更清楚

### 3. 典型写法

```cpp
class Person {
private:
	int age;

public:
	void setAge(int a) {
		if (a >= 0) age = a;
	}

	int getAge() {
		return age;
	}
};
```

### 4. 当前阶段怎么理解

- `private`：保护数据
- `public`：暴露必要操作
- 这就是最基础的封装

---

## 四、继承

### 1. 什么是继承

- 子类复用父类已有成员
- 再加上自己的新功能

```cpp
class Animal {
public:
	void eat() {
		cout << "eat" << '\\n';
	}
};

class Dog : public Animal {
public:
	void bark() {
		cout << "wang" << '\\n';
	}
};
```

### 2. 怎么理解

```cpp
Dog d;
d.eat();
d.bark();
```

- `Dog` 继承了 `Animal` 的能力
- 自己也可以增加新行为

### 3. 当前阶段先记

- `class 子类 : public 父类`
- 子类可以用父类公开的成员

---

## 五、多态

### 1. 什么是多态

- 同样是“调用一个函数”
- 不同对象可以表现出不同结果

### 2. 没有 `virtual` 时

```cpp
class Animal {
public:
	void speak() {
		cout << "animal" << '\\n';
	}
};

class Dog : public Animal {
public:
	void speak() {
		cout << "dog" << '\\n';
	}
};
```

- 这时只是“同名覆盖现象”
- 还不是真正意义上的运行时多态

### 3. 有 `virtual` 时

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "animal" << '\\n';
	}
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "dog" << '\\n';
	}
};
```

```cpp
Animal* p = new Dog();
p->speak(); // dog
delete p;
```

### 4. 当前阶段先记结论

- **多态常和继承一起出现**
- **父类函数想支持多态，通常要写 `virtual`**
- **子类重写时推荐写 `override`**

---

## 六、重写 `override` 和重载 `overload`

### 1. 重写 `override`

- 发生在继承关系中
- 子类重写父类的虚函数

```cpp
class Base {
public:
	virtual void show() {}
};

class Derived : public Base {
public:
	void show() override {}
};
```

### 2. 重载 `overload`

- 同一个作用域里
- 函数名相同，参数列表不同

```cpp
void func(int x);
void func(double x);
```

### 3. 速记

- `override`：父子类之间
- `overload`：同名不同参数

---

## 七、构造函数与对象创建

### 1. 对象创建时会调用构造函数

```cpp
class Person {
public:
	Person() {
		cout << "构造" << '\\n';
	}
};
```

### 2. 子类对象创建时

- 先构造父类部分
- 再构造子类部分

### 3. 当前阶段记结论

- 构造函数负责初始化对象
- 面向对象里“对象一创建就有效”很重要

---

## 八、对象之间的关系：组合先有印象

### 1. 什么是组合

- 一个类把另一个类的对象作为成员
- 表示“有一个”的关系

```cpp
class Engine {
public:
	void start() {
		cout << "engine start" << '\\n';
	}
};

class Car {
public:
	Engine engine;
};
```

### 2. 怎么理解

- `Car` 不是 `Engine`
- 但 `Car` 里“有一个” `Engine`

### 3. 当前阶段结论

- **继承** 更像“是一个”
- **组合** 更像“有一个”

---

## 九、面向对象当前阶段最常见写法

### 1. 封装小类

```cpp
class Counter {
private:
	int cnt;

public:
	Counter() {
		cnt = 0;
	}

	void addOne() {
		++cnt;
	}

	int get() {
		return cnt;
	}
};
```

### 2. 基础继承

```cpp
class Animal {
public:
	void eat() {
		cout << "eat" << '\\n';
	}
};

class Cat : public Animal {
public:
	void meow() {
		cout << "meow" << '\\n';
	}
};
```

### 3. 基础多态

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "animal" << '\\n';
	}
};

class Cat : public Animal {
public:
	void speak() override {
		cout << "cat" << '\\n';
	}
};
```

---

## 十、现代 C++ 的基础习惯

### 1. 重写虚函数时加 `override`

- 防止“以为重写了，实际没重写”

### 2. 能封装就别把成员全写 `public`

- 优先让类自己管理状态

### 3. 先把类职责写清楚

- 一个类尽量只表达一个明确对象

### 4. 当前阶段先别把面向对象学得太散

- 先稳住封装、继承、多态主线
- 后面再补初始化列表、析构、拷贝控制、抽象类等

---

## 十一、高频易错点

1. 不要把类和对象混为一谈
2. `public` 继承写法别漏
3. 没有 `virtual`，很多场景下不会产生真正的运行时多态
4. 重写时优先写 `override`
5. 封装不是把所有成员都公开
6. 继承表示“是一个”，组合表示“有一个”
7. 面向对象不是单纯把函数塞进类里
8. 当前阶段先抓主线，不要一开始陷进太多细节

---

## 十二、最简模板

### 封装模板

```cpp
class Person {
private:
	int age;

public:
	void setAge(int a) {
		if (a >= 0) age = a;
	}

	int getAge() {
		return age;
	}
};
```

### 继承模板

```cpp
class Animal {
public:
	void eat() {
		cout << "eat" << '\\n';
	}
};

class Dog : public Animal {
public:
	void bark() {
		cout << "wang" << '\\n';
	}
};
```

### 多态模板

```cpp
class Animal {
public:
	virtual void speak() {
		cout << "animal" << '\\n';
	}
};

class Dog : public Animal {
public:
	void speak() override {
		cout << "dog" << '\\n';
	}
};
```

<aside> 📌

这一节真正要掌握的，不只是背“封装、继承、多态”这几个词，而是看到代码时能立刻判断：**这里是在定义对象模型、复用父类能力，还是在通过虚函数实现多态。**

</aside>