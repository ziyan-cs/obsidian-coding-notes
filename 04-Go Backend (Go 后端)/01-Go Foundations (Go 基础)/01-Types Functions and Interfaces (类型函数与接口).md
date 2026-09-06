---
tags: [language/go, go/basics]
status: seed
review_due: 2026-09-12
confidence: 1
verified: 2026-09-05
---

# 01-Types Functions and Interfaces (类型函数与接口)

> [!abstract] 一句话结论：Go 用组合（composition）和小 interface 表达行为；interface 是隐式满足的，不是 C++ 的继承层级。

## 30 秒回答

**01-Types Functions and Interfaces (类型函数与接口)**：先说明它解决的问题，再解释一个关键机制、一个边界条件，并用最小示例或真实项目验证。


## 核心语法

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

## 接口的正确使用

接口应在**使用方**定义，并尽量小：例如 `io.Reader` 只需 `Read`。先写具体类型；当测试替身或多实现真实出现时再抽 interface。

```go
type UserStore interface { Find(context.Context, int64) (User, error) }
```

## 常见坑

- 将 `interface{}` / `any` 当作“泛型”；优先具体类型或真正的 generics。
- 混用值 receiver 和指针 receiver，导致某个类型不满足预期接口。
- 用 interface 模拟多层 OOP 继承；Go 更常用 struct 嵌入与函数组合。

## C++ 对照

| 问题 | C++ | Go |
| --- | --- | --- |
| 多态 | virtual / inheritance | 隐式 interface 实现 |
| 修改对象 | reference / pointer | `*T` receiver |
| 泛型 | template | type parameter（仅需要时） |
| 可见性 | `public/private` | 首字母大小写 + package |

## 自测

1. 为什么 `*User` 可能实现 interface，而 `User` 不实现？
2. 哪种情况下该先写 interface，哪种情况下不该？

## 常见误区

- 只记结论或 API 名称，却没有说明前提、失败模式和替代方案。
- 在没有最小代码、测试、测量或项目现象的情况下，把理解误当成掌握。

## 学习闭环

### 复述

- 不看正文，说明 01-Types Functions and Interfaces (类型函数与接口) 的问题、核心机制与边界。

### 验证

- 写一个最小示例、测试用例或项目观察点，验证其中一个关键行为。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下会失效、变慢或需要替代方案？

## 学习闭环

### 复述

- 不看正文，说清本主题的问题、核心机制和适用边界。

### 验证

- 通过代码、测试、压测或项目现象验证一个关键结论。

### 自测

1. 这个主题解决什么问题？
2. 它在什么条件下需要替代方案？
