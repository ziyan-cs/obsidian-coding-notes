#cpp #enum #enum-class #type-safety #named-constants

## ⚡ TL;DR（快速决策）

- 枚举本质是：**给一组离散常量起有意义的名字**
- 一看到这些场景，要优先想到枚举：
    - 状态值有限
    - 选项固定
    - 不想再用魔法数字
- 现代 C++ 更推荐：`enum class`
- 原因是：
    - 类型更安全
    - 作用域更清晰

## 🧩 Core Idea（核心本质）

- `enum` 用来定义一组具名常量
- `enum class` 是更现代、更安全的强类型枚举
- 一句话理解：
    - **枚举就是把“几个固定选项”变成有名字的类型。**
- 好处：
    - 可读性更强
    - 少写错数字含义
    - 更利于表达业务状态

## 🔧 Usage Patterns（可复用代码模板）

1. 传统枚举

```cpp
enum Color { Red, Green, Blue };
```

1. 强类型枚举

```cpp
enum class Status {
    Idle,
    Running,
    Finished
};
```

1. 使用 `enum class`

```cpp
Status s = Status::Running;
```

1. 显式转整数

```cpp
int x = static_cast<int>(Status::Running);
```

## ⚠️ Pitfalls（高频错误）

- 传统 `enum` 名字容易污染外层作用域
- 忘记 `enum class` 访问要写成 `Status::Running`
- 乱把枚举和整数混用
- 以为枚举只能做“整数别名”，忽略其建模价值

## 🚀 Performance / Tips（性能优化）

- 枚举重点是语义清晰和类型安全
- 状态有限、分支清晰的问题很适合枚举
- 在现代 C++ 中，除非兼容旧代码，一般优先 `enum class`

## 🧪 Common Scenarios（常见使用场景）

- 状态机
- 颜色 / 方向 / 模式等固定选项
- 配置项常量
- switch 分支控制

## 🧾 Minimal Template（最小可运行模板）

```cpp
#include <iostream>
using namespace std;

enum class Status { Idle, Running, Done };

int main() {
    Status s = Status::Running;
    cout << static_cast<int>(s) << '\\n';
    return 0;
}
```

## 📌 One-liner Summary（一句话总结）

- **枚举就是：把一组固定选项表示成有名字、可读性更强的类型。**