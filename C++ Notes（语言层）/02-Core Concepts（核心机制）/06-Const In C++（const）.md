#cpp #basics #const 

## ⚡ TL;DR（快速决策）

- `const` 的核心作用：**限制修改**，让值、参数、指针、成员函数更安全
- 能明确“不该改”的地方，就优先考虑加 `const`
- 入门阶段最重要的是分清 4 类：**常量变量、const 引用、const 指针、const 成员函数**
- `const` 最容易错的不是语法，而是：**到底谁不能改、`*` 两边的含义、函数参数为什么常写 `const T&`**
- 现代 C++ 里，`const` 不是“可有可无的修饰”，而是代码可读性和安全性的基础习惯

## 🧩 Core Idea（核心本质）

- **本质**：`const` 用来表达“这个对象 / 这个访问方式 / 这个接口不应该修改数据”
- **关键机制**：编译器会在语法层面阻止一些不合法的修改操作
- **核心价值**：减少误修改、增强可读性、让接口语义更清晰
- **一句理解**：`const` 不只是“常量”，更像是在代码里写下“这里别乱动”

## 🔧 Usage Patterns（可复用代码模板）

### 1. const 常量变量

```cpp
const int n = 100;
// n = 200; // ❌ 不能修改
```

### 2. const 修饰引用

```cpp
int x = 10;
const int& r = x;
cout << r << '\n';
// r = 20; // ❌ 不能通过 r 修改 x
```

### 3. const 修饰函数参数

```cpp
void print(const string& s) {
	cout << s << '\n';
}
```

### 4. const 修饰返回值接收对象

```cpp
const string s = "hello";
cout << s.size() << '\n';
```

### 5. 指向常量的指针

```cpp
int x = 10;
int y = 20;
const int* p = &x;
// *p = 30; // ❌ 不能通过 p 改值
p = &y;      // ✅ 可以改指向
```

### 6. 常量指针

```cpp
int x = 10;
int y = 20;
int* const p = &x;
*p = 30;     // ✅ 可以改值
// p = &y;   // ❌ 不能改指向
```

### 7. 指向常量的常量指针

```cpp
int x = 10;
const int* const p = &x;
// *p = 20; // ❌
// p = ...; // ❌
```

### 8. const 成员函数

```cpp
class Person {
public:
	string name;
	void show() const {
		cout << name << '\n';
	}
};
```

### 9. auto 和 const 一起用

```cpp
const int x = 10;
auto a = x;        // a 是 int
const auto b = x;  // b 是 const int
```

## ⚠️ Pitfalls（高频错误）

- `const` 修饰谁，要看它离谁更近，不要只背表面形式
- `const int* p` 和 `int* const p` 不是一回事
- `const` 引用不能改值，但它绑定的原对象本身未必真的不可改
- `const` 对象必须初始化，不能先定义后再赋值
- 函数参数写成 `const T&`，是“只读 + 避免拷贝”，不是多此一举
- `auto` 会忽略顶层 `const`，别误以为 `auto` 一定保留原 const 属性
- `const` 成员函数里，不能随意修改成员变量
- 不要把 `#define PI 3.14` 和 `const double PI = 3.14;` 当成同一种东西
- `const` 不是运行时保护，而主要是编译期约束
- 不要把“对象是 const”和“指针 / 引用本身是 const”混为一谈

## 🚀 Performance / Tips（性能优化）

- 大对象传参时，优先写：

```cpp
const string& s
const vector<int>& v
```

这样既避免拷贝，又表达“不会修改参数”

- 只读遍历容器时，优先写：

```cpp
for (const auto& x : vec) {
	cout << x << '\n';
}
```

- 能确定不会改的局部量，也可以主动加 `const`：

```cpp
const int n = nums.size();
```

- 高频记忆规则：

```cpp
const T* p   // 不能通过 p 改值
T* const p   // p 不能改指向
const T& r   // 只读引用
void f() const // 成员函数不改对象状态
```

- 刷题 / 工作里最常见、最值得养成的 const 写法：

```cpp
const int MOD = 1e9 + 7;
const string& s
const auto& x
bool empty() const
```

## 🧪 Common Scenarios（常见使用场景）

- **定义不会变的常量**：`const int MOD = 1e9 + 7;`
- **函数只读参数**：`const string& s`
- **遍历但不修改元素**：`const auto& x`
- **类的只读接口**：`size() const`、`empty() const`
- **防止误改指针指向的数据**：`const int* p`
- **表达接口语义**：告诉别人“这个函数会读，但不会改”
- **减少对象拷贝成本**：大对象参数传 `const 引用`

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

void print(const string& s) {
	cout << s << '\n';
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	const int n = 10;
	string name = "YAN";
	const string& ref = name;

	print(ref);
	cout << n << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 里 `const` 的核心不是“会写一个关键字”，而是把 **哪些数据不能改、哪些接口不该改、哪些参数只读更合理** 这几件事表达清楚，让代码更安全、更清晰、更可靠。