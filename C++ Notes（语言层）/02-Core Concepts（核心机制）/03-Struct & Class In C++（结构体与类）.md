#cpp #核心机制 #结构体 #类
## 一页速览

- **`struct`**：默认成员权限是 `public`
- **`class`**：默认成员权限是 `private`
- **对象**：由类 / 结构体创建出来的实例
- **成员访问**：对象用 `.`，对象指针用 `->`
- **构造函数**：对象创建时自动调用
- **核心思想**：把数据和操作数据的函数放在一起
- **最常错**：默认权限、构造函数写法、`.` 和 `->` 混淆

## 一、先总后分：这一节到底学什么

### 1. 结构体解决什么问题

- 把多个相关数据打包成一个整体
- 适合表达“一个对象的信息”

### 2. 类解决什么问题

- 不只存数据，还把操作数据的方法一起封装
- 为后面的面向对象打基础

### 3. 当前阶段先抓主线

- 会定义 `struct` 和 `class`
- 会创建对象并访问成员
- 会写最基础的构造函数
- 会分清 `public` 和 `private`

---

## 二、结构体 `struct`

### 1. 结构体是什么

- 自定义类型
- 用来组织一组相关数据

```cpp
struct Student {
	string name;
	int age;
	double score;
};
```

### 2. 怎么使用

```cpp
Student s1;
s1.name = "Tom";
s1.age = 18;
s1.score = 95.5;
```

### 3. 速记

- `Student`：类型
- `s1`：对象 / 变量
- `s1.name`：成员访问

---

## 三、类 `class`

### 1. 类是什么

- 也是自定义类型
- 但更强调“数据 + 行为”

```cpp
class Student {
public:
	string name;
	int age;

	void show() {
		cout << name << " " << age << '\\n';
	}
};
```

### 2. 怎么使用

```cpp
Student s;
s.name = "Alice";
s.age = 20;
s.show();
```

### 3. 当前阶段怎么理解

- `struct`：更像“数据包”
- `class`：更像“完整对象模型”

---

## 四、`struct` 和 `class` 的区别

|对比项|`struct`|`class`|
|---|---|---|
|默认成员权限|`public`|`private`|
|默认继承权限|`public`|`private`|
|常见用途|简单数据组织|封装对象建模|

### 考试 / 入门先记结论

- 语法能力上两者非常像
- 真正最常考的是：**默认权限不同**

---

## 五、访问权限：`public` 和 `private`

### 1. `public`

- 类外可以直接访问

### 2. `private`

- 类外不能直接访问
- 用来保护数据

```cpp
class Person {
public:
	string name;

private:
	int age;
};
```

### 3. 速记

- `public`：外面能直接用
- `private`：只能类内部处理

---

## 六、成员函数

### 1. 什么是成员函数

- 写在类里的函数
- 用来操作类中的成员数据

```cpp
class Person {
public:
	string name;
	int age;

	void show() {
		cout << name << " " << age << '\\n';
	}
};
```

### 2. 怎么调用

```cpp
Person p;
p.name = "Bob";
p.age = 19;
p.show();
```

### 3. `this` 先有印象

- 成员函数默认就是在操作“当前对象”
- `this` 指向当前对象
- 当前阶段知道这点就够了

---

## 七、构造函数

### 1. 构造函数是什么

- 对象创建时自动调用
- 常用于初始化成员

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

### 2. 带参数构造函数

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

```cpp
Person p("Tom", 20);
```

### 3. 必记特征

- 函数名和类名相同
- 没有返回类型
- 创建对象时自动执行

---

## 八、封装：类最核心的思想

### 1. 什么是封装

- 把数据和方法放在一起
- 对外隐藏不该直接改的数据

### 2. 典型写法

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

### 3. 为什么这样写

- 防止外部乱改数据
- 让类自己保证数据合法

---

## 九、对象、对象指针与访问方式

### 1. 普通对象

```cpp
Person p;
p.show();
```

- 用 `.` 访问成员

### 2. 对象指针

```cpp
Person* p = new Person();
p->show();
delete p;
```

- 用 `->` 访问成员

### 3. 速记

- 对象：`.`
- 指针：`->`

---

## 十、当前阶段最常见写法

### 1. 简单数据组织用 `struct`

```cpp
struct Point {
	int x;
	int y;
};
```

### 2. 需要封装行为用 `class`

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

---

## 十一、现代 C++ 的基础习惯

### 1. 不要图省事把成员全写成 `public`

- 能封装就尽量封装

### 2. 初始化优先交给构造函数

- 让对象一创建就处于有效状态

### 3. 一个类尽量表达一个清晰对象

- 不要一上来塞太多职责

### 4. 后面还会继续补

- 初始化列表
- 拷贝构造
- 析构函数
- 继承与多态

---

## 十二、高频易错点

1. `struct` 默认是 `public`，`class` 默认是 `private`
2. 构造函数没有返回类型
3. 类定义末尾别漏分号 `;`
4. 对象访问成员用 `.`，指针访问成员用 `->`
5. `private` 成员不能在类外直接访问
6. 不要把“类”和“对象”当成一回事
7. 构造函数会在对象创建时自动调用
8. 简单数据打包和封装建模要分场景

---

## 十三、最简模板

### 结构体模板

```cpp
struct Student {
	string name;
	int age;
};
```

### 类模板

```cpp
class Person {
public:
	string name;
	int age;

	void show() {
		cout << name << " " << age << '\\n';
	}
};
```

### 带构造函数模板

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

<aside> 📌

这一节真正要掌握的，不只是会写 `struct` 和 `class`，而是看到代码时能立刻判断：**这是类型还是对象、成员能不能直接访问、对象创建时是怎么初始化的。**

</aside>