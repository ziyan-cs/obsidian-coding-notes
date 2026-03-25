#cpp #basics #enum #enum-class 

## ⚡ TL;DR（快速决策）

- **枚举 `enum`**：把一组相关常量名字组织在一起
- **枚举类 `enum class`**：更现代、更安全、作用域更清晰
- 入门阶段最重要的是先分清：**普通枚举会把名字直接暴露出来，`enum class` 不会**
- 现代 C++ 里，如果没有特殊兼容需求，优先想到 `enum class`
- 枚举最容易错的不是“会不会写”，而是：**作用域、和整数的关系、能不能直接比较 / 输出 / 转换**
- 如果你想表达“状态 / 选项 / 类型类别”，枚举通常比一堆魔法数字更清楚

## 🧩 Core Idea（核心本质）

- **本质**：枚举是“给一组整数常量起有意义名字”
- **关键机制**：把离散状态统一放到一个类型里，避免到处写裸数字
- **核心价值**：提升可读性，让代码更容易理解和维护
- **一句理解**：与其写 `0、1、2`，不如写成 `Red、Green、Blue` 这种有语义的名字

## 🔧 Usage Patterns（可复用代码模板）

### 1. 最基础枚举 `enum`

```cpp
enum Color {
	Red,
	Green,
	Blue
};
```

### 2. 使用普通枚举

```cpp
Color c = Red;
if (c == Green) {
	cout << "green" << '\\n';
}
```

### 3. 指定枚举值

```cpp
enum Status {
	Pending = 0,
	Running = 1,
	Done = 2
};
```

### 4. 枚举值自动递增

```cpp
enum Level {
	Low = 1,
	Medium,
	High
};
// Medium = 2, High = 3
```

### 5. `enum class` 基础写法

```cpp
enum class Color {
	Red,
	Green,
	Blue
};
```

### 6. 使用 `enum class`

```cpp
Color c = Color::Red;
if (c == Color::Green) {
	cout << "green" << '\\n';
}
```

### 7. 指定底层类型

```cpp
enum class ErrorCode : int {
	Ok = 0,
	NotFound = 404,
	ServerError = 500
};
```

### 8. 转成整数

```cpp
enum class Color { Red, Green, Blue };
int x = static_cast<int>(Color::Green);
```

### 9. switch 配合枚举

```cpp
enum class Direction { Up, Down, Left, Right };

Direction d = Direction::Left;
switch (d) {
	case Direction::Up:
		cout << "up";
		break;
	case Direction::Down:
		cout << "down";
		break;
	case Direction::Left:
		cout << "left";
		break;
	case Direction::Right:
		cout << "right";
		break;
}
```

### 10. 用枚举表达状态

```cpp
enum class State {
	Idle,
	Running,
	Finished
};

State s = State::Running;
```

## ⚠️ Pitfalls（高频错误）

- 普通 `enum` 的枚举名会直接进入外层作用域，容易和别的名字冲突
- `enum class` 必须写成 `Color::Red` 这种形式，不能直接写 `Red`
- `enum class` 不会像普通 `enum` 那样轻易隐式转成整数
- 想把 `enum class` 转成整数，通常要显式写 `static_cast<int>(x)`
- 不要把枚举当成普通整数乱加乱减，语义会变差
- `switch` 写枚举时，仍然要注意 `break`
- 如果枚举值有明确业务含义，最好显式赋值，不要全靠默认递增
- 不要把“枚举是类型”和“枚举值是整数常量”这两层概念混为一谈
- 输出枚举时不能直接得到名字，默认拿到的通常只是整数值或需要自己映射
- 刷题 / 工程里，枚举是为了表达语义，不是为了炫语法

## 🚀 Performance / Tips（性能优化）

- 表达状态、模式、方向、类别时，优先考虑枚举，而不是直接写魔法数字：

```cpp
0
1
2
```

- 现代 C++ 更推荐：

```cpp
enum class Xxx { ... };
```

因为它更安全、作用域更清楚

- 高频记忆规则：

```cpp
enum        // 旧式枚举，名字容易污染作用域
enum class  // 新式枚举，更安全
```

- 如果题目 / 代码里需要和整数交互，常见写法是：

```cpp
static_cast<int>(EnumValue)
```

- 和 `switch` 搭配时最自然：

```cpp
switch (state) {
	...
}
```

- 工程里常见用途：

```cpp
状态机
错误码
方向
权限等级
任务状态
```

## 🧪 Common Scenarios（常见使用场景）

- **表示颜色、方向、状态**：枚举 / 枚举类
- **替代魔法数字**：让 `0/1/2` 变成有名字的状态
- **配合 `switch` 做分支处理**：代码更清楚
- **表示错误码 / 结果类型**：`enum class ErrorCode`
- **面向对象或项目代码里表达类别**：`enum class`
- **题目里状态数量固定且离散**：优先考虑枚举

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <bits/stdc++.h>
using namespace std;

enum class Status {
	Idle,
	Running,
	Done
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Status s = Status::Running;

	if (s == Status::Running) {
		cout << "running" << '\\n';
	}

	cout << static_cast<int>(s) << '\\n';

	return 0;
}
```

## 📌 One-liner Summary（一句话总结）

👉 C++ 枚举的核心不是“会写几个标签名”，而是把 **离散状态、固定选项、类别语义** 这些信息明确表达出来，让代码少一些魔法数字，多一些可读性和类型安全。