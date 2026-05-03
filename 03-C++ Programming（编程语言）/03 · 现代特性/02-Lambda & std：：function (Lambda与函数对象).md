#cpp #stl #lambda #functional #callable

## ⚡ TL;DR（快速决策）

- Lambda 的核心是：**在需要可调用对象的地方，就地定义匿名函数**
- 它是现代 C++ 中最重要、最常用的可调用对象表达方式之一
- 若只抓重点，应优先记住：
    - 捕获列表
    - 参数列表
    - 返回值推导
    - 在算法中的使用

## 🧩 Core Idea（核心本质）

- Lambda 解决的是“为了写一个短函数对象，不想单独定义类或函数”的问题
- 一句话理解：
    - **Lambda 是临时可调用对象的语法糖。**

## 🔧 典型写法

```cpp
auto f = [](int x) { return x * 2; };
```

```cpp
sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;
});
```

## 🔧 捕获列表

- `[=]`：值捕获所有变量
- `[&]`：引用捕获所有变量
- `[x]`：值捕获某变量
- `[&x]`：引用捕获某变量

## ⚠️ Pitfalls（高频错误）

- 捕获方式写错，导致值未更新或悬垂引用
- Lambda 写太长，反而可读性下降
- 在比较器中写出不符合严格弱序的逻辑

## 🚀 Performance / Tips（理解深化）

- Lambda 是 STL 算法最自然的搭配之一
- 刷题和工程里，只要逻辑局部、复用需求不强，优先考虑 Lambda

## 📌 One-liner Summary（一句话总结）

- **Lambda 的核心，是在使用现场快速定义可调用逻辑并直接接入 STL 算法。**
# Functional Utilities (函数对象补充)

---
01-仿函数.md
#cpp #stl #functional #functor #operator-call

## ⚡ TL;DR（快速决策）

- 仿函数的核心是：**把对象做成像函数一样可调用**
- 本质做法是重载 `operator()`
- 仿函数在 STL 中常用来作为算法的比较器、谓词或操作器

## 🧩 Core Idea（核心本质）

- 仿函数兼具“函数调用形式”和“对象可保存状态”的双重特性
- 一句话理解：
    - **仿函数是可调用对象。**

## 🗂️ STL 仿函数的三大分类

### 1. 算术仿函数（Arithmetic Function Objects）

- 作用：表达基础算术运算
- 常见成员：
    - `plus<T>`
    - `minus<T>`
    - `multiplies<T>`
    - `divides<T>`
    - `modulus<T>`
    - `negate<T>`
- 典型理解：
    - 它们把 `+ - * / %` 这类运算封装成“可传给算法的对象”

### 2. 关系仿函数（Relational Function Objects）

- 作用：表达大小比较与相等关系
- 常见成员：
    - `equal_to<T>`
    - `not_equal_to<T>`
    - `greater<T>`
    - `less<T>`
    - `greater_equal<T>`
    - `less_equal<T>`
- 典型理解：
    - 这一类最常出现在 `sort`、`set`、`priority_queue` 等需要比较规则的场景中

### 3. 逻辑仿函数（Logical Function Objects）

- 作用：表达逻辑判断运算
- 常见成员：
    - `logical_and<T>`
    - `logical_or<T>`
    - `logical_not<T>`
- 典型理解：
    - 它们把布尔逻辑也统一进“可调用对象”体系里

## 🔧 典型写法

```cpp
struct Cmp {
    bool operator()(int a, int b) const {
        return a > b;
    }
};
```

```cpp
sort(v.begin(), v.end(), Cmp());
```

内建仿函数示例：

```cpp
sort(v.begin(), v.end(), greater<int>());
int x = plus<int>()(2, 3);           // 5
bool ok = logical_and<bool>()(true, false);   // false
```

## 🚀 为什么 STL 喜欢仿函数

- 可内联
- 可携带状态
- 能作为模板参数高效参与泛型编程

## ⚠️ Pitfalls（高频错误）

- 把仿函数和普通函数完全等同
- 只知道自定义 `operator()`，却不知道 STL 已经提供了三大类内建仿函数
- 自定义比较器逻辑不满足严格弱序

## 📌 One-liner Summary（一句话总结）

