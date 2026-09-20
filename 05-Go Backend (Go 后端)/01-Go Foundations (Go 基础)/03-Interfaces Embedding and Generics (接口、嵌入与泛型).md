---
study_stage: backlog
tags: [language/go, go/basics]
---

> [!abstract] 学习定位
> Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。

> [!summary] 核心摘要
>
> 先用 struct 表达数据，用函数表达操作；当调用方只依赖少数行为时，在消费者一侧定义小 interface，让具体类型隐式满足。

# 核心语法

```go
type User struct { ID int64; Name string }

func (u User) Greeting() string { return "hi, " + u.Name }      // 值 receiver
func (u *User) Rename(name string) { u.Name = name }             // 指针 receiver

type Greeter interface { Greeting() string }

func printGreeting(g Greeter) { fmt.Println(g.Greeting()) }
```

- 大写开头的标识符（`User`）对包外可见（exported）；小写仅包内可见。
- `:=` 用于函数内“至少一个新变量”的短声明；公开 API 中别为了短而牺牲清晰。
- map、slice、channel 是引用到运行时结构的值；复制它们不等于复制全部元素。

# 接口的正确使用

接口应在**使用方**定义，并尽量小：例如 `io.Reader` 只需 `Read`。先写具体类型；当测试替身或多实现真实出现时再抽 interface。

```go
type UserStore interface { Find(context.Context, int64) (User, error) }
```

> [!warning]- 易错点
> - 将 `interface{}` / `any` 当作“泛型”；优先具体类型或真正的 generics。
> - 混用值 receiver 和指针 receiver，导致某个类型不满足预期接口。
> - 用 interface 模拟多层 OOP 继承；Go 更常用 struct 嵌入与函数组合。

# 接口值与 typed nil

接口值可理解为动态类型与动态值的组合。只有两者都为空时接口才等于 nil；装入 typed nil pointer 后，接口本身不为 nil。

```go
type MyError struct{ Message string }
func (e *MyError) Error() string { return e.Message }

var concrete *MyError = nil
var err error = concrete
fmt.Println(err == nil) // false
```

因此返回 error 时不要把 nil 的具体指针装进接口。调用接口方法也要考虑 receiver 是否能处理 nil；通常更简单的做法是只在真实错误存在时返回非 nil error。

# Method set 与 receiver

`T` 的 method set 包含值 receiver 方法；`*T` 包含值和指针 receiver 方法。因此只有指针 receiver 的类型通常由 `*T` 满足接口。编译器有时会为可寻址值自动取地址调用方法，但接口满足关系仍遵循 method set，不能混为一谈。

接口在消费者一侧定义，使依赖保持最小。返回具体类型通常比返回接口更灵活；参数接收所需的小接口。不要为了测试给每个 struct 预先配一个 `IType`。

# 嵌入与组合

嵌入字段会提升方法，外层类型仍不是内层类型的子类。提升方法出现重名时需要显式选择；内层方法接收到的 receiver 仍是内层值，不会动态分派到外层“覆盖”方法。

组合 HTTP handler、repository、clock 等依赖时，显式命名字段通常更清楚。嵌入更适合确实要把完整方法集提升为外层 API 的场景。

# Generics 的边界

泛型适合算法和容器在多种类型上保持同一逻辑，并且类型关系在编译期可表达。constraint 定义允许的类型集合或行为；`comparable` 只保证可用于 `==` 和 map key，不表示有序。

```go
func Contains[T comparable](items []T, target T) bool {
    for _, item := range items {
        if item == target { return true }
    }
    return false
}
```

若不同类型需要不同业务行为，小 interface 或普通函数更合适。不要用泛型隐藏反射、序列化和数据库字段差异；也不要为了消除两三行重复制造难懂 constraint。

Go 1.27 新增**具体类型的泛型方法**，但 interface 的方法仍不能自己声明类型参数，泛型方法也不能据此实现 interface 方法。初学时先掌握普通泛型函数与方法集，再读 [Go 1.27 发布说明](https://go.dev/doc/go1.27)；不要把旧资料中的“Go 方法绝不能有类型参数”当成现行规则。

# 实验

分别让 `User` 和 `*User` 赋值给一个小接口，观察编译结果；构造 typed nil error 并解释输出；最后写泛型 `Map` 或 `Contains`，比较它与 `[]any` 在类型安全和装箱语义上的差别。
# C++ 对照

| 问题 | C++ | Go |
| --- | --- | --- |
| 多态 | virtual / inheritance | 隐式 interface 实现 |
| 修改对象 | reference / pointer | `*T` receiver |
| 泛型 | template | type parameter（仅需要时） |
| 可见性 | `public/private` | 首字母大小写 + package |

> [!question]- 自测：先回答再展开
> 1. 为什么 `*User` 可能实现 interface，而 `User` 不实现？
> 2. 哪种情况下该先写 interface，哪种情况下不该？
