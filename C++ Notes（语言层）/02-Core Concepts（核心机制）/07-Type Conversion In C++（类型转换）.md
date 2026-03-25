#cpp #basics #type-conversion #cast 

## ⚡ TL;DR（快速决策）

- C++ 类型转换的核心：**把一种类型的值转成另一种类型来使用**
- 入门阶段最重要的是先分清：**隐式转换** 和 **显式转换**
- 现代 C++ 里，手动转换优先想到 `static_cast`，不要一上来就乱用旧式强转
- 类型转换最容易错的不是“会不会写”，而是：**会不会丢精度、会不会改变含义、会不会产生未定义 / 不安全行为**
- 如果题目涉及整数除法、字符转数字、浮点转整数、基类子类转换，这一节就很关键

## 🧩 Core Idea（核心本质）

- **本质**：类型转换就是让同一个值以另一种类型的规则被解释或使用
- **关键机制**：转换后，值的表示方式、可保留的信息、参与运算的方式都可能变化
- **核心价值**：让不同类型之间可以协同工作，但也因此容易带来精度损失和语义错误
- **一句理解**：类型转换不是“换个写法”，而是可能真的改变值的表现和结果

## 🔧 Usage Patterns（可复用代码模板）

### 1. 隐式转换

```cpp
int a = 10;
double b = a;   // int -> double
```

### 2. 整数提升 / 混合运算

```cpp
int a = 5;
double b = 2.5;
double c = a + b;
```

### 3. 整数除法转浮点结果

```cpp
int a = 5, b = 2;
double ans = static_cast<double>(a) / b;
```

### 4. 浮点转整数

```cpp
double x = 3.9;
int y = static_cast<int>(x);  // 3
```

### 5. 字符和 ASCII

```cpp
char c = 'A';
int code = static_cast<int>(c);
```

### 6. 数字字符转整数

```cpp
char c = '7';
int x = c - '0';
```

### 7. `static_cast`

```cpp
double x = 3.14;
int y = static_cast<int>(x);
```

### 8. `const_cast`

```cpp
const int x = 10;
const int* p = &x;
int* q = const_cast<int*>(p);
```

### 9. `dynamic_cast`（多态场景）

```cpp
class Base { public: virtual ~Base() {} };
class Derived : public Base {};

Base* p = new Derived();
Derived* d = dynamic_cast<Derived*>(p);
```

### 10. `reinterpret_cast`（了解）

```cpp
int x = 65;
int* p = &x;
long long addr = reinterpret_cast<long long>(p);
```

### 11. C 风格强转（不推荐优先使用）

```cpp
double x = 3.14;
int y = (int)x;
```

## ⚠️ Pitfalls（高频错误）

- `int / int` 的结果还是 `int`，不是自动变小数
- `double -> int` 会直接截断小数部分，不是四舍五入
- `char` 和数字字符不是一回事：`'7'` 不是整数 `7`
- `c - '0'` 只适合数字字符 `'0' ~ '9'`
- 隐式转换虽然方便，但有时会悄悄丢信息
- 窄化转换最容易出问题，比如：

```cpp
double x = 1e9;
int y = x;
```

- 不要把 `static_cast` 当成万能安全转换，它也不能解决所有类型关系问题
- `dynamic_cast` 主要用于有继承、多态的场景，不是普通变量转换工具
- `const_cast` 不是“解除一切限制”的魔法，乱改真正的常量对象会出问题
- `reinterpret_cast` 风险最高，初学阶段只要知道它“很底层、很危险”就够了

## 🚀 Performance / Tips（性能优化）

- 手动转换优先写：

```cpp
static_cast<T>(value)
```

因为它更清晰，也更符合现代 C++ 风格

- 高频刷题转换写法记住这几个：

```cpp
static_cast<double>(a) / b
static_cast<int>(x)
c - '0'
```

- 如果只是做字符统计、数字字符处理，常用的是：

```cpp
char -> int
int -> char
```

例如：

```cpp
int x = c - '0';
char c = '0' + x;
```

- 高频记忆规则：

```cpp
隐式转换：编译器自动做
显式转换：程序员手动写
static_cast：最常用
const_cast：改 const 属性
dynamic_cast：继承多态
reinterpret_cast：底层危险
```

- 题目里一旦看到：

```cpp
除法结果不对
字符转数字
浮点变整数
基类指针转子类
```

就要立刻想到“这里可能是类型转换问题”

## 🧪 Common Scenarios（常见使用场景）

- **让整数除法得到小数结果**：`static_cast<double>(a) / b`
- **把 `double` 转成 `int`**：`static_cast<int>(x)`
- **把字符 `'7'` 转成整数 `7`**：`c - '0'`
- **把整数 7 转成字符 `'7'`**：`'0' + 7`
- **读取字符的编码值**：`static_cast<int>(c)`
- **继承体系下安全向下转型**：`dynamic_cast`
- **去掉引用 / 指针上的 const 属性**：`const_cast`（谨慎）
- **底层地址级别解释**：`reinterpret_cast`（了解即可）

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int a = 5, b = 2;
	double ans = static_cast<double>(a) / b;

	char c = '7';
	int x = c - '0';

	double y = 3.9;
	int z = static_cast<int>(y);

	cout << ans << '\n';
	cout << x << '\n';
	cout << z << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 类型转换的核心不是“会写几个 cast”，而是把 **什么时候会自动转换、什么时候要手动转换、转换后会不会丢信息或改变语义** 这几件事想清楚，让不同类型之间的协作既正确又安全。