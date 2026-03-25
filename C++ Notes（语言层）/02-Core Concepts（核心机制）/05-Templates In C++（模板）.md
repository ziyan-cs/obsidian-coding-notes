#cpp #templates #generic-programming

## ⚡ TL;DR（快速决策）

- 逻辑相同，只是类型不同 → 先想函数模板
- 需要定义通用数据结构 / 类 → 先想类模板
- 想让代码“对多种类型复用”而不是复制粘贴 → 模板就是核心工具
- 模板题真正关键通常不在语法，而在“类型参数到底抽象了什么”
- 一旦模板参数、返回类型、实例化时机没想清楚，报错会非常绕

## 🧩 Core Idea（核心本质）

- **本质**：模板是在编译期把“类型”也当成参数，让一份代码生成多份类型版本
- **关键机制**：通过类型参数化，把原本依赖具体类型的逻辑抽象成通用逻辑
- **核心价值**：减少重复代码，增强泛型能力，是 STL、容器、算法和现代 C++ 复用能力的基础

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础函数模板

```cpp
template <typename T>
T add(T a, T b) {
	return a + b;
}
```

### 2. 多类型参数模板

```cpp
template <typename T1, typename T2>
auto add(T1 a, T2 b) {
	return a + b;
}
```

### 3. 最基础类模板

```cpp
template <typename T>
class Box {
private:
	T value;

public:
	Box(T v) : value(v) {}
	T get() const {
		return value;
	}
};
```

### 4. 模板函数配合容器

```cpp
template <typename T>
void print_vector(const vector<T>& nums) {
	for (const T& x : nums) {
		cout << x << ' ';
	}
	cout << '\n';
}
```

### 5. 非类型模板参数先有印象

```cpp
template <typename T, int N>
struct Array {
	T data[N];
};
```

### 6. 类模板成员函数

```cpp
template <typename T>
class Counter {
private:
	T value;

public:
	Counter(T v = 0) : value(v) {}
	void add(T x) {
		value += x;
	}
	T get() const {
		return value;
	}
};
```

### 7. 显式指定模板参数

```cpp
cout << add<int>(1, 2) << '\n';
```

### 8. 模板是 STL 的基础

```cpp
vector<int> a;
vector<string> b;
sort(a.begin(), a.end());
```

## ⚠️ Pitfalls（高频错误）

- 模板不是运行期多态，它是编译期生成代码
- 模板参数推导不出来时，需要显式写出模板参数
- 函数模板和普通函数同时存在时，匹配规则可能和直觉不一样
- 类模板在使用时通常要写具体类型，如 `Box<int>`
- 模板报错常常很长，真正问题通常在最前面几行
- 把“所有东西都模板化”会让代码复杂度失控
- 模板代码依赖操作符或接口时，要先确认类型真的支持那些操作
- 模板能复用逻辑，但不能替代清晰的抽象设计

## 🚀 Performance / Tips（性能优化）

- 模板真正高频的价值是：

```cpp
复用逻辑
让类型参数化
支撑 STL 风格代码
```

- 写模板前优先先想清：

```cpp
哪些东西在变化
哪些逻辑不变
```

- 简单泛型需求优先函数模板，结构抽象需求再用类模板
- 模板参数命名尽量有语义，不一定永远只写 `T`
- 如果只是为了少写几行，不要过度模板化普通业务代码

## 🧪 Common Scenarios（常见使用场景）

- **通用函数**：交换、比较、求和、打印
- **通用容器 / 包装类**：`Box<T>`、节点、缓存结构
- **STL 使用理解**：`vector<T>`、`sort`、`pair<T1, T2>`
- **算法泛型化**：让同一逻辑适配多种类型
- **面试基础问答**：函数模板、类模板、模板参数推导、编译期多态
- **现代 C++ 阅读**：模板是理解 STL 和泛型代码的前提

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

template <typename T>
T add(T a, T b) {
	return a + b;
}

template <typename T>
class Box {
private:
	T value;

public:
	Box(T v) : value(v) {}
	T get() const {
		return value;
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	cout << add<int>(1, 2) << '\n';
	Box<string> box("hello");
	cout << box.get() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 模板的核心不是“会写 `template` 关键字”，而是先把问题抽象成 **不变的逻辑 + 可替换的类型参数**，再让编译器在编译期生成对应版本的代码。