- **仿函数的核心，是把对象设计成可像函数一样被调用，并用于 STL 泛型算法中。**
---
02-Lambda（⭐）.md
#cpp #stl #lambda #functional #callable

## ⚡ TL;DR（快速决策）

- Lambda 的核心是：**在需要可调用对象的地方，就地定义匿名函数**
- 它是现代 C++ 中最重要、最常用的可调用对象表达方式之一
- 若只抓重点，应优先记住：
    - 捕获列表
    - 参数列表
    - 返回值推导
    - 在算法中的使用

## 🧩 Core Idea（核心本质）

- Lambda 解决的是“为了写一个短函数对象，不想单独定义类或函数”的问题
- 一句话理解：
    - **Lambda 是临时可调用对象的语法糖。**

## 🔧 典型写法

```cpp
auto f = [](int x) { return x * 2; };
```

```cpp
sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;
});
```

## 🔧 捕获列表

- `[=]`：值捕获所有变量
- `[&]`：引用捕获所有变量
- `[x]`：值捕获某变量
- `[&x]`：引用捕获某变量

## ⚠️ Pitfalls（高频错误）

- 捕获方式写错，导致值未更新或悬垂引用
- Lambda 写太长，反而可读性下降
- 在比较器中写出不符合严格弱序的逻辑

## 🚀 Performance / Tips（理解深化）

- Lambda 是 STL 算法最自然的搭配之一
- 刷题和工程里，只要逻辑局部、复用需求不强，优先考虑 Lambda

## 📌 One-liner Summary（一句话总结）

- **Lambda 的核心，是在使用现场快速定义可调用逻辑并直接接入 STL 算法。**
---
03-Function（封装）.md
#cpp #stl #function #callable #wrapper

## ⚡ TL;DR（快速决策）

- `std::function` 的核心是：**统一封装不同类型的可调用对象**
- 它能装普通函数、Lambda、仿函数、成员函数绑定结果等
- 适合“接口层面需要统一可调用签名”的场景

## 🧩 Core Idea（核心本质）

- 模板能静态接收可调用对象，而 `std::function` 提供的是运行时统一封装
- 一句话理解：
    - **`std::function` 是可调用对象的类型擦除容器。**

## 🔧 典型写法

```cpp
function<int(int,int)> op = [](int a, int b) {
    return a + b;
};
cout << op(2, 3) << '\\n';
```

## 🧪 适用场景

- 回调函数封装
- 事件处理器列表
- 统一保存多种可调用对象

## ⚠️ Pitfalls（高频错误）

- 在极高性能场景滥用 `std::function`
- 把模板和 `std::function` 的职责混淆

## 🚀 Performance / Tips（理解深化）

- 若追求极致性能，模板参数往往更轻
- 若追求接口统一和灵活性，`std::function` 更方便

## 📌 One-liner Summary（一句话总结）

- **`std::function` 的核心，是把不同可调用对象统一封装成同一种函数签名接口。**
---
04-bind.md
#cpp #stl #bind #functional #callable

## ⚡ TL;DR（快速决策）

- `bind` 的核心是：**把一个可调用对象的部分参数预先绑定，生成新的可调用对象**
- 它常用于调整函数参数顺序、固定部分参数、适配回调接口
- 现代 C++ 中，很多场景也可由 Lambda 替代

## 🧩 Core Idea（核心本质）

- `bind` 相当于生成一个“参数改装版函数”
- 一句话理解：
    - **`bind` 是可调用对象适配器。**

## 🔧 典型写法

```cpp
auto f = bind(plus<int>(), 10, placeholders::_1);
cout << f(5) << '\\n';   // 15
```

## 🔧 常见用途

- 固定前几个参数
- 调整参数顺序
- 包装成员函数调用

## ⚠️ Pitfalls（高频错误）

- 占位符顺序写错
- 过度使用导致代码可读性下降
- 在简单场景中不用 Lambda 反而更绕

## 🚀 Performance / Tips（理解深化）

- 现代 C++ 里，简单绑定逻辑通常优先用 Lambda
- 学 `bind` 的重点，是理解“函数适配”思想，而不是把它当默认首选

## 📌 One-liner Summary（一句话总结）

- **`bind` 的核心，是通过预绑定参数生成更适合当前接口的新可调用对象。**