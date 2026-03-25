#cpp #using #typedef #alias

## ⚡ TL;DR（快速决策）

- `using` 和 `typedef` 的核心作用：**给已有类型起别名**
- 入门阶段最重要的是先分清：**它们不是新类型，只是旧类型的新名字**
- 现代 C++ 里，优先使用 `using`，因为它更直观、模板别名也更好写
- `typedef` 你必须会看懂，但新代码里通常更推荐 `using`
- 最容易错的不是“会不会写别名”，而是：**把别名当新类型、复杂指针写法看不懂、分不清 `using` 为什么更现代**
- 如果代码里类型很长，比如迭代器、函数指针、模板类型，别名会大幅提升可读性

## 🧩 Core Idea（核心本质）

- **本质**：类型别名就是给一个已经存在的类型起一个更短、更清晰、更好记的名字
- **关键机制**：编译器看到别名时，本质上仍把它当成原类型处理
- **核心价值**：减少冗长写法，提升可读性，方便维护和复用
- **一句理解**：别名不是“创造新类型”，而是“给旧类型贴一个更顺手的标签”

## 🔧 Usage Patterns（可复用代码模板）

### 1. `typedef` 基础写法

```cpp
typedef int Integer;
Integer x = 10;
```

### 2. `using` 基础写法

```cpp
using Integer = int;
Integer x = 10;
```

### 3. 给长类型起别名

```cpp
using ll = long long;
using vi = vector<int>;
```

### 4. 容器别名

```cpp
using pii = pair<int, int>;
using umii = unordered_map<int, int>;
```

### 5. 迭代器别名

```cpp
using Iter = vector<int>::iterator;
```

### 6. `typedef` 写指针别名

```cpp
typedef int* IntPtr;
IntPtr p;
```

### 7. `using` 写指针别名

```cpp
using IntPtr = int*;
IntPtr p;
```

### 8. 函数指针别名

```cpp
using Func = int(*)(int, int);
```

### 9. 模板别名（`using` 的优势）

```cpp
template <typename T>
using Vec = vector<T>;

Vec<int> nums;
```

### 10. 刷题常见别名

```cpp
using ll = long long;
using pii = pair<int, int>;
using vi = vector<int>;
using vvi = vector<vector<int>>;
```

## ⚠️ Pitfalls（高频错误）

- `using` 和 `typedef` **不会创建新类型**，只是原类型的别名
- `typedef int Integer;` 里的 `Integer` 本质还是 `int`
- `typedef` 看复杂类型时阅读方向不直观，初学者很容易绕晕
- 模板别名只能用 `using`，`typedef` 做不到同样自然的写法
- 不要为了“炫技巧”起过多缩写别名，太多反而降低可读性
- `using namespace std;` 和 `using ll = long long;` 不是一回事，不要混淆
- 别名只是简化书写，不会改变底层存储、大小、行为
- 在团队 / 工程代码里，别名名字要清晰，不要滥用难懂缩写
- `typedef` 在旧代码里很常见，所以你必须会读，但新写代码通常优先 `using`
- 指针、函数指针、模板类型是最容易因为写法复杂而需要别名的地方

## 🚀 Performance / Tips（性能优化）

- `using` 和 `typedef` 本身**几乎没有运行时性能差异**，它们的价值主要在可读性和维护性
- 现代 C++ 更推荐写：

```cpp
using ll = long long;
```

而不是：

```cpp
typedef long long ll;
```

因为 `using` 更像“左边是新名字，右边是原类型”，更符合直觉

- 高频刷题别名记住这几个就够用：

```cpp
using ll = long long;
using pii = pair<int, int>;
using vi = vector<int>;
using vvi = vector<vector<int>>;
```

- 如果类型特别长，先起别名再写逻辑，代码会清楚很多
- 高频记忆规则：

```cpp
typedef OldType NewName;
using NewName = OldType;
```

- 一句话选型：

```cpp
能用 using 就优先 using
看旧代码时要会读 typedef
```

## 🧪 Common Scenarios（常见使用场景）

- **简化 `long long`**：`using ll = long long;`
- **简化容器类型**：`using vi = vector<int>;`
- **简化二维容器类型**：`using vvi = vector<vector<int>>;`
- **简化键值对类型**：`using pii = pair<int, int>;`
- **简化哈希表类型**：`using umii = unordered_map<int, int>;`
- **模板封装时定义通用别名**：`template <typename T> using Vec = vector<T>;`
- **旧项目 / 老教程代码阅读**：常见 `typedef`
- **函数指针、迭代器等复杂类型简化**：优先考虑别名

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using pii = pair<int, int>;
using vi = vector<int>;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	ll x = 10000000000LL;
	pii p = {1, 2};
	vi nums = {1, 2, 3};

	cout << x << '\n';
	cout << p.first << ' ' << p.second << '\n';
	cout << nums.size() << '\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 别名的核心不是“把类型改个名字”，而是把 **冗长类型写短、复杂类型写清、代码语义写顺** 这几件事处理好，让代码更易读、更易写、更易维护。